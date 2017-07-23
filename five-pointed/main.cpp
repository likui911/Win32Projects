#include <Windows.h>
#include <math.h>

const double PI = 3.14;
const int R = 150;

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
	hwnd = CreateWindow(szAppName, TEXT("Sine Waves"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
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
	int x_center, y_center;
	POINT apt[5];
	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		x_center = (rect.right - rect.left) / 2;
		y_center = (rect.bottom - rect.top) / 2;
		//左上点
		apt[0].x = int(x_center - R*sin(2 * PI / 5));
		apt[0].y = int(y_center - R*cos(2 * PI / 5));
		//顶点
		apt[1].x = int(x_center);
		apt[1].y = int(y_center - R);
		//右上点
		apt[2].x = int(x_center + R*sin(2 * PI / 5));
		apt[2].y = int(y_center - R*cos(2 * PI / 5));
		//右下点
		apt[3].x = int(x_center + R*sin(PI / 5));
		apt[3].y = int(y_center + R*cos(PI / 5));
		//左下角
		apt[4].x = int(x_center - R*sin(PI / 5));
		apt[4].y = int(y_center + R*cos(PI / 5));

		MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
		LineTo(hdc, apt[2].x, apt[2].y);
		LineTo(hdc, apt[4].x, apt[4].y);
		LineTo(hdc, apt[1].x, apt[1].y);
		LineTo(hdc, apt[3].x, apt[3].y);
		LineTo(hdc, apt[0].x, apt[0].y);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}