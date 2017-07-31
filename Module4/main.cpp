#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1")
#include <atlbase.h>
#include "basewin.h"
class MainWindow : public BaseWindow<MainWindow>
{
	CComPtr<ID2D1Factory>            pFactory;
	CComPtr<ID2D1HwndRenderTarget>   pRenderTarget;
	CComPtr<ID2D1SolidColorBrush>    pBrush;
	D2D1_ELLIPSE                     ellipse;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    OnPaint();
	void    Resize();

public:

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
	}

	PCWSTR  ClassName() const { return TEXT("Circle Window Class"); }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;
	if (!win.Create(TEXT("Module 4"), WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	ShowWindow(win.Window(), nCmdShow);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDBLCLK:
		MessageBox(m_hwnd, TEXT(""), TEXT(""), MB_OK);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}