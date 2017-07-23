#include <Windows.h>
#include <cstdlib>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR appName[] = TEXT("GDI01");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = appName;
	wndclass.lpszMenuName = NULL;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Fail"), TEXT("Errow"), MB_OK);
		return 0;
	}

	hwnd = CreateWindow(appName, TEXT("GDI02"), WS_POPUP,
		100, 100, 600, 400,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HGDIOBJ hPen,HPenOld;
	RECT rect;
	int width, height;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		width = (rect.right - rect.left) / 6;
		height = (rect.bottom - rect.top) / 4;

		//draw border rect
		hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		HPenOld = SelectObject(hdc, hPen);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject(hdc, HPenOld);
		DeleteObject(hPen);

		//draw vert lines
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		HPenOld = SelectObject(hdc, hPen);
		for (int i = rect.left + width; i < rect.right; )
		{
			MoveToEx(hdc, i, rect.top, NULL);
			LineTo(hdc, i, rect.bottom);
			i += width;
		}
		SelectObject(hdc, HPenOld);
		DeleteObject(hPen);

		//draw horiz lines
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		HPenOld = SelectObject(hdc, hPen);
		for (int i = rect.top + height; i < rect.bottom; )
		{
			MoveToEx(hdc, rect.left, i, NULL);
			LineTo(hdc, rect.right, i);
			i += height;
		}
		SelectObject(hdc, HPenOld);
		DeleteObject(hPen);
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
