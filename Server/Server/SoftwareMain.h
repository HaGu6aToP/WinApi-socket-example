#pragma once
#include <vector>
#include <memory>
#include <string>

#include <Windows.h>
#include <CommCtrl.h>
#include <winsock.h>
#include <stdio.h>
#pragma comment (lib, "comctl32.lib")
#pragma comment(lib, "Wsock32.lib")

#define MAX_NICKNAME_LENGTH 16 // with '\0'
#define MAX_PASSWORD_LENGTH 16 
#define MAX_TEXT_LENGTH 256

#define IDM_ChangePortMenuAction 1//
#define IDC_ChangePortButton 2
#define IDM_Start 3
#define IDM_Stop 4
#define MAX_QUEUE_LENGTH 5

#define WSA_ACCEPT (WM_USER + 1)
#define WSA_NETEVENT (WM_USER + 2)


struct ClientMessage {
	char nickname[MAX_NICKNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char text[MAX_TEXT_LENGTH];
	int update;
};
typedef ClientMessage CLIENTMESSAGE;
typedef ClientMessage* HCLIENTMESSAGE;

struct LoggedMessage {
	int logged; // 1 - успешное логирование, -1 - неверный пароль, 0 - без логирования
};

typedef LoggedMessage LOGGEDMESSAGE;
typedef LoggedMessage* HLOGGEDMESSAGE;

void SetMessage(HCLIENTMESSAGE m, char nickname[MAX_NICKNAME_LENGTH], char password[MAX_PASSWORD_LENGTH], char text[MAX_TEXT_LENGTH]);
void ClearMessage(HCLIENTMESSAGE m);

struct Declare{
	wchar_t nickname[MAX_NICKNAME_LENGTH];
	wchar_t text[MAX_TEXT_LENGTH];
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
void IntToString(int num, char* str);
void Int64toString(wchar_t* buff, unsigned long long num);

// Запуск сервера
void ServerStart(HWND hWnd);

// Останов сервера
void ServerStop(HWND hWnd);

// Обработка сообщения WSA_ACCEPT
void OnWSAAccept(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Обработка сообщения WSA_NETEVENT
void OnWSANetEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void AddUserInfo(const char nickname[MAX_NICKNAME_LENGTH], const char password[MAX_PASSWORD_LENGTH]);
// 1 если логин и пароль верны, 0 если такой пользователь не зарегистрирован, -1 если пароль не правильный
int CheckUser(const char* nickname, const char* password);
//int CheckUser(const char nickname[MAX_NICKNAME_LENGTH],const char password[MAX_PASSWORD_LENGTH]);
// 
// -1 если не найдено объявление с данным именем, иначе номер байта с которого начинается объявление
long long CheckAdvertisement(const char* nickname);

void AddAdvertisement(const char* nickname, const char* text);

void EditAdvertisement(long long position, const char* text);

void SendAdvertisements(SOCKET sock);
// 
// Равны ли строки
int MyCompareString(const char* str1, const char* str2, int l1, int l2);
int MyCompareString(const char* str1, const char* str2, int l);

void LoadAdvertations();
void AddNewAdvertiseToTable(const wchar_t* nickname, const wchar_t* text);
const wchar_t* StrToWStr(const char* str);