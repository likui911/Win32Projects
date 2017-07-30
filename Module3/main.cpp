#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1")
#include <atlbase.h>
#include "basewin.h"

//float g_DPIScaleX = 1.0f;
//float g_DPIScaleY = 1.0f;
//
//void InitializeDPIScale(ID2D1Factory *pFactory)
//{
//	FLOAT dpiX, dpiY;
//
//	pFactory->GetDesktopDpi(&dpiX, &dpiY);
//
//	g_DPIScaleX = dpiX / 96.0f;
//	g_DPIScaleY = dpiY / 96.0f;
//}
//
//template <typename T>
//float PixelsToDipsX(T x)
//{
//	return static_cast<float>(x) / g_DPIScaleX;
//}
//
//template <typename T>
//float PixelsToDipsY(T y)
//{
//	return static_cast<float>(y) / g_DPIScaleY;
//}

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

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y) * 5 / 6;
		ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius/2);
		//rotate the ellipse
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(20, D2D1::Point2F(x, y)));
	}
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Orange);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
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
		//remove the current transform
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		hr = pRenderTarget->EndDraw();
		// re-create the render target and all device-dependent resources
		if (hr == D2DERR_RECREATE_TARGET)
		{
			CreateGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
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
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(TEXT("Circle"), WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = {};
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
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;


	case WM_SIZE:
		Resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

