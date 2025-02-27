#pragma once

#include <Windows.h>
#include <winsock.h>
#include <CommCtrl.h>
#include <stdio.h>


#pragma comment (lib, "comctl32.lib")
#pragma comment(lib, "Wsock32.lib")

#define MAX_NICKNAME_LENGTH 16 // with '\0'
#define MAX_PASSWORD_LENGTH 16 
#define MAX_ADDRESS_LENGTH 16

#define IDM_ChangePortMenuAction 1
#define IDM_ChangeAddressMenuAction 3
#define IDM_LogginAction 5


#define IDC_ChangePortButton 2
#define IDC_ChangeAddressButton 4
#define IDC_LogginButton 6

#define NICKNAME_LENGTH 16
#define TEXT_LENGTH 256

struct Message {
	BOOL is_anonymous;
	wchar_t nickname[MAX_NICKNAME_LENGTH];
	wchar_t text[TEXT_LENGTH];
};

struct Declare {
	wchar_t text[TEXT_LENGTH];
};

typedef Declare DECLARE;
typedef Declare* HDECLARE;


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR hCursor, HINSTANCE hInst, HICON hIcon, LPCWSTR Name, WNDPROC Procedure);

void MainwndAddMenus(HWND hWnd);
void MainWndAddWidges(HWND hWnd);

HWND CreateChildChangePortWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK PortChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

HWND CreateChildChangeAddressWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK AddressChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

HWND CreateChildLogginWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK LogginProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


void IntToString(int num, wchar_t* str);
void LoadData();
void SaveData();
size_t StrLen(wchar_t* str);

// Установка соединения
void SetConnection(HWND hWnd);

// Закрыть соединение
void CloseConnection(HWND hWnd);

// Передача сообщения
void SendMsg(HWND hWnd);