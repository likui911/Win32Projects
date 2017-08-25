#pragma once

#include "stdafx.h"
#include "resource.h"

#define UNTITLED TEXT("UNTITLED")

static WCHAR szAppName[] = TEXT("POPPAD");

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
BOOL PopFileWrite(HWND hwndEdit,PTSTR pstrFileName);

//ANSI Unicode
enum class Encode{ANSI=1,UNICODE_LE,UNICODE_BE,UTF8,UTF8_NOBOM};
BOOL CheckUnicodeWithoutBOM(const PBYTE pText,long length);
Encode DetectEncode(const PBYTE, long length);

//font dialog
static LOGFONT logfont;
static HFONT hFont;
static CHOOSEFONT choosef;
//todo 字体设置
//初始化时hwndedit设置为系统字体
void PopInitializeFont(HWND hwnd);
BOOL PopFontChoose(HWND hwnd);
//打开字体选择框
//设置字体


