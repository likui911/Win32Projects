#include <Windows.h>
#include <windowsx.h>

#define DIVISION 5
WCHAR szChildClass[] = TEXT("Checker_Child");
int idFocus = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASS wndcls{};
	wndcls.hInstance = hInstance;
	wndcls.lpfnWndProc = WndProc;
	wndcls.lpszClassName = TEXT("Checker");
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// register mainclass
	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, TEXT("register failed"), TEXT("Register"), MB_OK);
		return 0;
	}

	wndcls.lpfnWndProc = ChildWndProc;
	wndcls.hIcon = NULL;
	wndcls.lpszClassName = szChildClass;
	wndcls.cbWndExtra = sizeof(long);
	// register childclass
	RegisterClass(&wndcls);

	HWND hwnd = CreateWindow(TEXT("Checker"), TEXT("Checker"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, 0);
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
	static HWND hwndChild[DIVISION][DIVISION];
	switch (uMsg)
	{
	case WM_CREATE:
	{
		for (int x = 0; x < DIVISION; ++x)
		{
			for (int y = 0; y < DIVISION; ++y)
			{
				// For a child window, hMenu specifies the child-window identifier, an integer value 
				// used by a dialog box control to notify its parent about events.
				hwndChild[x][y] = CreateWindow(szChildClass, NULL, WS_CHILDWINDOW | WS_VISIBLE,
					0, 0, 0, 0, hwnd, (HMENU)(UINT_PTR)(y << 8 | x),
					(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
			}
		}
		return 0;
	}

	case WM_KEYDOWN:
	{
		int x = idFocus & 0xFF;
		int y = idFocus >> 8;
		switch (wParam)
		{
		case VK_UP:
			--y;
			break;
		case VK_DOWN:
			++y;
			break;
		case VK_LEFT:
			--x;
			break;
		case VK_RIGHT:
			++x;
			break;
		default:return 0;
		}
		x = (x + DIVISION) % DIVISION;
		y = (y + DIVISION) % DIVISION;
		idFocus = y << 8 | x;
		SetFocus(GetDlgItem(hwnd, idFocus));
	}
	return 0;

	case WM_SIZE:
	{
		int cxBlock = GET_X_LPARAM(lParam) / DIVISION;
		int cyBlock = GET_Y_LPARAM(lParam) / DIVISION;
		for (int x = 0; x < DIVISION; ++x)
		{
			for (int y = 0; y < DIVISION; ++y)
			{
				MoveWindow(hwndChild[x][y], x*cxBlock, y*cyBlock, cxBlock, cyBlock, TRUE);
			}
		}
		return 0;
	}

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hwnd, idFocus));
		return 0;

	case WM_LBUTTONDOWN:
		MessageBeep(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		SetWindowLong(hwnd, 0, 0);
		return 0;

	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0));
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	case WM_SETFOCUS:
		idFocus = GetWindowLong(hwnd, GWLP_ID);

	case WM_KILLFOCUS:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_RETURN || wParam == VK_SPACE)
		{
			SetFocus(hwnd);
			SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0));
			InvalidateRect(hwnd, NULL, FALSE);
		}
		else
		{
			SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
		}
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		if (GetWindowLong(hwnd, 0))
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, rect.right, rect.bottom);
			MoveToEx(hdc, rect.right, 0, NULL);
			LineTo(hdc, rect.left, rect.bottom);
		}
		if (hwnd == GetFocus())
		{
			rect.left += rect.right / 10;
			rect.right -= rect.left;
			rect.top += rect.bottom / 10;
			rect.bottom -= rect.top;

			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			SelectObject(hdc, CreatePen(PS_DASH, 1, 0));
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}