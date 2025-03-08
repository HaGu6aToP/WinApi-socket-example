#pragma once

#include <Windows.h>
#include <winsock.h>
#include <CommCtrl.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <memory>


#pragma comment (lib, "comctl32.lib")
#pragma comment(lib, "Wsock32.lib")

#define MAX_NICKNAME_LENGTH 16 // with '\0'
#define MAX_PASSWORD_LENGTH 16 
#define MAX_ADDRESS_LENGTH 16
#define MAX_TEXT_LENGTH 256


#define IDM_ChangePortMenuAction 1
#define IDC_ChangePortButton 2
#define IDM_ChangeAddressMenuAction 3
#define IDC_ChangeAddressButton 4
#define IDM_LogginAction 5
#define IDC_LogginButton 6
#define IDM_LoggoutAction 7
#define IDC_CreateAdvertiseButton 8
#define IDC_MakeAdvertisesSubmitButton 9
#define IDC_UpdateButton 10
#define IDC_NextAdvertiseButton 11
#define IDC_PrevAdvertiseButton 12



struct Message {
	char nickname[MAX_NICKNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char text[MAX_TEXT_LENGTH];
	int update;
};

typedef Message MESSAGE;
typedef Message* HMESSAGE;

struct LoggedMessage {
	int logged;
};

typedef LoggedMessage LOGGEDMESSAGE;
typedef LoggedMessage* HLOGGEDMESSAGE;

void SetMessage(HMESSAGE m, char nickname[MAX_NICKNAME_LENGTH], char password[MAX_PASSWORD_LENGTH], char text[MAX_TEXT_LENGTH]);
void ClearMessage(HMESSAGE m); 
void ClearArray(char* a, int length);

struct Declare {
	wchar_t text[MAX_TEXT_LENGTH];
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

HWND CreateChildMakeAdvertiseWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK MakeAdvertiseProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


void IntToString(int num, wchar_t* str);
void LoadData();
void SaveData();
size_t StrLen(wchar_t* str);

// Установка соединения
void SetConnection(HWND hWnd);

// Закрыть соединение
void CloseConnection(HWND hWnd);

// Передача сообщения
// 1 - обновить объявления
void SendMsg(int update);

void ShowAdvertise();
