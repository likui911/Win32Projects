#include <Windows.h>

const int BORDER = 30;
BOOL CONTINUE = TRUE;

static int chess[15][15] = { 0 };//0 无旗子，1 黑子，2 白子

BOOL isFive(int xChess, int yChess)
{
	//横向检测
	int x_min = xChess - 1;
	int x_max = xChess;
	int y_min = yChess - 1;
	int y_max = yChess;
	int total = 0;
	for (x_min; x_min >= 0; x_min--)
	{
		if (chess[x_min][yChess] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	for (x_max; x_max <= 14; x_max++)
	{
		if (chess[x_max][yChess] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	if (total == 5)
	{
		return TRUE;
	}
	//竖向检测
	x_min = xChess - 1;
	x_max = xChess;
	y_min = yChess - 1;
	y_max = yChess;
	total = 0;
	for (y_min; y_min >= 0; y_min--)
	{
		if (chess[xChess][y_min] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	for (y_max; y_max <= 14; y_max++)
	{
		if (chess[xChess][y_max] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	if (total == 5)
	{
		return TRUE;
	}
	//逆时针
	x_min = xChess - 1;
	x_max = xChess;
	y_min = yChess - 1;
	y_max = yChess;
	total = 0;
	for (x_min, y_min; y_min >= 0 && x_min >= 0; y_min--, x_min--)
	{
		if (chess[x_min][y_min] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	for (x_max, y_max; y_max <= 14 && x_max <= 14; x_max++, y_max++)
	{
		if (chess[x_max][y_max] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	if (total == 5)
	{
		return TRUE;
	}
	//顺时针
	x_min = xChess;
	x_max = xChess + 1;
	y_min = yChess - 1;
	y_max = yChess;
	total = 0;
	for (x_min, y_max; y_max <= 14 && x_min >= 0; y_max++, x_min--)
	{
		if (chess[x_min][y_max] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	for (x_max, y_min; y_min >= 0 && x_max <= 14; x_max++, y_min--)
	{
		if (chess[x_max][y_min] != chess[xChess][yChess])
		{
			break;
		}
		total += 1;
	}
	if (total == 5)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL hasChess(int xChess, int yChess)
{
	return chess[xChess][yChess] != 0;
}

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
	hwnd = CreateWindow(szAppName, TEXT("Five Chess"), WS_OVERLAPPEDWINDOW,
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
	PMINMAXINFO pMininfo;
	static int cxClient, cyClient;
	static int xCenter, yCenter;
	static int cxRect;
	static int cxLeftChess, cyTopChess;
	static BOOL bIsBlack = TRUE;
	int xPos, yPos;//鼠标点坐标

	switch (msg)
	{
	case WM_GETMINMAXINFO:
	{
		pMininfo = (PMINMAXINFO)lParam;
		pMininfo->ptMinTrackSize.x = 400;
		pMininfo->ptMinTrackSize.y = 400;
		return 0;
	}
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		//求格子大小
		cxRect = ((cxClient > cyClient ? cyClient : cxClient) - 2 * BORDER) / 14;
		xCenter = cxClient / 2;
		yCenter = cyClient / 2;
		cxLeftChess = xCenter - cxRect * 7;
		cyTopChess = yCenter - cxRect * 7;
		return 0;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		//画围棋格子
		for (int i = 0; i < 15; ++i)
		{
			MoveToEx(hdc, xCenter - 7 * cxRect, yCenter - (7 - i) * cxRect, NULL);
			LineTo(hdc, xCenter + 7 * cxRect, yCenter - (7 - i) * cxRect);
		}
		for (int i = 0; i < 15; ++i)
		{
			MoveToEx(hdc, cxClient / 2 - (7 - i) * cxRect, cyClient / 2 - 7 * cxRect, NULL);
			LineTo(hdc, cxClient / 2 - (7 - i) * cxRect, cyClient / 2 + 7 * cxRect);
		}
		//画关键点
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		Ellipse(hdc, cxClient / 2 - 3, cyClient / 2 - 3, cxClient / 2 + 3, cyClient / 2 + 3);
		Ellipse(hdc, cxClient / 2 - 4 * cxRect - 3, cyClient / 2 - 4 * cxRect - 3, cxClient / 2 - 4 * cxRect + 3, cyClient / 2 - 4 * cxRect + 3);
		Ellipse(hdc, cxClient / 2 + 4 * cxRect - 3, cyClient / 2 - 4 * cxRect - 3, cxClient / 2 + 4 * cxRect + 3, cyClient / 2 - 4 * cxRect + 3);
		Ellipse(hdc, cxClient / 2 - 4 * cxRect - 3, cyClient / 2 + 4 * cxRect - 3, cxClient / 2 - 4 * cxRect + 3, cyClient / 2 + 4 * cxRect + 3);
		Ellipse(hdc, cxClient / 2 + 4 * cxRect - 3, cyClient / 2 + 4 * cxRect - 3, cxClient / 2 + 4 * cxRect + 3, cyClient / 2 + 4 * cxRect + 3);

		//画棋子
		for (int i = 0; i < 15; ++i)
		{
			for (int j = 0; j < 15; ++j)
			{
				if (chess[i][j] == 1)
				{
					SelectObject(hdc, GetStockObject(BLACK_BRUSH));
					Ellipse(hdc, cxLeftChess + cxRect*i - cxRect / 3, cyTopChess + cxRect*j - cxRect / 3, cxLeftChess + cxRect*i + cxRect / 3, cyTopChess + cxRect*j + cxRect / 3);
				}
				else if (chess[i][j] == 2)
				{
					SelectObject(hdc, GetStockObject(WHITE_BRUSH));
					Ellipse(hdc, cxLeftChess + cxRect*i - cxRect / 3, cyTopChess + cxRect*j - cxRect / 3, cxLeftChess + cxRect*i + cxRect / 3, cyTopChess + cxRect*j + cxRect / 3);
				}
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		if (!CONTINUE)
		{
			return 0;
		}
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		//x,y 相对于中心偏移量
		int x = xPos > xCenter ? (xPos + cxRect / 2 - xCenter) / cxRect : (xPos - cxRect / 2 - xCenter) / cxRect;
		int y = yPos > yCenter ? (yPos + cxRect / 2 - yCenter) / cxRect : (yPos - cxRect / 2 - yCenter) / cxRect;
		//超出边界处理
		if (abs(x) > 7 || abs(y) > 7)
		{
			return 0;
		}
		//棋盘上的位置
		int xChess = x + 7;
		int yChess = y + 7;
		//检测是否存在棋子
		if (hasChess(xChess, yChess))
		{
			return 0;
		}
		if (bIsBlack)
		{
			chess[xChess][yChess] = 1;
			bIsBlack = FALSE;
		}
		else
		{
			chess[xChess][yChess] = 2;
			bIsBlack = TRUE;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		//检测是否获胜
		if (isFive(xChess, yChess))
		{
			TCHAR szWarn[20] = TEXT("");
			if (!bIsBlack)
			{
				wcscat_s(szWarn, TEXT("黑棋"));
			}
			else
			{
				wcscat_s(szWarn, TEXT("白棋"));
			}
			wcscat_s(szWarn, TEXT("获胜，是否继续？"));
			int b = MessageBox(hwnd, szWarn, TEXT("Five Chess"), MB_YESNO);
			if (b == IDYES)
			{
				for (int i = 0; i < 15; ++i)
				{
					for (int j = 0; j < 15; ++j)
					{
						chess[i][j] = 0;
					}
				}
				bIsBlack = TRUE;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			else
			{
				CONTINUE = FALSE;
			}
		}
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
