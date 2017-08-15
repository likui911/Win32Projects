// menudemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "menudemo.h"

#define ID_TIMER 1
TCHAR szAppName[] = TEXT("MenuDemo");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	HACCEL hAccel;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wndclass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_MENUDEMO);
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

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MENUDEMO));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int idColor[5] = { WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH,
		DKGRAY_BRUSH, BLACK_BRUSH };
	static int iSelection = IDM_BKGND_WHITE;
	HMENU hMenu;
	static HMENU hPopMenu;
	POINT point;
	switch (message)
	{
	case WM_CREATE:
		hPopMenu = LoadMenu((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDC_POPMENU));
		hPopMenu = GetSubMenu(hPopMenu, 0);
		return 0;

	case WM_RBUTTONDOWN:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		ClientToScreen(hwnd, &point);
		TrackPopupMenu(hPopMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
		return 0;

	case WM_COMMAND:
		hMenu = GetMenu(hwnd);
		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
		case ID_FILE_OPEN:
		case ID_FILE_SAVE:
		case ID_FILE_SAVE_AS:
			MessageBeep(0);
			return 0;

		case IDR_MENU_COPY:
		case IDR_MENU_CUT:
		case IDR_MENU_PASTE:
			MessageBeep(0);
			return 0;

		case IDM_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		case ID_EDIT_UNDO:
		case ID_EDIT_CUT:
		case ID_EDIT_COPY:
		case ID_EDIT_PASTE:
		case ID_EDIT_CLEAR:
			MessageBeep(0);
			return 0;

		case IDM_BKGND_WHITE:
		case IDM_BKGND_LTGRAY:
		case IDM_BKGND_GRAY:
		case IDM_BKGND_DKGRAY:
		case IDM_BKGND_BLACK:
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);
			iSelection = LOWORD(wParam);//低位菜单ID,高位状态
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND,
				(LONG)GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case IDM_TIMER_START:
			if (SetTimer(hwnd, ID_TIMER, 1000, NULL))
			{
				EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);
			}
			return 0;

		case IDM_TIMER_STOP:
			KillTimer(hwnd, ID_TIMER);
			EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);
			return 0;

		case IDM_HELP:
			MessageBox(hwnd, TEXT("Help not yet implemented!"),
				szAppName, MB_ICONEXCLAMATION | MB_OK);
			return 0;

		case IDM_ABOUT:
			DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
				MAKEINTRESOURCE(IDM_APP_ABOUT), hwnd, AboutWndProc);
			return 0;
		}
		break;
	case WM_TIMER:
		MessageBeep(0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}