#pragma once

#include "stdafx.h"
#include "resource.h"
#include <algorithm>

#define MAXFIND 256

static WCHAR szAppName[MAX_PATH];
static WCHAR szUntitled[MAX_PATH];
static WCHAR szOkMessageR[256];
static WCHAR szOkMessageW[256];
static WCHAR szAsForSave[256];
static WCHAR szStrNotFound[MAXFIND];

void InitializeResource(HINSTANCE hInstance);
//dialog procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//utility dialog
void DoCaption(HWND hwnd, TCHAR * szTitleName);
void OkMessage(HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName);
int AskForSave(HWND hwnd, WCHAR* szTitleName);

//open file dialog
static OPENFILENAME ofn;
void PopFileInitialize(HWND hwnd);
BOOL PopFileOpenDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL PopFileRead(HWND hwndEdit, PTSTR pstrFileName);
BOOL PopFileSaveDialog(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL PopFileWrite(HWND hwndEdit, PTSTR pstrFileName);

//ANSI Unicode
enum class Encode { ANSI = 1, UNICODE_LE, UNICODE_BE, UTF8, UTF8_NOBOM };
BOOL CheckUnicodeWithoutBOM(const PBYTE pText, long length);
Encode DetectEncode(const PBYTE, long length);

//font dialog
static LOGFONT logfont;
static HFONT hFont;
static CHOOSEFONT choosef;
void PopInitializeFont(HWND hwnd);
BOOL PopFontChoose(HWND hwnd);

//search dialog
static FINDREPLACE findre;
static TCHAR szFindWhat[MAXFIND];
static TCHAR szReplaceWhat[MAXFIND];
UINT uFindReplaceMsg;
HWND PopFindDialog(HWND hwnd);
BOOL SearchNextText(HWND hwndEdit, BOOL bFlag,BOOL bMatch);
HWND PopReplaceDialog(HWND hwnd);
TCHAR* ReverseSearch(TCHAR* str, const TCHAR* sub_str);
BOOL ReplaceNextText(HWND hwndEdit,BOOL bMatch);
void SearchNotFound();

