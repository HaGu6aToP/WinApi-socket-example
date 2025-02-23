#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#pragma comment (lib, "comctl32.lib")

#define IDM_ChangePortMenuAction 1//(WM_USER + 1)
#define IDC_ChangePortButton 2

struct Declare{
	char hickname[10];
	char text[256];
};

typedef Declare DECLARE;
typedef Declare* HDECLARE;


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR hCursor, HINSTANCE hInst, HICON hIcon, LPCWSTR Name, WNDPROC Procedure);

HWND CreateChildWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK ConnectChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void MainwndAddMenus(HWND hWnd);
void MainWndAddWidges(HWND hWnd);
void EditListViewLine(HWND hListView, int iItem, LPWSTR text);

void LoadData();
void SaveData();
void IntToString(int num, wchar_t* str);