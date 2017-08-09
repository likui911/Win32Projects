#include <atlbase.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1")
#include <dwrite.h>
#pragma comment(lib,"dwrite.lib")
#include "basewin.h"
#include <Windows.h>
#include <windowsx.h>
#include <Strsafe.h>

#define ID_TIMER 1

class AnalogClock :public BaseWindow<AnalogClock>
{
public:
	AnalogClock() :pFactory{ NULL }, pRenderTarget{ NULL }, pColorBrush{ NULL } {}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT CreateGraphicsResource();

	void OnPaint();
	void OnResize();
	void DrawDial();

	PCWSTR ClassName() const { return TEXT("Analog Clock"); }
private:
	void DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth);
	void DrawClockScale();
	void DrawClockNum();

	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> pColorBrush;
	CComPtr<ID2D1SolidColorBrush> pColorStroke;
	CComPtr<ID2D1SolidColorBrush> pColorNum;

	CComPtr<IDWriteFactory> pWriteFactory;
	CComPtr<IDWriteTextFormat> pWriteFormat;

	D2D1_ELLIPSE dialEllipse;

};

LRESULT AnalogClock::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		if (SUCCEEDED(hr))
		{
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED, __uuidof(pWriteFactory),
				reinterpret_cast<IUnknown**>(&pWriteFactory));
		}
		if (FAILED(hr))
		{
			return -1;//failed createWindowEx
		}
		SetTimer(m_hwnd, ID_TIMER, 1000, NULL);
		return 0;
	}

	case WM_TIMER:
		InvalidateRect(m_hwnd, NULL, FALSE);
		return 0;

	case WM_SIZE:
		OnResize();
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_DESTROY:
		KillTimer(m_hwnd, ID_TIMER);
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
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::WhiteSmoke, 1.0f));

		DrawDial();

		pRenderTarget->EndDraw();

		EndPaint(m_hwnd, &ps);
	}
}

void AnalogClock::OnResize()
{
	if (pRenderTarget != NULL)
	{
		RECT rect;
		GetClientRect(m_hwnd, &rect);

		D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
		pRenderTarget->Resize(size);

		InvalidateRect(m_hwnd, NULL, FALSE);
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
			hr = pWriteFactory->CreateTextFormat(TEXT("Consolas"), NULL, DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, TEXT(""), &pWriteFormat);
		}
		if (SUCCEEDED(hr))
		{
			D2D1_COLOR_F color1 = D2D1::ColorF(D2D1::ColorF::Orange, 1.0f);
			D2D1_COLOR_F color2 = D2D1::ColorF(D2D1::ColorF::Black, 1.0f);
			D2D1_COLOR_F color3 = D2D1::ColorF(D2D1::ColorF::Black, 1.0f);

			pRenderTarget->CreateSolidColorBrush(color1, &pColorBrush);
			pRenderTarget->CreateSolidColorBrush(color2, &pColorStroke);
			pRenderTarget->CreateSolidColorBrush(color3, &pColorNum);

			pWriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pWriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
	return hr;
}

void AnalogClock::DrawDial()
{
	pColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue, 1.0f));
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	const float a = (float)rect.right / 2;
	const float b = (float)rect.bottom / 2;
	const float radius = (float)5.0 / 6 * min(a, b);
	dialEllipse = D2D1::Ellipse(D2D1::Point2F(a, b), radius, radius);
	pRenderTarget->FillEllipse(dialEllipse, pColorBrush);

	DrawClockScale();

	SYSTEMTIME time;
	GetLocalTime(&time);
	const float fHourAngle = (360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f);
	const float fMinuteAngle = (360.0f / 60) * (time.wMinute);
	const float fSecondAngle = (360.0f / 60) * (time.wSecond);
	DrawClockHand(0.6f, fHourAngle, 4);
	DrawClockHand(0.75f, fMinuteAngle, 2);
	DrawClockHand(0.85f, fSecondAngle, 1);

	//Draw Nums
	DrawClockNum();
}

void AnalogClock::DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth)
{
	pRenderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(fAngle, dialEllipse.point));

	// endPoint defines one end of the hand.
	D2D_POINT_2F endPoint = D2D1::Point2F(
		dialEllipse.point.x, dialEllipse.point.y - (dialEllipse.radiusY * fHandLength));

	// Draw a line from the center of the ellipse to endPoint.
	pRenderTarget->DrawLine(
		dialEllipse.point, endPoint, pColorStroke, fStrokeWidth);

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void AnalogClock::DrawClockScale()
{
	D2D_POINT_2F begPoint = D2D1::Point2F(
		dialEllipse.point.x, dialEllipse.point.y - dialEllipse.radiusY*0.95f);
	D2D_POINT_2F endPoint = D2D1::Point2F(
		dialEllipse.point.x, dialEllipse.point.y - dialEllipse.radiusY);

	for (int i = 0; i < 60; ++i)
	{
		const float fAngle = (360.0f / 60) * i;
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(fAngle, dialEllipse.point));
		pRenderTarget->DrawLine(begPoint, endPoint, pColorStroke, 1.0f);
	}

	begPoint = D2D1::Point2F(
		dialEllipse.point.x, dialEllipse.point.y - dialEllipse.radiusY*0.9f);
	for (int i = 0; i < 12; ++i)
	{
		const float fAngle = (360.0f / 12) * i;
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(fAngle, dialEllipse.point));
		pRenderTarget->DrawLine(begPoint, endPoint, pColorStroke, 3.0f);
	}

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void AnalogClock::DrawClockNum()
{
	D2D1_POINT_2F point = D2D1::Point2F(dialEllipse.point.x, dialEllipse.point.y - dialEllipse.radiusY);
	for (int i = -2; i <= 9; ++i)
	{
		const float fAngle = 6.28f*i / 12;
		float x = (float)(dialEllipse.point.x + cos(fAngle)*(dialEllipse.radiusY*0.8));
		float y = (float)(dialEllipse.point.y + sin(fAngle)*(dialEllipse.radiusY*0.8));
		D2D1_RECT_F rect = D2D1::RectF(x - 20, y - 20,x + 20, y + 20);

		WCHAR szNum[] = TEXT("12");
		StringCbPrintf(szNum, sizeof(szNum),TEXT("%d"), i+3);
		pRenderTarget->DrawTextW(szNum, 2, pWriteFormat, rect, pColorNum);
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AnalogClock analog;
	if (!analog.Create(TEXT("Analog Clock"),
		WS_OVERLAPPEDWINDOW | WS_EX_LAYERED, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300))
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


