#include <Windows.h>
#include <Strsafe.h>
#include "sysmets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR appName[] = TEXT("SysMets2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = appName;
	wndclass.lpszMenuName = NULL;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT(""), TEXT(""), MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(appName, TEXT("Get System Metrics"),
		WS_VSCROLL | WS_HSCROLL | WS_OVERLAPPEDWINDOW,
		100, 70, 600, 480, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))//这里GetMessage hwnd设置为NULL
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	HDC hdc;
	int i, x, y, iVertPos, iHrozPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	SCROLLINFO si;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		cxChar = tm.tmAveCharWidth;
		cxCaps = ((tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar) / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		iMaxWidth = 40 * cxChar + 22 * cxCaps;
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		//set vert scrollbar
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nMax = NUMLINES - 1;
		si.nMin = 0;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		//set hroz scrollbar
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nMax = 2 + iMaxWidth / cxChar;
		si.nMin = 0;
		si.nPage = cxClient / cxChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		return 0;
	case WM_VSCROLL:
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);
		if (si.nPos != iVertPos)
		{
			ScrollWindow(hwnd, 0, cyChar*(iVertPos - si.nPos), NULL, NULL);
			UpdateWindow(hwnd);
		}
		return 0;
	case WM_HSCROLL:
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHrozPos = si.nPos;
		switch(LOWORD(wParam))
		{
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd,SB_HORZ,&si);
		if (iHrozPos != si.nPos)
		{
			ScrollWindow(hwnd, cxChar*(iHrozPos - si.nPos), 0, NULL, NULL);
			UpdateWindow(hwnd);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		
		GetScrollInfo(hwnd, SB_VERT, &si);
		iPaintBeg = max(0, si.nPos + ps.rcPaint.top / cyChar);
		iPaintEnd = min(NUMLINES - 1, si.nPos + ps.rcPaint.bottom / cyChar);
		iVertPos = si.nPos;
		
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHrozPos = si.nPos;
		for (i = iPaintBeg; i <= iPaintEnd; i++)
		{
			x =  cxChar*(1 - iHrozPos);
			y = cyChar*(i - iVertPos);
			TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, x+22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);
			//formatted systemmetricts
			StringCbPrintf(szBuffer, sizeof(szBuffer), TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex));
			TextOut(hdc, x+22 * cxCaps + 40 * cxChar, y, szBuffer, lstrlen(szBuffer));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}