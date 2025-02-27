#define _CRT_SECURE_NO_WARNINGS
#include "SoftwareMain.h"


HWND g_hStaticDeclaration, g_hStaticNickname;
HWND g_hButtonCreateDeclaration, g_hButtonNextDeclaration, g_hButtonPrevDeclaration, g_hButtonUpdate;
HMENU g_hMenuLogout, g_hMenuAccount;

HWND g_hWndMain;
HWND g_hEditPortChange, g_hWndPortChange;
HWND g_hWndAddressChange, g_hEditAddressChange;
HWND g_hWndLoggin, g_hEditLogginNickname, g_hEditLogginPassword, g_hButtonEnter;

BOOL g_isLogged = FALSE;

wchar_t g_Password[MAX_PASSWORD_LENGTH];
wchar_t g_Nickname[MAX_NICKNAME_LENGTH];

wchar_t g_PasswordBuffer[MAX_PASSWORD_LENGTH];
wchar_t g_NicknameBuffer[MAX_NICKNAME_LENGTH];

INT g_Port = 12000;
wchar_t g_Adress[MAX_ADDRESS_LENGTH] = L"127.0.0.1";
HDECLARE g_hDeclares = NULL;
HINSTANCE g_hInst;

wchar_t g_PortBuffer[16];
wchar_t g_AddressBuffer[16];

SOCKET srv_socket;
SOCKADDR_IN local_sin;
SOCKADDR_IN dest_sin;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInstance, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);
	g_hInst = hInstance;
	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	g_hWndMain = CreateWindow(L"MainWndClass", L"Client", WS_VISIBLE | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 720, 500, NULL, NULL, NULL, NULL);
	while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwareMainMessage);
		DispatchMessage(&SoftwareMainMessage);
	}
	return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR hCursor, HINSTANCE hInst, HICON hIcon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 };
	NWC.hCursor = hCursor;
	NWC.hIcon = hIcon;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case IDM_ChangePortMenuAction:
			g_hWndPortChange = CreateChildChangePortWindow(g_hInst, hWnd, L"Port change", PortChangeProcedure);
			ShowWindow(g_hWndPortChange, SW_NORMAL);
			UpdateWindow(g_hWndPortChange);
			break;
		case IDM_ChangeAddressMenuAction:
			g_hWndAddressChange = CreateChildChangeAddressWindow(g_hInst, hWnd, L"Adress change", AddressChangeProcedure);
			ShowWindow(g_hWndPortChange, SW_NORMAL);
			UpdateWindow(g_hWndPortChange);
			break;
		case IDM_LogginAction:
			g_hWndLoggin = CreateChildLogginWindow(g_hInst, hWnd, L"Loggin", LogginProcedure);
			ShowWindow(g_hWndPortChange, SW_NORMAL);
			UpdateWindow(g_hWndPortChange);
			break;
		}
		break;
	case WM_CREATE:
		MainwndAddMenus(hWnd);
		MainWndAddWidges(hWnd);

		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		LoadData();

		WSADATA  WSAData;
		// Инициализация и проверка версии Windows Sockets
		
		if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
		{
			MessageBox(NULL, L"WSAStartup error", L"Error", MB_OK);
			return FALSE;
		}

		SetConnection(hWnd);

		break;
	case WM_DESTROY:
		SaveData();
		WSACleanup();
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainwndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU Settings = CreateMenu();
	HMENU Account = CreateMenu();

	AppendMenu(Settings, MF_STRING, IDM_ChangePortMenuAction, L"Изменить порт");
	AppendMenu(Settings, MF_STRING, IDM_ChangeAddressMenuAction, L"Изменить адрес");

	AppendMenu(Account, MF_STRING, IDM_LogginAction, L"Войти");
	AppendMenu(Account, MF_STRING, NULL, L"Выйти");
	
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Settings, L"Настройки");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Account, L"Профиль");

	EnableMenuItem(Account, 1, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);

	g_hMenuAccount = Account;

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidges(HWND hWnd) {
	g_hStaticDeclaration = CreateWindowEx(0, L"STATIC", L"None", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,200, 50, 300, 300, hWnd, NULL, GetModuleHandle(NULL), NULL);
	g_hStaticDeclaration = CreateWindowEx(0, L"STATIC", L"Logged as: ", WS_CHILD | WS_VISIBLE | SS_CENTER, 520, 5, 150, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	
	g_hButtonCreateDeclaration = CreateWindowEx(0, L"BUTTON", L"Создать объявление", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 25, 200, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	g_hButtonPrevDeclaration = CreateWindowEx(0, L"BUTTON", L"<=", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 130, 30, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	g_hButtonNextDeclaration = CreateWindowEx(0, L"BUTTON", L"=>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 540, 130, 30, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
	g_hButtonUpdate = CreateWindowEx(0, L"BUTTON", L"Обновить", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 300, 355, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);

}

void EditListViewLine(HWND hListView, int iItem, LPWSTR text) {

}

void IntToString(int num, wchar_t* str) {
	int i = 0, isNegative = 0;

	if (num < 0) {
		isNegative = 1;
		num = -num;
	}

	do {
		str[i++] = (num % 10) + '0';
		num /= 10;
	} while (num > 0);

	if (isNegative) {
		str[i++] = -1;
	}

	str[i] = '\0';

	int start = 0, end = i - 1;
	char temp;
	while (start < end) {
		temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

HWND CreateChildChangePortWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS w = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"ChildWindowPortChange", Procedure);
	RegisterClass(&w);
	HWND res = CreateWindow(L"ChildWindowPortChange", Name, WM_ACTIVATEAPP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 150, 150, 400, 110, hWnd, NULL, hInst, NULL);

	IntToString(g_Port, g_PortBuffer);
	CreateWindow(L"static", L"Прослушиваемый порт", WS_VISIBLE | WS_CHILD, 120, 0, 200, 20, res, NULL, NULL, NULL);
	g_hEditPortChange = CreateWindowEx(0, L"EDIT", g_PortBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 148, 20, 100, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindowEx(0, L"BUTTON", L"Изменить", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 148, 45, 100, 20, res, (HMENU)IDC_ChangePortButton, GetModuleHandle(NULL), NULL);
	return res;
}
LRESULT CALLBACK PortChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case IDC_ChangePortButton:
			if (GetWindowText(g_hEditPortChange, g_PortBuffer, sizeof(g_PortBuffer)) > 0) {
				if (swscanf_s(g_PortBuffer, L"%d", &g_Port) != 1) {
					perror("Error occured while reading port");
				}
				SendMessage(hWnd, WM_CLOSE, wp, lp);
			}
			break;
		}
		break;
	case WM_CREATE:

		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

HWND CreateChildChangeAddressWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS w = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"ChildWindowAddressChange", Procedure);
	RegisterClass(&w);
	HWND res = CreateWindow(L"ChildWindowAddressChange", Name, WM_ACTIVATEAPP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 150, 150, 400, 110, hWnd, NULL, hInst, NULL);

	CreateWindow(L"static", L"Адрес", WS_VISIBLE | WS_CHILD, 180, 0, 200, 20, res, NULL, NULL, NULL);
	g_hEditAddressChange = CreateWindowEx(0, L"EDIT", g_Adress, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 20, 110, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindowEx(0, L"BUTTON", L"Изменить", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 45, 100, 20, res, (HMENU)IDC_ChangeAddressButton, GetModuleHandle(NULL), NULL);
	return res;
}
LRESULT CALLBACK AddressChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case IDC_ChangeAddressButton:
			if (GetWindowText(g_hEditAddressChange, g_AddressBuffer, sizeof(g_AddressBuffer)) > 0) {
				
				for (int i = 0; i < 16; i++)
					g_Adress[i] = g_AddressBuffer[i];

				SendMessage(hWnd, WM_CLOSE, wp, lp);
			}
			break;

		}
		break;
	case WM_CREATE:

		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

HWND CreateChildLogginWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS w = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"ChildWindowLoggin", Procedure);
	RegisterClass(&w);
	HWND res = CreateWindow(L"ChildWindowLoggin", Name, WM_ACTIVATEAPP | WS_VISIBLE | WS_SYSMENU, 150, 150, 200, 140, hWnd, NULL, hInst, NULL);

	CreateWindow(L"static", L"Логин: ", WS_VISIBLE | WS_CHILD, 5, 10, 60, 20, res, NULL, NULL, NULL);
	g_hEditLogginNickname = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 10, 110, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindow(L"static", L"Пароль: ", WS_VISIBLE | WS_CHILD, 5, 40, 60, 20, res, NULL, NULL, NULL);
	g_hEditLogginPassword = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 70, 40, 110, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindowEx(0, L"BUTTON", L"Войти", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 80, 70, 100, 20, res, (HMENU)IDC_LogginButton, GetModuleHandle(NULL), NULL);

	return res;
}
LRESULT CALLBACK LogginProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case IDC_LogginButton:
			int nickname_length = GetWindowText(g_hEditLogginNickname, g_NicknameBuffer, sizeof(g_NicknameBuffer));
			int password_length = GetWindowText(g_hEditLogginPassword, g_PasswordBuffer, sizeof(g_PasswordBuffer));
			
			if (nickname_length == 0 || password_length == 0) {
				break;
			}
			if (nickname_length >= MAX_NICKNAME_LENGTH || password_length >= MAX_PASSWORD_LENGTH) {
				MessageBox(hWnd, L"Max length of nickname or password was exceeded", L"Error", MB_OK);
				break;
			}

			
			
			break;
		}
		break;
	case WM_CREATE:

		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void SaveData() {
	FILE* file = fopen("data.bin", "wb");
	if (file == NULL) {
		perror("Error occured while opening file data.txt");
		fclose(file);
		return;
	}

	if (fwrite(&g_Port, sizeof(INT), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	if (fwrite(&g_Adress, sizeof(wchar_t[16]), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	fclose(file);
}
void LoadData() {
	FILE* file = fopen("data.bin", "rb");
	if (file == NULL) {
		perror("Error occured while opening file data.txt");
		fclose(file);
		return;
	}

	if (fread(&g_Port, sizeof(INT), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	if (fread(&g_Adress, sizeof(wchar_t[16]), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	fclose(file);
}
size_t StrLen(wchar_t* str) {
	int i = 0;
	while (str[i] != '\0') {
		i++;
	}
	return i;
}

void SetConnection(HWND hWnd)
{
	PHOSTENT  phe;

	// Создаем сокет 
	srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_socket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Error occured when tried to create socket", L"Error", MB_OK);
		return;
	}

	// Устанавливаем адрес IP и номер порта
	dest_sin.sin_family = AF_INET;

	// Определяем адрес узла
	// Адрес локального узла для отладки
	//phe = gethostbyname("localhost");
	//perror(NULL);
	//if (phe == NULL)
	//{
	//	closesocket(srv_socket);
	//	MessageBox(NULL, L"gethostbyaddr error", L"Error", MB_OK);
	//	return;
	//}

	//// Копируем адрес узла
	//memcpy((char FAR*) & (dest_sin.sin_addr), phe->h_addr, phe->h_length);

	char buff[16];
	wsprintfA(buff, "%ls", g_Adress);
	dest_sin.sin_addr .s_addr = inet_addr (buff);

	// Копируем номер порта
	dest_sin.sin_port = htons(g_Port);

	// Устанавливаем соединение
	if (connect(srv_socket, (PSOCKADDR)&dest_sin,
		sizeof(dest_sin)) < 0)
	{
		closesocket(srv_socket);
		MessageBox(NULL, L"connect error", L"Error", MB_OK);
		return;
	}

	// В случае успеха выводим сообщение об установке
	// соединения с узлом
	MessageBox(NULL, L"Connected to server", L"Success", MB_OK);
}
void CloseConnection(HWND hWnd) {
	// Если сокет был создан, закрываем его
	if (srv_socket != INVALID_SOCKET) {
		closesocket(srv_socket);
	}
}