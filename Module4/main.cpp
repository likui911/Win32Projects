#include <Windows.h>
#include <windowsx.h>
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
	D2D1_POINT_2F					 ptMouse;

	HRESULT CreateGraphicsResources();
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp();
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void    OnPaint();
	void    Resize();

public:

	MainWindow() : pFactory{ NULL }, pRenderTarget{ NULL }, pBrush{ NULL },
		ellipse{ D2D1::Ellipse(D2D1::Point2F(),0,0) }, ptMouse{ D2D1::Point2F() }
	{
	}

	PCWSTR  ClassName() const { return TEXT("Circle Window Class"); }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};


HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;

	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);
		if (SUCCEEDED(hr))
		{
			pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &pBrush);
		}

	}
	return hr;
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	SetCapture(m_hwnd);
	ellipse.point = ptMouse = D2D1::Point2F((float)pixelX, (float)pixelY);
	ellipse.radiusX = ellipse.radiusY = 1.0f;
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnLButtonUp()
{
	ReleaseCapture();
}
void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	if (flags&MK_LBUTTON)
	{
		const D2D1_POINT_2F dips = D2D1::Point2F((float)pixelX, (float)pixelY);
		float width = (dips.x - ptMouse.x) / 2;
		float height;
		if (GetKeyState(VK_SHIFT) & 0x8000)//¼ì²âSHIFT¼üÊÇ·ñ°´ÏÂ
		{
			height = width;
		}
		else
		{
			height = (dips.y - ptMouse.y) / 2;
		}
		const float x1 = ptMouse.x + width;
		const float y1 = ptMouse.y + height;

		ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		pRenderTarget->Resize(size);

		InvalidateRect(m_hwnd, NULL, TRUE);
	}
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
		pRenderTarget->FillEllipse(ellipse, pBrush);

		pRenderTarget->EndDraw();
		EndPaint(m_hwnd, &ps);
	}

}

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
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;
		}
		return 0;

	case WM_SIZE:
		Resize();
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_LBUTTONDOWN:
	{
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	}

	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_SETCURSOR:
		// CHECK IF the cursor is over the client area of the window
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}