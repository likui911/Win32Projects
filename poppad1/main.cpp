#include <Windows.h>

#define ID_EDIT 1

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;
	switch (uMsg)
	{
	case WM_CREATE:
		hwndEdit = CreateWindow(TEXT("edit"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | 
			ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, 
			hwnd, (HMENU)ID_EDIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_SIZE:
		RECT rc;
		GetClientRect(hwnd, &rc);
		MoveWindow(hwndEdit, 0, 0, rc.right, rc.bottom, TRUE);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_EDIT)
		{
			switch (HIWORD(wParam))
			{
			case EN_ERRSPACE:
			case EN_MAXTEXT:
				MessageBox(hwnd, TEXT("Edit control out of space"), TEXT("POPPAD 1"), MB_ICONSTOP);
				break;
			case EN_KILLFOCUS:
				MessageBox(hwnd, TEXT("Edit lost focus"), TEXT("POPPAD 1"), MB_ICONSTOP);
				break;
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	static WCHAR szClsName[] = TEXT("Poppad Window");
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
	HWND hwnd = CreateWindow(szClsName, TEXT("POPPAD 1"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
