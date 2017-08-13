#include <Windows.h>

#define ID_LIST 1
#define ID_STATIC 2
#define MAXREAD 8192
#define DIRATTR (DDL_READWRITE|DDL_READONLY|DDL_HIDDEN|DDL_SYSTEM|\
					DDL_DIRECTORY|DDL_ARCHIVE|DDL_DRIVES)
#define DTFLAGS (DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

WNDPROC oldProc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	static WCHAR szClsName[] = TEXT("HEAD Window");
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
	HWND hwnd = CreateWindow(szClsName, TEXT("HEAD"), WS_OVERLAPPEDWINDOW,
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bValidFile;
	static CHAR  buffer[MAXREAD];
	static HWND hwndList, hwndStatic;
	HDC hdc;
	static RECT rect;
	int i, cxChar, cyChar;
	static WCHAR szBuffer[MAX_PATH + 1];
	static WCHAR szFile[MAX_PATH + 1];
	HANDLE hFile;
	PAINTSTRUCT ps;
	DWORD iWords;

	switch (uMsg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		hwndList = CreateWindow(TEXT("listbox"), NULL,
			WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER,
			cxChar, cyChar * 3, cxChar * 40 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 15,
			hwnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		// 获取当前文件夹
		GetCurrentDirectory(MAX_PATH, szBuffer);

		hwndStatic = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, (HMENU)ID_STATIC, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		// 绑定新的回调函数
		oldProc = (WNDPROC)SetWindowLongPtr(hwndList, GWLP_WNDPROC, (LPARAM)ListProc);
		//LB_DIR 当前目录下的文件
		SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
		return 0;

	case WM_SIZE:
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST&&HIWORD(wParam) == LBN_DBLCLK)
		{
			if (LB_ERR == (i = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0)))
				break;
			SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)szFile);

			// 判断是否为文件
			if (INVALID_HANDLE_VALUE != (
				hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
			{
				CloseHandle(hFile);
				bValidFile = TRUE;

				GetCurrentDirectory(MAX_PATH + 1, szBuffer);
				if (szBuffer[lstrlen(szBuffer) - 1] != '\\')
					lstrcat(szBuffer, TEXT("\\"));
				SetWindowText(hwndStatic, lstrcat(szBuffer, szFile));
			}
			else//文件夹
			{
				bValidFile = FALSE;
				//根目录 [-c-]
				if (szFile[1] == '-')
				{
					szFile[3] = ':';
					szFile[4] = '\\';
					szFile[lstrlen(szFile)] = '\0';
					lstrcpy(szBuffer, szFile + 2);

				}
				else//文件夹 [TEST]
				{
					szFile[lstrlen(szFile) - 1] = '\0';
					GetCurrentDirectory(MAX_PATH + 1, szBuffer);
					if (szBuffer[lstrlen(szBuffer) - 1] != '\\')
						lstrcat(szBuffer, TEXT("\\"));
					lstrcat(szBuffer, szFile + 1);
				}
				SetCurrentDirectory(szBuffer);
				SetWindowText(hwndStatic, szBuffer);

				SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
				SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
			}
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		if (!bValidFile)
			break;

		if (INVALID_HANDLE_VALUE == (hFile = CreateFile(szFile,
			GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
		{
			bValidFile = FALSE;
			break;
		}

		ReadFile(hFile, buffer, MAXREAD, &iWords, NULL);
		CloseHandle(hFile);

		hdc = BeginPaint(hwnd, &ps);
		//设置字体、颜色、背景
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
		// 这里假设是ASCII编码
		DrawTextA(hdc, buffer, (int)iWords, &rect, DTFLAGS);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN&&wParam == VK_RETURN)
	{
		// 回车键发送鼠标双击事件给父窗口
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(ID_LIST, LBN_DBLCLK), (LPARAM)hwnd);
	}
	// 调用系统回调函数
	return CallWindowProc(oldProc, hwnd, uMsg, wParam, lParam);
}