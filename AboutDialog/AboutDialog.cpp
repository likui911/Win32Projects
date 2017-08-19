// menudemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AboutDialog.h"

TCHAR szAppName[] = TEXT("AboutDialog");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int iCurrentColor = IDC_BLACK;
int iCurrentFigure = IDC_RECT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	HACCEL hAcc;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wndclass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ABOUT_DIALOG));
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_ABOUTDIALOG);
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Menu Demonstration"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ABOUTACCEL));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAcc, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
void PaintWindow(HWND hwnd, int iColor, int iFigure)
{
	static COLORREF crColor[8] = { RGB(0, 0, 0), RGB(0, 0, 255),
		RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255, 0, 0), RGB(255, 0, 255),
		RGB(255, 255, 0), RGB(255, 255, 255) };
	HBRUSH hBrush;
	HDC hdc;
	RECT rect;
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (iFigure == IDC_RECT)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}
void PaintTheBlock(HWND hCtrl, int iColor, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
				MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, AboutWndProc);
			break;
		}
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static int iColor, iFigure;
	switch (message)
	{
	case WM_INITDIALOG:
		iColor = iCurrentColor;
		iFigure = iCurrentFigure;
		CheckRadioButton(hwnd, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hwnd, IDC_RECT, IDC_ELLIPSE, iFigure);

		hCtrlBlock = GetDlgItem(hwnd, IDC_PAINT);
		SetFocus(GetDlgItem(hwnd, iColor));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor = iColor;
			iCurrentFigure = iFigure;
			EndDialog(hwnd, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			return TRUE;

		case IDC_BLACK:
		case IDC_BLUE:
		case IDC_GREEN:
		case IDC_CYAN:
		case IDC_RED:
		case IDC_MAGENTA:
		case IDC_YELLOW:
		case IDC_WHITE:
			iColor = LOWORD(wParam);
			CheckRadioButton(hwnd, IDC_BLACK, IDC_WHITE, iColor);
			PaintTheBlock(hwnd, iColor, iFigure);
			return TRUE;

		case IDC_RECT:
		case IDC_ELLIPSE:
			iFigure = LOWORD(wParam);
			CheckRadioButton(hwnd, IDC_RECT, IDC_ELLIPSE, iFigure);
			PaintTheBlock(hwnd, iColor, iFigure);
			return TRUE;
		}
		break;
	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, iColor, iFigure);
		break;
	}
	return FALSE;
}
