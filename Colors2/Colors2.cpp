#include "stdafx.h"
#include "Resource.h"
#include <Windows.h>

TCHAR szAppClass[] = TEXT("COLOR2");
HWND hDlgModeless;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ColorScrDlg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	MSG msg;
	HWND hwnd;
	HACCEL hAccel;
	WNDCLASS wndclass{};
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_COLORS2), IMAGE_ICON,
		SM_CXICON, SM_CYICON, LR_DEFAULTCOLOR);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_COLORS2);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppClass;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Failed"), TEXT("Warning"), MB_OK);
	}

	hwnd = CreateWindow(szAppClass, TEXT(""), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	hDlgModeless = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), hwnd, ColorScrDlg);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COLORS2));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hDlgModeless == 0 || !IsDialogMessage(hDlgModeless, &msg))
		{
			if (!TranslateAccelerator(hwnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EDIT:
			ShowWindow(hDlgModeless, SW_SHOW);
			SendMessage(hDlgModeless, WM_COMMAND, MAKELONG(IDM_EDIT, 0),
				(LONG)GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND));
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK ColorScrDlg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int colors[3];
	HWND hParent, hCtrl;
	int iCtrlID, iIndex;
	HBRUSH hOldBrush;
	LOGBRUSH lgbrush;
	COLORREF currentColor;

	switch (msg)
	{
	case WM_INITDIALOG:
		for (int i = IDC_SCROLLBAR1; i <= IDC_SCROLLBAR3; ++i)
		{
			hCtrl = GetDlgItem(hwnd, i);
			SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE);
			SetScrollPos(hCtrl, SB_CTL, 0, FALSE);
		}
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDM_EDIT)
		{
			GetObject((HBRUSH)lParam, sizeof(LOGBRUSH), &lgbrush);
			currentColor=lgbrush.lbColor;
			colors[0] = GetRValue(currentColor);
			colors[1] = GetGValue(currentColor);
			colors[2] = GetBValue(currentColor);
			for (int i = 0; i < 3; ++i)
			{
				hCtrl = GetDlgItem(hwnd, IDC_SCROLLBAR1 + i);
				SetScrollPos(hCtrl, SB_CTL, colors[i], FALSE);
				SetDlgItemInt(hwnd, IDC_STATIC1 + i, colors[i], FALSE);
			}
		}
		return TRUE;

	case WM_VSCROLL:
		hCtrl = (HWND)lParam;
		iCtrlID = GetWindowLongPtr(hCtrl, GWLP_ID);
		iIndex = iCtrlID - IDC_SCROLLBAR1;
		hParent = GetParent(hwnd);

		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			colors[iIndex] -= 1;
			break;
		case SB_LINEDOWN:
			colors[iIndex] += 1;
			break;
		case SB_PAGEDOWN:
			colors[iIndex] += 15;
			break;
		case SB_PAGEUP:
			colors[iIndex] -= 15;
			break;
		case SB_THUMBPOSITION:
			colors[iIndex] = HIWORD(wParam);
			break;
		}
		SetScrollPos(hCtrl, SB_CTL, colors[iIndex], TRUE);
		SetDlgItemInt(hwnd, IDC_STATIC1 + iIndex, colors[iIndex], FALSE);

		hOldBrush = (HBRUSH)SetClassLongPtr(hParent, GCLP_HBRBACKGROUND,
			(LONG)CreateSolidBrush(RGB(colors[0], colors[1], colors[2])));
		DeleteObject(hOldBrush);
		InvalidateRect(hParent, NULL, TRUE);

		return TRUE;

	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		return TRUE;
	}
	return FALSE;
}
