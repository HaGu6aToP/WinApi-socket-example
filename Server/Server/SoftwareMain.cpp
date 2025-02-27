#define _CRT_SECURE_NO_WARNINGS
#include "SoftwareMain.h"

HWND g_hListViewConnections, g_hListViewDeclarations;
HWND g_hWndMain, g_hWndPortChange;
HWND g_hEditPortChange;
HINSTANCE g_hInst;

INT g_Port;
INT g_DeclaresCount = 0;
HDECLARE g_hDeclares = NULL;

wchar_t g_PortBuffer[16];

SOCKET g_Queue[5];
int g_QueueLength = 0;



SOCKET srv_socket;
int acc_sin_len;
SOCKADDR_IN acc_sin;
SOCKADDR_IN local_sin;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	g_hInst = hInstance;
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInstance, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);
	
	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	g_hWndMain = CreateWindow(L"MainWndClass", L"Server", WS_VISIBLE | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 520, 500, NULL, NULL, NULL, NULL);
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
			g_hWndPortChange = CreateChildWindow(g_hInst, hWnd, L"Port change", PortChangeProcedure);
			ShowWindow(g_hWndPortChange, SW_NORMAL);
			UpdateWindow(g_hWndPortChange);
			break;
		case IDM_Start:
			ServerStart(hWnd);
			break;
		case IDM_Stop:
			ServerStop(hWnd);
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

		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0) {
			MessageBox(NULL, L"Error occured while trying to init WSA lib", L"Error", MB_OK);
			break;
		}

		LoadData();
		break;
	case WSA_ACCEPT:
		OnWSAAccept(hWnd, msg, wp, lp);
		break;
	case WSA_NETEVENT:
		OnWSANetEvent(hWnd, msg, wp, lp); 
		break;
	case WM_DESTROY:
		SaveData();
		free(g_hDeclares);
		WSACleanup();
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainwndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU Settings = CreateMenu();
	HMENU Server = CreateMenu();
	
	AppendMenu(Server, MF_STRING, IDM_Start, L"Запустить сервер");
	AppendMenu(Server, MF_STRING, IDM_Stop, L"Остановить сервер");
	AppendMenu(Settings, MF_STRING, IDM_ChangePortMenuAction, L"Изменить прослушиваемый порт");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Settings, L"Настройки");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Server, L"Сервер");

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidges(HWND hWnd) {
	CreateWindow(L"static", L"Подключенные пользователи", WS_VISIBLE | WS_CHILD, 5, 5, 200, 20, hWnd, NULL, NULL, NULL);
	HWND hListView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT, 50, 25, 100, 102, hWnd, NULL, GetModuleHandle(NULL), NULL);
	
	ListView_SetExtendedListViewStyle(hListView, LVS_EX_GRIDLINES);

	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	 
	lvc.pszText = (LPWSTR)L"Nickname";
	lvc.cx = 100;
	lvc.iSubItem = 0;
	ListView_InsertColumn(hListView, 0, &lvc);

	LVITEM lvI;
	lvI.mask = LVIF_TEXT;
	lvI.iItem = 0;
	lvI.iSubItem = 0;
	lvI.pszText = (LPWSTR)L"-";
	ListView_InsertItem(hListView, &lvI);

	lvI.iItem = 1;
	lvI.pszText = (LPWSTR)L"-";
	ListView_InsertItem(hListView, &lvI);

	lvI.iItem = 2;
	lvI.pszText = (LPWSTR)L"-";
	ListView_InsertItem(hListView, &lvI);

	lvI.iItem = 3;
	lvI.pszText = (LPWSTR)L"-";
	ListView_InsertItem(hListView, &lvI);

	lvI.iItem = 4;
	lvI.pszText = (LPWSTR)L"-";
	ListView_InsertItem(hListView, &lvI);

	g_hListViewConnections = hListView;

	CreateWindow(L"static", L"Объявления", WS_VISIBLE | WS_CHILD, 5, 135, 200, 20, hWnd, NULL, NULL, NULL);
	hListView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT, 50, 155, 400, 250, hWnd, NULL, GetModuleHandle(NULL), NULL);

	ListView_SetExtendedListViewStyle(hListView, LVS_EX_GRIDLINES);


	lvc.pszText = (LPWSTR)L"Nickname";
	lvc.cx = 100;
	lvc.iSubItem = 0;
	ListView_InsertColumn(hListView, 0, &lvc);

	lvc.pszText = (LPWSTR)L"Text";
	lvc.cx = 300;
	lvc.iSubItem = 1;
	ListView_InsertColumn(hListView, 1, &lvc);

	g_hListViewDeclarations = hListView;
}

void EditListViewLine(HWND hListView, int iItem, LPWSTR text) {
	/*LVITEM lvI;
	lvI.mask = LVIF_TEXT;
	lvI.iItem = iItem;
	lvI.iSubItem = 0;
	lvI.pszText = text;
	ListView_InsertItem(hListView, &lvI);*/
	ListView_SetItemText(hListView, iItem, 0, text);
	
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

HWND CreateChildWindow(HINSTANCE hInst, HWND hWnd, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS w = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"ChildWindowPortChange", Procedure);
	RegisterClass(&w);
	HWND res = CreateWindow(L"ChildWindowPortChange", Name, WM_ACTIVATEAPP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 150, 150, 400, 110, hWnd, NULL, hInst, NULL);
	
	IntToString(g_Port, g_PortBuffer);
	CreateWindow(L"static", L"Прослушиваемый порт", WS_VISIBLE | WS_CHILD, 120, 0, 200, 20, res, NULL, NULL, NULL);
	g_hEditPortChange = CreateWindowEx(0, L"EDIT", g_PortBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 150, 20, 100, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindowEx(0, L"BUTTON", L"Изменить", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 45, 100, 20, res, (HMENU)IDC_ChangePortButton, GetModuleHandle(NULL), NULL);
	return res; 
}

void LoadData() {
	FILE *file = fopen("data.bin", "rb");
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

	if (fread(&g_DeclaresCount, sizeof(INT), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	g_hDeclares = (HDECLARE)malloc(g_DeclaresCount * sizeof(HDECLARE));

	if (g_hDeclares == NULL) {
		perror("Error occured while allocate memory");
		fclose(file);
		return;
	}

	if (fread(g_hDeclares, sizeof(HDECLARE), g_DeclaresCount, file) != g_DeclaresCount) {
		perror("Error occured while reading declares");
		fclose(file);
		return;
	}

	fclose(file);
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

	if (fwrite(&g_DeclaresCount, sizeof(INT), 1, file) != 1) {
		perror("Error occured while reading gile fata.txt");
		fclose(file);
		return;
	}

	if (fwrite(g_hDeclares, sizeof(HDECLARE), g_DeclaresCount, file) != g_DeclaresCount) {
		perror("Error occured while reading declares");
		fclose(file);
		return;
	}

	fclose(file);
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

void ServerStart(HWND hWnd) {
	struct sockaddr_in srv_address;
	int rc;

	srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_socket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Error occured when tried to create server socket", L"Error", MB_OK);
		return;
	}

	// Устанавливаем адрес IP и номер порта
	srv_address.sin_family = AF_INET;
	srv_address.sin_addr.s_addr = INADDR_ANY;
	srv_address.sin_port = htons(g_Port);

	// Связываем адрес IP с сокетом  
	if (bind(srv_socket, (LPSOCKADDR)&srv_address, sizeof(srv_address)) == SOCKET_ERROR)
	{
		// При ошибке закрываем сокет
		closesocket(srv_socket);
		MessageBox(NULL, L"Error ocured when tried to bin server socket", L"Error", MB_OK);
		return;
	}

	// Устанавливаем сокет в режим приема для
    // выполнения ожидания соединения с клиентом
	if (listen(srv_socket, MAX_QUEUE_LENGTH) == SOCKET_ERROR)
	{
		closesocket(srv_socket);
		MessageBox(NULL, L"Error occured when tried to listen server socket", L"Error", MB_OK);
		return;
	}

	// При попытке установки соединения главное окно приложения
    // получит сообщение WSA_ACCEPT
	if (WSAAsyncSelect(srv_socket, hWnd, WSA_ACCEPT, FD_ACCEPT) > 0)
	{
		closesocket(srv_socket);
		MessageBox(NULL, L"Error occured when tried to select server socket", L"Error", MB_OK);
		return;
	}

	MessageBox(NULL, L"Server started", L"Success", MB_OK);
}

void ServerStop(HWND hWnd) {
	// Отменяем приход любых извещений в главную функцию
	// окна при возникновении любых событий, связанных
	// с системой Windows Sockets
	WSAAsyncSelect(srv_socket, hWnd, 0, 0);

	// Если сокет был создан, закрываем его
	if (srv_socket != INVALID_SOCKET) {
		closesocket(srv_socket);
	}

	for (int i = 0; i < g_QueueLength; i++) {
		EditListViewLine(g_hListViewConnections, i, (LPWSTR)L"-");
	}

	g_QueueLength = 0;

	MessageBox(NULL, L"Server stopped", L"Success", MB_OK);
}

void OnWSAAccept(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int rc;

	// При ошибке отменяем поступление извещений
	// в главное окно приложения
	if (WSAGETSELECTERROR(lParam) != 0)
	{
		MessageBox(NULL, L"Error occured when tried to accept connection", L"Error", MB_OK);
		WSAAsyncSelect(srv_socket, hWnd, 0, 0);
		return;
	}

	// Определяем размер адреса сокета
	acc_sin_len = sizeof(acc_sin);

	// Разрешаем установку соединения
	srv_socket = accept(srv_socket, (LPSOCKADDR)&acc_sin, (int FAR*) & acc_sin_len);

	if (srv_socket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Accept error, invalid socket ", L"Error", MB_OK);
		return;
	}

	// Если на данном сокете начнется передача данных от
	// клиента, в главное окно приложения поступит 
	// сообщение WSA_NETEVENT.
	// Это же сообщение поступит при разрыве соединения
	

	if (WSAAsyncSelect(srv_socket, hWnd, WSA_NETEVENT, FD_READ | FD_CLOSE) > 0)
	{
		closesocket(srv_socket);
		MessageBox(NULL, L"Error occured when tryed select accepted socket", L"Error", MB_OK);
		return;
	}
	
	if (WSAGETSELECTEVENT(lParam) != FD_CLOSE) {
		EditListViewLine(g_hListViewConnections, g_QueueLength, (LPWSTR)L"Anonymous");
		g_QueueLength++;
	}

}

void OnWSANetEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char szTemp[256];
	int rc;

	// Если на сокете выполняется передача данных,
	// принимаем и отображаем эти данные в виде
	// текстовой строки
	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		rc = recv((SOCKET)wParam, szTemp, 256, 0);
		
		if (rc)
		{
			szTemp[rc] = '\0';
			MessageBox(NULL, L"Success", L"Reсeived data", MB_OK);
		}
		return;
	}

	// Если соединение завершено, выводми об этом сообщение
	else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		g_QueueLength--;
		EditListViewLine(g_hListViewConnections, g_QueueLength, (LPWSTR)L"-");
		MessageBox(NULL, L"Connection closed", L"Server", MB_OK);
	}
}

void Int64toString(wchar_t* buff, unsigned long long num) {
	for (int i = 0; i < 8; i++)
		buff[i] = num >> (8 - 1 - i) * 8;
}