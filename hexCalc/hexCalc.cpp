#include "stdafx.h"
#include "hexCalc.h"

WCHAR szAppCls[] = TEXT("HEXCALC");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdline, int icmdShow)
{
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass{};
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppCls;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;//NOTE!!!
	wndclass.hInstance = hInstance;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Failed"), TEXT("Warning"), MB_OK);
		return 0;
	}

	hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, NULL);

	ShowWindow(hwnd, icmdShow);
	UpdateWindow(hwnd);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		// Determines whether a message is intended for the specified dialog box 
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;

}
void ShowNumber(HWND hwnd, UINT iNumber)
{
	TCHAR szBuffer[20];

	wsprintf(szBuffer, TEXT("%X"), iNumber);
	SetDlgItemText(hwnd, VK_ESCAPE, szBuffer);
}

DWORD CalcIt(UINT iFirstNum, int iOperation, UINT iNum)
{
	switch (iOperation)
	{
	case '=': return iNum;
	case '+': return iFirstNum + iNum;
	case '-': return iFirstNum - iNum;
	case '*': return iFirstNum *  iNum;
	case '&': return iFirstNum &  iNum;
	case '|': return iFirstNum | iNum;
	case '^': return iFirstNum ^  iNum;
	case '<': return iFirstNum << iNum;
	case '>': return iFirstNum >> iNum;
	case '/': return iNum ? iFirstNum / iNum : MAXDWORD;
	case '%': return iNum ? iFirstNum % iNum : MAXDWORD;
	default: return 0;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL  bNewNumber = TRUE;
	static int   iOperation = '=';
	static UINT  iNumber, iFirstNum;
	HWND         hButton;

	switch (message)
	{
	case WM_KEYDOWN:                   // left arrow --> backspace
		if (wParam != VK_LEFT)
			break;
		wParam = VK_BACK;
		// fall through
	case WM_CHAR:
		if ((wParam = (WPARAM)CharUpper((TCHAR *)wParam)) == VK_RETURN)
			wParam = '=';

		if (hButton = GetDlgItem(hwnd, wParam))
		{
			SendMessage(hButton, BM_SETSTATE, 1, 0);
			Sleep(100);
			SendMessage(hButton, BM_SETSTATE, 0, 0);
		}
		else
		{
			MessageBeep(0);
			break;
		}
		// fall through
	case WM_COMMAND:
		SetFocus(hwnd);

		if (LOWORD(wParam) == VK_BACK)         // backspace
			ShowNumber(hwnd, iNumber /= 16);

		else if (LOWORD(wParam) == VK_ESCAPE)  // escape
			ShowNumber(hwnd, iNumber = 0);

		else if (isxdigit(LOWORD(wParam)))    // hex digit
		{
			if (bNewNumber)
			{
				iFirstNum = iNumber;
				iNumber = 0;
			}
			bNewNumber = FALSE;

			if (iNumber <= MAXDWORD >> 4)
				ShowNumber(hwnd, iNumber = 16 * iNumber + wParam -
				(isdigit(wParam) ? '0' : 'A' - 10));
			else
				MessageBeep(0);
		}
		else                                    // operation
		{
			if (!bNewNumber)
				ShowNumber(hwnd, iNumber =
					CalcIt(iFirstNum, iOperation, iNumber));
			bNewNumber = TRUE;
			iOperation = LOWORD(wParam);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
