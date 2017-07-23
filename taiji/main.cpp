#include <Windows.h>

const int R = 200;//矩形的边长

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = TEXT("Rectangle");
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
	hwnd = CreateWindow(szAppName, TEXT("TaiJi"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 550, 500,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;
	int iCenterX, iCenterY;
	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		iCenterX = (rect.right - rect.left) / 2;
		iCenterY = (rect.bottom - rect.top) / 2;
	
		Ellipse(hdc, iCenterX - R - 1, iCenterY - R - 1, iCenterX + R + 1, iCenterY + R + 1);
		
		//左半圆（黑色）
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		Pie(hdc, iCenterX - R - 1, iCenterY - R - 1, iCenterX + R + 1, iCenterY + R + 1, iCenterX, iCenterY - R, iCenterX, iCenterY + R);
		//右半圆（白色）
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		Pie(hdc, iCenterX - R, iCenterY - R, iCenterX + R, iCenterY + R, iCenterX, iCenterY + R, iCenterX, iCenterY - R);
		//下小半圆（白色）
		Pie(hdc, iCenterX - R / 2, iCenterY, iCenterX + R / 2, iCenterY + R, iCenterX, iCenterY, iCenterX, iCenterY + R);
		//上小半圆（黑色）
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		Pie(hdc, iCenterX - R / 2, iCenterY - R, iCenterX + R / 2, iCenterY, iCenterX, iCenterY, iCenterX, iCenterY - R);
		//下小圆（黑色）
		Ellipse(hdc, iCenterX - R / 6, iCenterY + R / 3, iCenterX + R / 6, iCenterY + 2 * R / 3);
		//上小圆（白色）
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		Ellipse(hdc, iCenterX - R / 6, iCenterY - 2 * R / 3, iCenterX + R / 6, iCenterY - R / 3);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}