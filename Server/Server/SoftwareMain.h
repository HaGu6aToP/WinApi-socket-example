#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <winsock.h>
#include <stdio.h>
#pragma comment (lib, "comctl32.lib")
#pragma comment(lib, "Wsock32.lib")

#define IDM_ChangePortMenuAction 1//
#define IDC_ChangePortButton 2

#define IDM_Start 3
#define IDM_Stop 4

#define WSA_ACCEPT (WM_USER + 1)
#define WSA_NETEVENT (WM_USER + 2)

#define MAX_QUEUE_LENGTH 5

#define NICKNAME_LENGTH 16
#define TEXT_LENGTH 256

struct Declare{
	wchar_t nickname[NICKNAME_LENGTH];
	wchar_t text[TEXT_LENGTH];
};

typedef Declare DECLARE;
typedef Declare* HDECLARE;


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR hCursor, HINSTANCE hInst, HICON hIcon, LPCWSTR Name, WNDPROC Procedure);

HWND CreateChildWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK PortChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void MainwndAddMenus(HWND hWnd);
void MainWndAddWidges(HWND hWnd);
void EditListViewLine(HWND hListView, int iItem, LPWSTR text);

void LoadData();
void SaveData();
void IntToString(int num, wchar_t* str);
void Int64toString(wchar_t* buff, unsigned long long num);

// Запуск сервера
void ServerStart(HWND hWnd);

// Останов сервера
void ServerStop(HWND hWnd);

// Обработка сообщения WSA_ACCEPT
void OnWSAAccept(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Обработка сообщения WSA_NETEVENT
void OnWSANetEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);