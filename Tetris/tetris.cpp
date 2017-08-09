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

class Tetris :public BaseWindow<Tetris>
{
public:
	Tetris() :pFactory{ NULL }, pRenderTarget{ NULL }, pColorBrush{ NULL } {}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT CreateGraphicsResource();

	void OnPaint();
	void OnResize();

	PCWSTR ClassName() const { return TEXT("Tetris"); }
private:
	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> pColorBrush;

};

LRESULT Tetris::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		
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

void Tetris::OnPaint()
{
	HRESULT hr = CreateGraphicsResource();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::WhiteSmoke, 1.0f));

		

		pRenderTarget->EndDraw();

		EndPaint(m_hwnd, &ps);
	}
}

void Tetris::OnResize()
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

HRESULT Tetris::CreateGraphicsResource()
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
			D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Orange, 1.0f);
			pRenderTarget->CreateSolidColorBrush(color, &pColorBrush);

		}
	}
	return hr;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Tetris tetris;
	if (!tetris.Create(TEXT("Tetris"),
		WS_OVERLAPPEDWINDOW | WS_EX_LAYERED, 200, 20, 518, 700))
	{
		return 0;
	}

	ShowWindow(tetris.Window(), nCmdShow);

	//run message handle
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


/*
形状 
Z 
1,1,0
0,1,1

L 
2,2,2
2,0,0

J
3,3,3
0,0,3

I 
4,4,4,4

O 
5,5
5,5

T 
6,6,6
0,6,0

S
0,7,7
7,7,0

初始状态
{
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
}
一段时间后
{
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{2,2,3,3,1,1,0,0,0,0},
{2,1,1,3,1,1,7,0,0,0},
{2,1,1,3,6,0,7,0,0,2},
{0,4,5,5,6,6,7,0,0,2},
{4,4,4,5,5,6,7,0,2,2},
}
*/

//square 小格子
//shape 存储各种形状
//previousShape 前一个形状
//currentShape  当前形状
//grid 网格

//todo 俄罗斯方块