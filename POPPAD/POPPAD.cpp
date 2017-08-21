// POPPAD.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "POPPAD.h"

#define EDIT_ID 1

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
	static BOOL bNeedSave = FALSE;
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];

	switch (msg)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hwndEdit = CreateWindow(TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_MULTILINE |
			ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, 0, 0,
			hwnd, (HMENU)EDIT_ID, hInst, NULL);
		SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);

		PopFileInitialize(hwnd);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_COMMAND:
		if (lParam&&LOWORD(wParam) == EDIT_ID)
		{
			switch (HIWORD(wParam))
			{
			case EN_UPDATE:
				bNeedSave = TRUE;
				return 0;
			}
		}

		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
			if (bNeedSave&&IDCANCEL == AskForSave(hwnd, szTitleName))
			{
				return 0;
			}
			SetWindowText(hwndEdit, TEXT("\0"));
			szFileName[0] = '\0';
			szTitleName[0] = '0';
			bNeedSave = FALSE;
			return 0;

		case ID_FILE_OPEN:
			if (bNeedSave&&IDCANCEL == AskForSave(hwnd, szTitleName))
			{
				return 0;
			}
			return 0;

		case ID_FILE_SAVE:
			
			return 0;

		case ID_FILE_SAVEAS:

			return 0;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, AboutWndProc);
			return 0;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hwnd, TRUE);
		}
		break;
	}
	return FALSE;

}

int AskForSave(HWND hwnd, WCHAR* szTitleName)
{

	TCHAR szBuffer[64 + MAX_PATH];
	int iReturn;
	wsprintf(szBuffer, TEXT("Save current changes in %s?"), szTitleName[0] ? szTitleName : UNTITLED);
	iReturn = MessageBox(hwnd, szBuffer, szAppName, MB_YESNOCANCEL | MB_ICONQUESTION);
	if (iReturn == IDYES)
	{
		if (!SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0))
			return IDCANCEL;
	}
	return iReturn;
}

void PopFileInitialize(HWND hwnd)
{
	TCHAR *szFileFilter   = TEXT("Text Files(*TXT)\0*.txt\0All Files(*.*)\0*.*\0\0");
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = szFileFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = NULL;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = NULL;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = 0;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = TEXT("txt");
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
}