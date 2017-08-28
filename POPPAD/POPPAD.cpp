// POPPAD.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "POPPAD.h"

#define EDIT_ID 1

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int icmdShow)
{
	InitializeResource(hInstance);

	MSG        msg;
	HWND       hwnd;
	HACCEL     hacc;
	WNDCLASS   wndcls{};
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR,
		SM_CXCURSOR, SM_CYCURSOR, LR_DEFAULTCOLOR);
	wndcls.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_POPPAD), IMAGE_ICON,
		SM_CXICON, SM_CYICON, LR_DEFAULTCOLOR);
	wndcls.lpfnWndProc = WndProc;
	wndcls.lpszClassName = szAppName;
	wndcls.lpszMenuName = MAKEINTRESOURCE(IDC_POPPAD);

	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, TEXT("Register Failed"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, szCmdLine);

	hacc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POPPAD));

	ShowWindow(hwnd, icmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hacc, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

void InitializeResource(HINSTANCE hInstance)
{
	LoadString(hInstance, IDS_APP_TITLE, szAppName, sizeof(szAppName) / sizeof(TCHAR));
	LoadString(hInstance, IDS_UNTITLE, szUntitled, sizeof(szUntitled) / sizeof(TCHAR));
	LoadString(hInstance, IDS_OKMESSAGEW, szOkMessageW, sizeof(szOkMessageW) / sizeof(TCHAR));
	LoadString(hInstance, IDS_OKMESSAGER, szOkMessageR, sizeof(szOkMessageR) / sizeof(TCHAR));
	LoadString(hInstance, IDS_ASKFORSAVE, szAsForSave, sizeof(szAsForSave) / sizeof(TCHAR));
	LoadString(hInstance, IDS_STRNOTFOUND, szStrNotFound, sizeof(szStrNotFound) / sizeof(TCHAR));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;
	static HINSTANCE hInst;
	static BOOL bNeedSave = FALSE;
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	DWORD dwSel;
	BOOL bEnable;

	switch (msg)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hwndEdit = CreateWindow(TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE |
			ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, 0, 0,
			hwnd, (HMENU)EDIT_ID, hInst, NULL);
		SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
		szTitleName[0] = '\0';
		DoCaption(hwnd, szTitleName);
		uFindReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);
		PopFileInitialize(hwnd);
		PopInitializeFont(hwndEdit);
		return 0;

	case WM_INITMENUPOPUP:
		switch (LOWORD(lParam))
		{
		case 0://file menu
			EnableMenuItem((HMENU)wParam, ID_PRINT, MF_GRAYED);
			break;
		case 1://edit menu
			EnableMenuItem((HMENU)wParam, ID_EDIT_UNDO,
				SendMessage(hwndEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, ID_EDIT_PASTE,
				IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);

			dwSel = SendMessage(hwndEdit, EM_GETSEL, NULL, NULL);
			bEnable = (LOWORD(dwSel) < HIWORD(dwSel)) ? TRUE : FALSE;
			EnableMenuItem((HMENU)wParam, ID_EDIT_COPY, bEnable ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, ID_EDIT_DELETE, bEnable ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, ID_EDIT_CUT, bEnable ? MF_ENABLED : MF_GRAYED);
			break;
		case 2://search menu
			bEnable = (BOOL)GetWindowTextLength(hwndEdit);
			EnableMenuItem((HMENU)wParam, ID_SEARCH_FIND, bEnable ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, ID_SEARCH_FINDNEXT, bEnable ? MF_ENABLED : MF_GRAYED);
			break;
		}
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_COMMAND:
		if (lParam&&LOWORD(wParam) == EDIT_ID)
		{
			switch (HIWORD(wParam))
			{
			case EN_UPDATE:
				bNeedSave = TRUE;
				return 0;
			}
		}

		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
			if (bNeedSave&&IDCANCEL == AskForSave(hwnd, szTitleName))
			{
				return 0;
			}
			SetWindowText(hwndEdit, TEXT("\0"));
			szFileName[0] = '\0';
			szTitleName[0] = '\0';
			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			break;

		case ID_FILE_OPEN:
			if (bNeedSave&&IDCANCEL == AskForSave(hwnd, szTitleName))
			{
				return 0;
			}
			if (PopFileOpenDialog(hwnd, szFileName, szTitleName))
			{
				if (!PopFileRead(hwndEdit, szFileName))
				{
					OkMessage(hwnd, szOkMessageR, szTitleName);
					szFileName[0] = '\0';
					szTitleName[0] = '\0';
				}
			}
			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			break;

		case ID_FILE_SAVE:
			if (!szFileName[0])
			{
				SendMessage(hwnd, WM_COMMAND, MAKELONG(ID_FILE_SAVEAS, 0), 0);
				return 0;
			}
			if (!PopFileWrite(hwndEdit, szFileName))
			{
				OkMessage(hwnd, szOkMessageW, szTitleName);
				szFileName[0] = '\0';
				szTitleName[0] = '\0';
			}
			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			break;

		case ID_FILE_SAVEAS:
			if (PopFileSaveDialog(hwnd, szFileName, szTitleName))
			{
				if (!PopFileWrite(hwndEdit, szFileName))
				{
					OkMessage(hwnd, szOkMessageW, szTitleName);
					szFileName[0] = '\0';
					szTitleName[0] = '\0';
				}
			}
			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			break;

		case ID_PRINT:
			PopPrintDlg(hwnd);
			break;

		case IDM_EXIT:
			if (bNeedSave&&AskForSave(hwnd, szTitleName))
			{
				return 0;
			}
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;

		case ID_EDIT_UNDO:
			SendMessage(hwndEdit, EM_UNDO, 0, 0);
			break;

		case ID_EDIT_CUT:
			SendMessage(hwndEdit, WM_CUT, 0, 0);
			break;

		case ID_EDIT_COPY:
			SendMessage(hwndEdit, WM_COPY, 0, 0);
			break;

		case ID_EDIT_PASTE:
			SendMessage(hwndEdit, WM_PASTE, 0, 0);
			break;

		case ID_EDIT_DELETE:
			SendMessage(hwndEdit, WM_CLEAR, 0, 0);
			break;

		case ID_EDIT_SELECTALL:
			SendMessage(hwndEdit, EM_SETSEL, 0, -1);
			break;

		case ID_FOMAT_FONT:
			if (PopFontChoose(hwnd))
			{
				DeleteObject(hFont);
				hFont = CreateFontIndirect(&logfont);
				SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0);
				RECT rect;
				GetClientRect(hwndEdit, &rect);
				InvalidateRect(hwndEdit, &rect, TRUE);
			}
			break;

		case ID_SEARCH_FIND:
			PopFindDialog(hwnd);
			break;

		case ID_SEARCH_FINDNEXT:
			if (*szFindWhat != '\0')
			{
				if (!SearchNextText(hwndEdit, findre.Flags& FR_DOWN, findre.Flags&FR_MATCHCASE))
				{
					SearchNotFound();
				}
				break;
			}
			PopFindDialog(hwnd);
			break;

		case ID_SEARCH_REPLACE:
			PopReplaceDialog(hwnd);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, AboutWndProc);
			break;
		}
		return 0;

	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;
	}

	if (msg == uFindReplaceMsg)
	{
		if (findre.Flags & FR_FINDNEXT)
		{
			if (!SearchNextText(hwndEdit, findre.Flags& FR_DOWN, findre.Flags& FR_MATCHCASE))
			{
				SearchNotFound();
			}
		}
		else if (findre.Flags&FR_REPLACE)
		{
			if (!ReplaceNextText(hwndEdit, findre.Flags&FR_MATCHCASE))
			{
				SearchNotFound();
			}
		}
		else if (findre.Flags&FR_REPLACEALL)
		{
			SendMessage(hwndEdit, EM_SETSEL, (WPARAM)0, 0);
			while (ReplaceNextText(hwndEdit, findre.Flags&FR_MATCHCASE));
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hwnd, TRUE);
		}
		break;
	}
	return FALSE;

}

void DoCaption(HWND hwnd, TCHAR * szTitleName)
{
	TCHAR szCaption[64 + MAX_PATH];
	wsprintf(szCaption, TEXT("%s %s"), szTitleName[0] ? szTitleName : szUntitled, szAppName);
	SetWindowText(hwnd, szCaption);
}

void OkMessage(HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName)
{
	TCHAR szBuffer[64 + MAX_PATH];
	wsprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName : szUntitled);
	MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}

int AskForSave(HWND hwnd, WCHAR* szTitleName)
{

	TCHAR szBuffer[64 + MAX_PATH];
	int iReturn;
	wsprintf(szBuffer, szAsForSave, szTitleName[0] ? szTitleName : szUntitled);
	iReturn = MessageBox(hwnd, szBuffer, szAppName, MB_YESNOCANCEL | MB_ICONQUESTION);
	if (iReturn == IDYES)
	{
		if (!SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0))
			return IDCANCEL;
	}
	return iReturn;
}

void PopFileInitialize(HWND hwnd)
{
	TCHAR *szFileFilter = TEXT("Text Files(*TXT)\0*.txt\0All Files(*.*)\0*.*\0\0");
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFileFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("txt");
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL PopFileOpenDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_EXPLORER;
	return GetOpenFileName(&ofn);
}

BOOL PopFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
	DWORD iFileSize;
	HANDLE hFile;
	PBYTE pBuffer, pConv, pText;
	BYTE bySwap;
	Encode encode;
	int nLenWide;

	if (INVALID_HANDLE_VALUE ==
		(hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
	{
		return FALSE;
	}
	iFileSize = GetFileSize(hFile, NULL);
	pBuffer = (PBYTE)malloc(iFileSize + 2);
	ReadFile(hFile, pBuffer, iFileSize, NULL, NULL);
	CloseHandle(hFile);
	pBuffer[iFileSize] = '\0';
	pBuffer[iFileSize + 1] = '\0';

	encode = DetectEncode(pBuffer, iFileSize);
	switch (encode)
	{
	case Encode::UNICODE_LE:
		pText = pBuffer + 2;
		iFileSize -= 2;

		pConv = (PBYTE)malloc(iFileSize + 2);
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
		break;

	case Encode::UNICODE_BE:
		pText = pBuffer + 2;
		iFileSize -= 2;
		for (int i = 0; i < (int)iFileSize / 2; ++i)
		{
			bySwap = pText[2 * i];
			pText[2 * i] = pText[2 * i + 1];
			pText[2 * i + 1] = bySwap;
		}
		pConv = (PBYTE)malloc(iFileSize + 2);
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
		break;

	case Encode::UTF8:
		pText = pBuffer + 3;
		nLenWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pText, -1, NULL, 0);
		pConv = (PBYTE)calloc(nLenWide, sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pText, -1, (PWSTR)pConv, nLenWide);
		break;

	case Encode::UTF8_NOBOM:
		pText = pBuffer;
		nLenWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pText, -1, NULL, 0);
		pConv = (PBYTE)calloc(nLenWide, sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pText, -1, (PWSTR)pConv, nLenWide);
		break;

	case Encode::ANSI:
		pText = pBuffer;
		pConv = (PBYTE)malloc(iFileSize * 2 + 2);
		MultiByteToWideChar(CP_ACP, 0, (LPCCH)pText, -1, (PWSTR)pConv, iFileSize + 1);
		break;

	}

	SetWindowText(hwndEdit, (PWSTR)pConv);
	free(pBuffer);
	free(pConv);
	return TRUE;
}

BOOL PopFileSaveDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{

	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	return GetSaveFileName(&ofn);
}

BOOL PopFileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
	HANDLE hFile;
	PBYTE pBuffer;
	int iLength;
	WORD wByteOrderMark = 0xFEFF;
	DWORD dwBytesWritten;
	if (INVALID_HANDLE_VALUE == (
		hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
	{
		return FALSE;
	}
	iLength = GetWindowTextLength(hwndEdit);
	pBuffer = (PBYTE)calloc(iLength + 1, sizeof(TCHAR));
	GetWindowText(hwndEdit, (PWSTR)pBuffer, iLength + 1);

	if (!pBuffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	// write BOM
	WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);
	// write data
	WriteFile(hFile, pBuffer, iLength * sizeof(TCHAR), &dwBytesWritten, NULL);

	// verify output
	if (iLength * sizeof(TCHAR) != (int)dwBytesWritten)
	{
		CloseHandle(hFile);
		free(pBuffer);
		return FALSE;
	}

	CloseHandle(hFile);
	free(pBuffer);
	return TRUE;
}

Encode DetectEncode(const PBYTE pBuffer, long length)
{
	if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFE)
	{
		return Encode::UNICODE_LE;
	}
	else if (pBuffer[0] == 0xFE && pBuffer[1] == 0xFF)
	{
		return Encode::UNICODE_BE;
	}
	else if (pBuffer[0] == 0xEF && pBuffer[1] == 0xBB && pBuffer[2] == 0xBF)
	{
		return Encode::UTF8;
	}
	else if (CheckUnicodeWithoutBOM(pBuffer, length))
	{
		return Encode::UTF8_NOBOM;
	}
	else
	{
		return Encode::ANSI;
	}
}

void PopInitializeFont(HWND hwnd)
{
	hFont = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(hFont, sizeof(LOGFONT), &logfont);
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, 0);
}

BOOL PopFontChoose(HWND hwnd)
{
	choosef.lStructSize = sizeof(CHOOSEFONT);
	choosef.hwndOwner = hwnd;
	choosef.hDC = NULL;
	choosef.lpLogFont = &logfont;
	choosef.iPointSize = NULL;
	choosef.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
	choosef.rgbColors = NULL;
	choosef.lCustData = NULL;
	choosef.lpfnHook = NULL;
	choosef.lpTemplateName = NULL;
	choosef.hInstance = NULL;
	choosef.lpszStyle = NULL;
	choosef.nFontType = NULL;
	choosef.nSizeMin = 8;
	choosef.nSizeMax = 72;
	return ChooseFont(&choosef);
}

HWND PopFindDialog(HWND hwnd)
{
	findre.lStructSize = sizeof(FINDREPLACE);
	findre.hwndOwner = hwnd;
	findre.hInstance = NULL;
	findre.Flags = findre.Flags | FR_FINDNEXT | FR_HIDEWHOLEWORD;
	findre.lpstrFindWhat = szFindWhat;
	findre.lpstrReplaceWith = NULL;
	findre.wFindWhatLen = MAXFIND;
	findre.wReplaceWithLen = NULL;
	findre.lCustData = NULL;
	findre.lpfnHook = NULL;
	findre.lpTemplateName = NULL;
	return FindText(&findre);
}

BOOL SearchNextText(HWND hwndEdit, BOOL bFlag, BOOL bMatch)
{
	int iLength = GetWindowTextLength(hwndEdit);
	TCHAR* pBuffer = (TCHAR*)malloc((iLength + 1) * sizeof(TCHAR));
	GetWindowText(hwndEdit, pBuffer, iLength + 1);
	TCHAR szFind[MAXFIND];
	lstrcpy(szFind, szFindWhat);
	if (!bMatch)
	{
		_wcslwr_s(szFind, wcslen(szFind) + 1);
		_wcslwr_s(pBuffer, iLength + 1);
	}
	if (bFlag)
	{
		int offset = HIWORD(SendMessage(hwndEdit, EM_GETSEL, 0, 0));
		TCHAR *pFind = _tcsstr(pBuffer + offset, szFind);
		if (!pFind)
		{
			return FALSE;
		}
		offset = pFind - pBuffer;
		SendMessage(hwndEdit, EM_SETSEL, offset, offset + _tcslen(szFind));
	}
	else
	{
		int offset = LOWORD(SendMessage(hwndEdit, EM_GETSEL, 0, 0));
		TCHAR *pText = (TCHAR*)malloc((offset + 1) * sizeof(TCHAR));
		wcsncpy_s(pText, offset + 1, pBuffer, offset);
		pText[offset] = '\0';

		TCHAR *pFind = ReverseSearch(pText, szFind);
		if (!pFind)
		{
			return FALSE;
		}
		offset = pFind - pText;
		SendMessage(hwndEdit, EM_SETSEL, offset, offset + _tcslen(szFind));
		free(pText);
	}
	free(pBuffer);
	return TRUE;
}

HWND PopReplaceDialog(HWND hwnd)
{
	findre.lStructSize = sizeof(FINDREPLACE);
	findre.hwndOwner = hwnd;
	findre.hInstance = NULL;
	findre.Flags = FR_FINDNEXT | FR_MATCHCASE | FR_HIDEWHOLEWORD;
	findre.lpstrFindWhat = szFindWhat;
	findre.lpstrReplaceWith = szReplaceWhat;
	findre.wFindWhatLen = MAXFIND;
	findre.wReplaceWithLen = MAXFIND;
	findre.lCustData = NULL;
	findre.lpfnHook = NULL;
	findre.lpTemplateName = NULL;
	return ReplaceText(&findre);
}

TCHAR* ReverseSearch(TCHAR * pStr, const TCHAR * pSub_str)
{
	TCHAR* pLast = pStr + wcslen(pStr);
	for (; pLast >= pStr; pLast--)
	{
		if (*pLast == *pSub_str)
		{
			if (wcsncmp(pLast, pSub_str, wcslen(pSub_str)) == 0)
			{
				return pLast;
			}
		}
	}
	return nullptr;
}

BOOL ReplaceNextText(HWND hwndEdit, BOOL bMatch)
{
	int iLength = GetWindowTextLength(hwndEdit);
	TCHAR* pBuffer = (TCHAR*)malloc((iLength + 1) * sizeof(TCHAR));
	GetWindowText(hwndEdit, pBuffer, iLength + 1);

	DWORD dwPosition = SendMessage(hwndEdit, EM_GETSEL, 0, 0);
	int iSelected = HIWORD(dwPosition) - LOWORD(dwPosition);
	TCHAR* pSelected = (TCHAR*)malloc((iSelected + 1) * sizeof(TCHAR));
	wcsncpy_s(pSelected, iSelected + 1, pBuffer + LOWORD(dwPosition), iSelected);
	pSelected[iSelected] = '\0';

	free(pBuffer);
	free(pSelected);

	if (wcscmp(pSelected, szFindWhat) != 0)
	{
		return SearchNextText(hwndEdit, TRUE, bMatch);
	}
	SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)szReplaceWhat);
	return SearchNextText(hwndEdit, TRUE, bMatch);
}

void SearchNotFound()
{
	TCHAR szBuffer[64 + MAX_PATH];
	wsprintf(szBuffer, szStrNotFound, findre.lpstrFindWhat);
	MessageBox(NULL, szBuffer, szAppName, MB_ICONINFORMATION | MB_OK);
}

void PopPrintDlg(HWND hwnd)
{
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = hwnd;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;

	if (PrintDlg(&pd) == TRUE)
	{
		//TODO PAINT
	}

}

int CheckUnicodeWithoutBOM(const PBYTE pText, long length)
{
	int i;
	DWORD nBytes = 0;
	UCHAR chr;

	BOOL bAllAscii = TRUE;
	for (i = 0; i < length; i++)
	{
		chr = *(pText + i);
		if ((chr & 0x80) != 0)
			bAllAscii = FALSE;
		if (nBytes == 0)
		{
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else
				{
					return FALSE;
				}
				nBytes--;
			}
		}
		else
		{
			if ((chr & 0xC0) != 0x80)
			{
				return FALSE;
			}
			nBytes--;
		}
	}
	if (nBytes > 0)
	{
		return FALSE;
	}
	if (bAllAscii)
	{
		return FALSE;
	}
	return TRUE;
}
