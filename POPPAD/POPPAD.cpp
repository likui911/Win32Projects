// POPPAD.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "POPPAD.h"

#define EDIT_ID 1


static WCHAR szAppName[] = TEXT("POPPAD");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int icmdShow)
{
	MSG        msg;
	HWND       hwnd;
	HACCEL     hacc;
	WNDCLASS   wndcls{};

	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR,
		SM_CXCURSOR, SM_CYCURSOR, LR_DEFAULTCOLOR);
	wndcls.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_POPPAD), IMAGE_ICON,
		SM_CXICON, SM_CYICON, LR_DEFAULTCOLOR);
	wndcls.lpfnWndProc = WndProc;
	wndcls.lpszClassName = szAppName;
	wndcls.lpszMenuName = MAKEINTRESOURCE(IDC_POPPAD);

	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, TEXT("Register Failed"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, szCmdLine);

	hacc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POPPAD));

	ShowWindow(hwnd, icmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hacc, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;
	static HINSTANCE hInst;
	switch (msg)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hwndEdit = CreateWindow(TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_MULTILINE |
			ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, 0, 0,
			hwnd, (HMENU)EDIT_ID, hInst, NULL);
		SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}