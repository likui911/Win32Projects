#include <Windows.h>

#define ID_BUTTON_1 1
#define ID_BUTTON_2 1

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HWND hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("BUTTON_1"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			100, 100, 100, 50, hwnd, (HMENU)(UINT_PTR)ID_BUTTON_1,
			(HINSTANCE)(UINT_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);

		HWND hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("BUTTON_2"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			250, 100, 100, 50, hwnd, (HMENU)(UINT_PTR)ID_BUTTON_2,
			(HINSTANCE)(UINT_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
		return 0;;
	}

	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *pdis = (LPDRAWITEMSTRUCT)lParam;
		HBRUSH hbr;
		//draw rect border
		if (pdis->itemState & ODS_SELECTED)
		{
			hbr = CreateSolidBrush(RGB(128, 128, 128));
			FillRect(pdis->hDC, &pdis->rcItem, hbr);
		}
		else
		{
			hbr = CreateSolidBrush(RGB(180, 180, 180));
			FillRect(pdis->hDC, &pdis->rcItem, hbr);
		}

		//draw text
		SetBkMode(pdis->hDC, TRANSPARENT);
		TextOut(pdis->hDC, 15, 15, TEXT("OwnerDraw"), 9);

		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{
	WCHAR szClsName[] = TEXT("OwnDraw");
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
	HWND hwnd = CreateWindow(szClsName, TEXT("OwnDraw Window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
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