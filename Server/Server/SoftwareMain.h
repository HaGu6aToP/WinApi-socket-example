#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment (lib, "comctl32.lib")

#define ChangePortAction (WM_USER + 1)


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR hCursor, HINSTANCE hInst, HICON hIcon, LPCWSTR Name, WNDPROC Procedure);

void MainwndAddMenus(HWND hWnd);
void MainWndAddWidges(HWND hWnd);
void EditListViewLine(HWND hListView, int iItem, LPWSTR text);