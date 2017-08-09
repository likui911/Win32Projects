#include <Windows.h>

#define WM_COLORCHANGE (WM_GETMINMAXINFO + 1)

#define COLOR_R 128
#define COLOR_G 128
#define COLOR_B 128

HWND hChild;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance = (HINSTANCE)(UINT_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE);
	static int colors[3] = { COLOR_R,COLOR_G,COLOR_B };
	static HWND scrolls[3] = { NULL };
	static HBRUSH hbr;
	static HBRUSH hbrClient;
	//static HBRUSH hBrush[3];
	switch (uMsg)
	{
	case WM_CREATE:
	{
		// create static label
		CreateWindow(TEXT("STATIC"), TEXT("red"), WS_VISIBLE | WS_CHILD | SS_SIMPLE,
			70, 30, 100, 20, hwnd, (HMENU)(UINT_PTR)7, hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("green"), WS_VISIBLE | WS_CHILD | SS_SIMPLE,
			150, 30, 100, 20, hwnd, (HMENU)(UINT_PTR)8, hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("blue"), WS_VISIBLE | WS_CHILD | SS_SIMPLE,
			230, 30, 100, 20, hwnd, (HMENU)(UINT_PTR)9, hInstance, 0);

		// create scrollbar
		for (int i = 0; i < 3; ++i)
		{
			scrolls[i] = CreateWindow(TEXT("SCROLLBAR"), NULL, WS_VISIBLE | WS_CHILD | SBS_VERT,
				70 + 80 * i, 60, 20, 250, hwnd, (HMENU)(UINT_PTR)i, hInstance, 0);
			SetScrollRange(scrolls[i], SB_CTL, 0, 255, FALSE);
			SetScrollPos(scrolls[i], SB_CTL, 128, FALSE);
		}

		return 0;
	}

	case WM_CTLCOLORSCROLLBAR://¹ö¶¯ÌõÑÕÉ«
	{
		int i = GetWindowLong((HWND)lParam, GWL_ID);

		COLORREF color;
		if (i == 0)
			color = RGB(colors[0], 0, 0);
		else if (i == 1)
			color = RGB(0, colors[1], 0);
		else if (i == 2)
			color = RGB(0, 0, colors[2]);

		//delete old brush
		DeleteObject(hbr);
		hbr = CreateSolidBrush(color);

		return (HRESULT)(UINT_PTR)hbr;
	}

	case WM_VSCROLL:
	{
		int id = GetWindowLong((HWND)(UINT_PTR)lParam, GWL_ID);

		switch (LOWORD(wParam))
		{
		case SB_PAGEUP:
			colors[id] -= 15;
			break;

		case SB_PAGEDOWN:
			colors[id] += 15;
			break;

		case SB_LINEUP:
			colors[id] = max(0, colors[id] - 1);
			break;

		case SB_LINEDOWN:
			colors[id] = min(255, colors[id] + 1);
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			colors[id] = HIWORD(wParam);
			break;
		}

		SetScrollPos(scrolls[id], SB_CTL, colors[id], TRUE);
		hbrClient = CreateSolidBrush(RGB(colors[0], colors[1], colors[2]));
		//send message to client window 
		SendMessage(hChild, WM_COLORCHANGE, 0, (UINT_PTR)hbrClient);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hbr = CreateSolidBrush(RGB(COLOR_R, COLOR_G, COLOR_B));
	switch (uMsg)
	{
	case WM_COLORCHANGE:
	{
		hbr = (HBRUSH)lParam;
		InvalidateRect(hChild, NULL, TRUE);
		return 0;
	}

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rc;
		GetClientRect(hwnd, &rc);
		FillRect(hdc, &rc, hbr);

		//delete old hbr
		DeleteObject(hbr);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	static WCHAR szClsName[] = TEXT("Color Scroll");
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
	HWND hwnd = CreateWindow(szClsName, TEXT("Color Scroll Window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 400,
		NULL, NULL, hInstance, NULL);

	wndcls.lpszClassName = TEXT("COLOR");
	wndcls.lpfnWndProc = ChildProc;
	RegisterClass(&wndcls);

	hChild = CreateWindow(TEXT("COLOR"), NULL, WS_VISIBLE | WS_CHILD,
		330, 0, 470, 400, hwnd, (HMENU)(UINT_PTR)4, hInstance, 0);
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