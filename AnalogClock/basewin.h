#pragma once
#include <Windows.h>

template<typename DERIVED_TYPE>
class BaseWindow
{
public:
	BaseWindow() :m_hwnd{ NULL } {}

	~BaseWindow() {}

	static LRESULT  CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DERIVED_TYPE* pThis = NULL;
		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
			pThis->m_hwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis)
		{
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	BOOL Create(LPCTSTR lpWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT,
		HWND hWndParent = NULL, HMENU hMenu = NULL, HINSTANCE hInstance = 0);
	
	HWND Window()const { return m_hwnd; }

protected:
	virtual PCWSTR  ClassName()const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	HWND m_hwnd;
};

template<class DERIVED_TYPE>
BOOL BaseWindow<DERIVED_TYPE>::Create( LPCTSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,
	HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
{
	WNDCLASS wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = (HCURSOR)LoadImage(
		NULL, IDC_ARROW, IMAGE_CURSOR, SM_CXCURSOR, SM_CYCURSOR, LR_VGACOLOR);
	wndclass.hIcon = (HICON)LoadImage(
		NULL, IDI_APPLICATION, IMAGE_ICON, SM_CXICON, SM_CYICON, LR_VGACOLOR);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = DERIVED_TYPE::WindowProc;
	wndclass.lpszClassName = ClassName();
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WndClass register failed"), TEXT("Analog Clock"), MB_OK);
		return FALSE;
	}

	m_hwnd = CreateWindow(ClassName(), lpWindowName, dwStyle, x, y,
		nWidth, nHeight, hWndParent, hMenu, hInstance, this);
	return (m_hwnd ? TRUE : FALSE);
}