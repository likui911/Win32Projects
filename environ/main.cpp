#include <Windows.h>

#define ID_LIST 1
#define ID_STATIC 2

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	static WCHAR szClsName[] = TEXT("Environ Window");
	WNDCLASS wndcls{};
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.lpfnWndProc = WndProc;
	wndcls.lpszClassName = szClsName;
	wndcls.hInstance = hInstance;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, TEXT("Register Failed"), TEXT("1"), MB_OK);
		return -1;
	}
	HWND hwnd = CreateWindow(szClsName, TEXT("ENVIRON"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void FillListBox(HWND hwndList)
{
	int iLength;
	WCHAR *pEnvironStr;
	WCHAR *pVarBlock, *pVarBegin, *pVarEnd,*pVarName;
	
	pEnvironStr = GetEnvironmentStrings();
	pVarBlock = pEnvironStr;

	while (*pVarBlock)
	{
		if (*pVarBlock != '=')
		{
			pVarBegin = pVarBlock;
			while (*pVarBlock++ != '=');
			pVarEnd = pVarBlock - 1;
			iLength = pVarEnd - pVarBegin;

			pVarName = (WCHAR*)calloc(iLength + 1, sizeof(WCHAR));
			CopyMemory(pVarName, pVarBegin, iLength * sizeof(WCHAR));
			pVarName[iLength] = TEXT('\0');

			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)pVarName);
			free(pVarName);

		}
		while (*pVarBlock++ != '\0');
	}
	FreeEnvironmentStrings(pEnvironStr);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndList, hwndStatic;
	int cxChar, cyChar, iIndex,iLength;
	WCHAR *pVarName,*pVarValue;

	switch (uMsg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		hwndList = CreateWindow(TEXT("listbox"), NULL,
			WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER,
			cxChar, cyChar * 3, cxChar * 40 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 15,
			hwnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		hwndStatic = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, (HMENU)ID_STATIC, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		FillListBox(hwndList);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST)
		{
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:

				iIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
				iLength=SendMessage(hwndList, LB_GETTEXTLEN, iIndex, 0)+1;
				pVarName =(WCHAR*)calloc(iLength, sizeof(WCHAR));
				SendMessage(hwndList, LB_GETTEXT, iIndex, (LPARAM)pVarName);

				iLength = GetEnvironmentVariable(pVarName, NULL, 0);
				pVarValue = (WCHAR*)calloc(iLength, sizeof(WCHAR));
				GetEnvironmentVariable(pVarName, pVarValue, iLength);

				SetWindowText(hwndStatic, pVarValue);
				free(pVarName);
				free(pVarValue);
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
