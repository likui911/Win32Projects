#include <Windows.h>
#include <amp_math.h>

const int NUM = 100;

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = TEXT("SineWave");
	WNDCLASS wndclass;
	HWND hwnd;
	MSG msg;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, SM_CXCURSOR, SM_CYCURSOR, LR_DEFAULTSIZE);
	wndclass.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, SM_CXICON, SM_CYICON, LR_DEFAULTSIZE);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WinProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Failed"), szAppName, MB_OK);
	}
	hwnd = CreateWindow(szAppName, TEXT("Sine Waves"), WS_VSCROLL | WS_HSCROLL | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;
	POINT apt[NUM];
	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		MoveToEx(hdc, rect.left, (rect.bottom - rect.top) / 2, NULL);
		LineTo(hdc, rect.right, (rect.bottom - rect.top) / 2);
		for (int i = 0; i < NUM; ++i)
		{
			apt[i].x = i*(rect.right - rect.left) / NUM;
			apt[i].y = (int)((rect.bottom - rect.top) / 2 * (1 - sin(6.28*i / NUM)));
		}
		Polyline(hdc, apt, NUM);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}