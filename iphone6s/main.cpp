#include <Windows.h>

const int R = 200;//¾ØÐÎµÄ±ß³¤

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
	hwnd = CreateWindow(szAppName, TEXT("iphone 6s"), WS_OVERLAPPEDWINDOW,
		100, 100, 450, 600,
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
	HDC hdc;
	static int cxClient, cyClient;

	switch (msg)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		RoundRect(hdc, 230, 49, 250, 70, 8, 8);
		TextOut(hdc, 160, 50, TEXT("iPhone 6"), 8);
		TextOut(hdc, 236, 50, TEXT("s"), 1);

		
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		// µçÔ´¼ü
		RoundRect(hdc, 299, 182, 304, 220, 8, 8);
		// ¾²Òô¼ü
		RoundRect(hdc, 98, 151, 103, 175, 8, 8);
		// ÒôÁ¿¼ü
		RoundRect(hdc, 98, 192, 103, 229, 8, 8);
		RoundRect(hdc, 98, 242, 103, 280, 8, 8);
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		// ÂÖÀª
		RoundRect(hdc, 100, 100, 302, 510, 40, 40);
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		// ¾àÀë¸ÐÓ¦Æ÷
		Ellipse(hdc, 198, 111, 205, 118);
		// ÌýÍ²
		RoundRect(hdc, 182, 126, 223, 130,10,3);
		// Ç°ÖÃÉãÏñÍ·
		Ellipse(hdc, 161, 124, 171, 134);
		// ÆÁÄ»
		Rectangle(hdc, 111, 146, 292, 458);
		// Home¼ü
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 0, 0)));
		Ellipse(hdc, 182, 463, 222, 502);
		
	
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}