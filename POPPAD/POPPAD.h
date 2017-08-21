#pragma once

#include "resource.h"
#include <commdlg.h>

#define UNTITLED TEXT("UNTITLED")

static WCHAR szAppName[] = TEXT("POPPAD");
int AskForSave(HWND hwnd, WCHAR* szTitleName);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static OPENFILENAME ofn;
void PopFileInitialize(HWND hwnd);


