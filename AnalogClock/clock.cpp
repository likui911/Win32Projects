#include <atlbase.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1")
#include "basewin.h"

class AnalogClock :public BaseWindow<AnalogClock>
{
public:
	AnalogClock() :pFactory{ NULL }, pRenderTarget{ NULL }, pColorBrush{ NULL } {}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	HRESULT CreateGraphicsResource();
	void DrawDial();

	PCWSTR ClassName() const { return TEXT("Analog Clock"); }
private:
	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> pColorBrush;
};

LRESULT AnalogClock::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	case WM_SIZE:
		InvalidateRect(m_hwnd, NULL, FALSE);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

void AnalogClock::OnPaint()
{
	HRESULT hr = CreateGraphicsResource();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f));
		
		DrawDial();//表盘
		//todo 绘制指针
		
		pRenderTarget->EndDraw();

		EndPaint(m_hwnd, &ps);
	}
}

HRESULT AnalogClock::CreateGraphicsResource()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rect;
		GetClientRect(m_hwnd, &rect);

		D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);

		hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

		if (SUCCEEDED(hr))
		{
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Orange,1.0f);
			pRenderTarget->CreateSolidColorBrush(color, &pColorBrush);
		}
	}
	return hr;
}

void AnalogClock::DrawDial()
{
	//todo 绘制表盘椭圆，刻度
	pColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue, 1.0f));
	pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(100, 100), 50, 50),pColorBrush);

}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AnalogClock analog;
	if (!analog.Create(TEXT("Analog Clock"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300))
	{
		return 0;
	}

	ShowWindow(analog.Window(), nCmdShow);

	//run message handle
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


