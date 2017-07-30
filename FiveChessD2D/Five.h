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

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    CreateFiveChessGrid();
	void    RenderChess();
	void    OnPaint();
	void    Resize();
	BOOL OnLeftButtonDown(LPARAM);
	void OnLeftButtonUp();
	BOOL HasChess(int xPosition, int yPosition);
	BOOL IsGameOver();
	void ReInitialization();

public:

	MainWindow() : pFactory{ NULL }, pRenderTarget{ NULL }, pBrush{ NULL },
		m_CentreX{ 0 }, m_CentreY{ 0 }, m_GridLen{ 0 }, m_topX{ 0 }, m_topY{ 0 },
		m_chess{ 0 }, m_IsBlack{ TRUE }, m_xPosition{0}, m_yPosition{0}
	{
	}

	PCWSTR  ClassName() const { return TEXT("Circle Window Class"); }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	float m_CentreX, m_CentreY, m_topX, m_topY;
	float m_GridLen;
	int m_chess[15][15];
	int m_xPosition, m_yPosition;
	BOOL m_IsBlack;
};