#define _CRT_SECURE_NO_WARNINGS
#include "SoftwareMain.h"

HWND g_hListViewConnections, g_hListViewAdvertations;
HWND g_hWndMain, g_hWndPortChange;
HWND g_hEditPortChange;
HWND g_hEditConnectionsCounter;
int g_counter = 0;
HINSTANCE g_hInst;

INT g_Port;
INT g_DeclaresCount = 0;
HDECLARE g_hDeclares = NULL;

wchar_t g_PortBuffer[16];

SOCKET g_Queue[5];
int g_QueueLength = 0;


char data_file[25] = "data.bin";
char users_file[25] = "users.bin";
char advertisements_file[25] = "advertisements.bin";

char g_nickname_buffer[MAX_NICKNAME_LENGTH];
char g_password_buffer[MAX_PASSWORD_LENGTH];

HCLIENTMESSAGE m;
CLIENTMESSAGE message = {};

SOCKET srv_socket, new_srv_socket;
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
	{
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
		LoadAdvertations();
		break;
	}
	case WSA_ACCEPT:
		OnWSAAccept(hWnd, msg, wp, lp);
		break;
	case WSA_NETEVENT:
		OnWSANetEvent(hWnd, msg, wp, lp); 
		break;
	case WM_DESTROY:
		SaveData();	
		free(g_hDeclares);
		closesocket(new_srv_socket);
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
	
	AppendMenu(Server, MF_STRING, IDM_Start, L"Launch server");
	AppendMenu(Server, MF_STRING, IDM_Stop, L"Stop server");
	AppendMenu(Settings, MF_STRING, IDM_ChangePortMenuAction, L"Change port");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Settings, L"Settings");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Server, L"Server");

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidges(HWND hWnd) {
	CreateWindow(L"static", L"Connected users: ", WS_VISIBLE | WS_CHILD, 5, 5, 200, 20, hWnd, NULL, NULL, NULL);
	HWND hListView = NULL;// = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT, 50, 25, 100, 102, hWnd, NULL, GetModuleHandle(NULL), NULL);
	g_hEditConnectionsCounter = CreateWindow(L"static", L"0", WS_CHILD | WS_VISIBLE | LVS_REPORT, 215, 5, 50, 20, hWnd, NULL, NULL, NULL);
	/*ListView_SetExtendedListViewStyle(hListView, LVS_EX_GRIDLINES);*/

	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	 
	g_hListViewConnections = hListView;

	CreateWindow(L"static", L"Advertisments", WS_VISIBLE | WS_CHILD, 5, 135, 200, 20, hWnd, NULL, NULL, NULL);
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

	g_hListViewAdvertations = hListView;
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
	CreateWindow(L"static", L"Listening port", WS_VISIBLE | WS_CHILD, 155, 0, 200, 20, res, NULL, NULL, NULL);
	g_hEditPortChange = CreateWindowEx(0, L"EDIT", g_PortBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 150, 20, 100, 20, res, NULL, GetModuleHandle(NULL), NULL);
	CreateWindowEx(0, L"BUTTON", L"Change", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 45, 100, 20, res, (HMENU)IDC_ChangePortButton, GetModuleHandle(NULL), NULL);
	return res; 
}

void LoadData() {
	FILE *file = fopen(data_file, "rb");
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
	FILE* file = fopen(data_file, "wb");
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

void IntToString(int num, char* str) {
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
	new_srv_socket = accept(srv_socket, (LPSOCKADDR)&acc_sin, (int FAR*) & acc_sin_len);

	if (new_srv_socket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Accept error, invalid socket ", L"Error", MB_OK);
		return;
	}

	// Если на данном сокете начнется передача данных от
	// клиента, в главное окно приложения поступит 
	// сообщение WSA_NETEVENT.
	// Это же сообщение поступит при разрыве соединения
	

	if (WSAAsyncSelect(new_srv_socket, hWnd, WSA_NETEVENT, FD_READ | FD_CLOSE) > 0)
	{
		closesocket(new_srv_socket);
		MessageBox(NULL, L"Error occured when tryed select accepted socket", L"Error", MB_OK);
		return;
	}
	
	if (WSAGETSELECTEVENT(lParam) != FD_CLOSE) {
		wchar_t buff[10];
		g_counter++;
		IntToString(g_counter, buff);
		SetWindowText(g_hEditConnectionsCounter, buff);
	}

}

void OnWSANetEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int rc;

	// Если на сокете выполняется передача данных,
	// принимаем и отображаем эти данные в виде
	// текстовой строки
	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		ClearMessage(&message);
		rc = recv((SOCKET)wParam, (char*)&message, sizeof(CLIENTMESSAGE), 0);
		
		LOGGEDMESSAGE lm = {};
		if (rc)
		{
			if (strlen(message.nickname) != 0 && strlen(message.password) != 0) {
				switch (CheckUser(message.nickname, message.password)) {
				case 0:
					AddUserInfo(message.nickname, message.password);
				case 1:
				{
					lm.logged = 1;
					int text_len = strlen(message.text);

					if (text_len != 0) {
						int position = CheckAdvertisement(message.nickname);
						if (position == -1) {
							AddAdvertisement(message.nickname, message.text);
						}
						else {
							EditAdvertisement(position, message.text);
						}
						LoadAdvertations();
					}

					break;
				}
				case -1:
					lm.logged = -1;
					break;
				}
				send((SOCKET)wParam, (char*)&lm, sizeof(LOGGEDMESSAGE), 0);
			}
			else {
				lm.logged = 0;
				send((SOCKET)wParam, (char*)&lm, sizeof(LOGGEDMESSAGE), 0);
			}



			if (message.update == 1) {
				SendAdvertisements((SOCKET)wParam);
			}
			
			
		}
		return;
	}

	// Если соединение завершено, выводми об этом сообщение
	else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		wchar_t buff[10];
		g_counter--;
		IntToString(g_counter, buff);
		SetWindowText(g_hEditConnectionsCounter, buff);
		closesocket((SOCKET)wParam);
	}
}

void Int64toString(wchar_t* buff, unsigned long long num) {
	for (int i = 0; i < 8; i++)
		buff[i] = num >> (8 - 1 - i) * 8;
}

void SetMessage(HCLIENTMESSAGE m, char nickname[MAX_NICKNAME_LENGTH], char password[MAX_PASSWORD_LENGTH], char text[MAX_TEXT_LENGTH]) {
	strcpy(m->nickname, nickname);
	strcpy(m->password, password);
	strcpy(m->text, text);
}

void ClearMessage(HCLIENTMESSAGE m) {
	int i;
	for (i = 0; i < MAX_NICKNAME_LENGTH; i++) {
		m->nickname[i] = '\0';
	}

	for (i = 0; i < MAX_PASSWORD_LENGTH; i++) {
		m->password[i] = '\0';
	}

	for (i = 0; i < MAX_TEXT_LENGTH; i++) {
		m->text[i] = '\0';
	}
}

void AddUserInfo(const char nickname[MAX_NICKNAME_LENGTH], const char password[MAX_PASSWORD_LENGTH]) {
	FILE* file = fopen(users_file, "ab");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return;
	}

	if (fwrite(nickname, sizeof(char), sizeof(char)*MAX_NICKNAME_LENGTH, file) != sizeof(char) * MAX_NICKNAME_LENGTH) {
		MessageBoxA(NULL, "Nickname write error", "Error", MB_OK);
		fclose(file);
		return;
	}

	if (fwrite(password, sizeof(char), sizeof(char)*MAX_PASSWORD_LENGTH, file) != sizeof(char) * MAX_PASSWORD_LENGTH) {
		MessageBoxA(NULL, "Password write error", "Error", MB_OK);
		fclose(file);
		return;
	}

	fclose(file);
}

int CheckUser(const char* nickname, const char* password) {
	char readed_nickname[MAX_NICKNAME_LENGTH];
	char readed_password[MAX_PASSWORD_LENGTH];

	FILE* file = fopen(users_file, "rb");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return -1;
	}

	int res;

	while (true) {

		if (fread(readed_nickname, sizeof(char), sizeof(readed_nickname), file) != sizeof(readed_nickname)) {
			if (feof(file)) {
				res = 0;
				break;
			}
			MessageBoxA(NULL, "Nickname read error", "Error", MB_OK);
			res = -1;
			break;
		}
		int k = fread(readed_password, sizeof(char), sizeof(readed_password), file);
		if (k != sizeof(readed_password)) {
			MessageBoxA(NULL, "Password read error", "Error", MB_OK);
			res = -1;
			break;
		}

		if (MyCompareString(readed_nickname, nickname, MAX_NICKNAME_LENGTH) == -1) {
			continue;
		}
		else {
			if (MyCompareString(readed_password, password, MAX_PASSWORD_LENGTH) == -1) {
				res = -1;
				break;
			}
			else {
				res = 1;
				break;
			}
		}

	}

	fclose(file);
	return res;

}

int MyCompareString(const char* str1, const char* str2, int l1, int l2) {
	if (l1 != l2)
		return -1;

	for (int i = 0; i < l1; i++) {
		if (str1[i] != str2[i])
			return -1;
		if (str1[i] == '\0')
			return 1;
	}
	return 1;
}
int MyCompareString(const char* str1,const char* str2, int l) {
	return MyCompareString(str1, str2, l, l);
}



long long CheckAdvertisement(const char* nickname) {
	FILE* file = fopen(advertisements_file, "rb");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return -1;
	}

	char readed_nickname[MAX_NICKNAME_LENGTH];
	char readed_text[MAX_TEXT_LENGTH];
	long long res;
	long long position = 0;
	while (true) {
		if(fread(readed_nickname, sizeof(char), sizeof(char) * MAX_NICKNAME_LENGTH, file) != sizeof(char)*MAX_NICKNAME_LENGTH) {
			if (feof(file)) {
				res = -1;
				break;
			}
			MessageBoxA(NULL, "Nickname read error", "Error", MB_OK);
			res = -1;
			break;
		}


		if (fread(readed_text, sizeof(char), sizeof(char) * MAX_TEXT_LENGTH, file) != sizeof(char)*MAX_TEXT_LENGTH) {
			MessageBoxA(NULL, "Text read error", "Error", MB_OK);
			res = -1;
			break;
		}

		position += sizeof(char) * MAX_NICKNAME_LENGTH;

		if (MyCompareString(readed_nickname, nickname, MAX_NICKNAME_LENGTH) == -1) {
			position += sizeof(char) * MAX_TEXT_LENGTH;
			continue;
		}
		else {
			res = position;
			break;
		}
	}
	fclose(file);
	return res;
}

void AddAdvertisement(const char* nickname, const char* text) {
	FILE* file = fopen(advertisements_file, "ab");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return;
	}

	if (fwrite(nickname, sizeof(char), sizeof(char) * MAX_NICKNAME_LENGTH, file) != sizeof(char) * MAX_NICKNAME_LENGTH) {
		MessageBoxA(NULL, "Nickname write error", "Error", MB_OK);
		fclose(file);
		return;
	}

	if (fwrite(text, sizeof(char), sizeof(char) * MAX_TEXT_LENGTH, file) != sizeof(char) * MAX_TEXT_LENGTH) {
		MessageBoxA(NULL, "text write error", "Error", MB_OK);
		fclose(file);
		return;
	}

	fclose(file);
}

void EditAdvertisement(long long position, const char* text) {
	FILE* file = fopen(advertisements_file, "rb+");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return;
	}

	if (fseek(file, position, SEEK_SET) != 0) {
		MessageBoxA(NULL, "Seek error", "Error", MB_OK);
		fclose(file);
		return;
	}

	if (fwrite(text, sizeof(char), sizeof(char) * MAX_TEXT_LENGTH, file) != sizeof(char) * MAX_TEXT_LENGTH) {
		MessageBoxA(NULL, "Write text error", "Error", MB_OK);
		fclose(file);
		return;
	}

	fclose(file);
	return;
}

void SendAdvertisements(SOCKET sock) {
	FILE* file = fopen(advertisements_file, "rb");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return;
	}

	char readed_nickname[MAX_NICKNAME_LENGTH];
	char readed_text[MAX_TEXT_LENGTH];

	std::vector<std::unique_ptr<char*>> advertisements{};

	while (true) {
		if (fread(readed_nickname, sizeof(char), sizeof(char) * MAX_NICKNAME_LENGTH, file) != sizeof(char) * MAX_NICKNAME_LENGTH) {
			if (feof(file)) {
				//send(sock, "", sizeof(char)*MAX_TEXT_LENGTH, 0);
				break;
			}
			MessageBoxA(NULL, "Nickname read error", "Error", MB_OK);
			fclose(file);
			return;
		}

		if (fread(readed_text, sizeof(char), sizeof(char) * MAX_TEXT_LENGTH, file) != sizeof(char) * MAX_TEXT_LENGTH) {
			MessageBoxA(NULL, "Text read error", "Error", MB_OK);
			fclose(file);
			return;
		}
		char* newstr = new char[MAX_TEXT_LENGTH];
		strcpy(newstr, readed_text);
		std::unique_ptr<char*> str = std::make_unique<char*>(newstr);

		advertisements.push_back(std::move(str));
		
		

		//send(sock, readed_text, sizeof(char) * MAX_TEXT_LENGTH, 0);
	}
	int len = advertisements.size();
	char buff[256];
	IntToString(len, buff);
	send(sock, buff, sizeof(char)*MAX_TEXT_LENGTH, 0);
	for (std::vector<std::unique_ptr<char*>>::iterator i = advertisements.begin(); i != advertisements.end(); i++) {
		send(sock, *((*i).get()), sizeof(char) * MAX_TEXT_LENGTH, 0);
	}

	fclose(file);
}

void LoadAdvertations() {
	FILE* file = fopen(advertisements_file, "rb");

	if (file == NULL) {
		MessageBoxA(NULL, "Open file error", "Error", MB_OK);
		return;
	}

	char readed_nickname[MAX_NICKNAME_LENGTH];
	char readed_text[MAX_TEXT_LENGTH];
	const wchar_t* wnickname, *wtext;
	ListView_DeleteAllItems(g_hListViewAdvertations);
	while (true) {
		if (fread(readed_nickname, sizeof(char), sizeof(char) * MAX_NICKNAME_LENGTH, file) != sizeof(char) * MAX_NICKNAME_LENGTH) {
			if (feof(file)) {
				break;
			}
			MessageBoxA(NULL, "Nickname read error", "Error", MB_OK);
			break;
		}


		if (fread(readed_text, sizeof(char), sizeof(char) * MAX_TEXT_LENGTH, file) != sizeof(char) * MAX_TEXT_LENGTH) {
			MessageBoxA(NULL, "Text read error", "Error", MB_OK);
			break;
		}
		
		wnickname = StrToWStr(readed_nickname);
		wtext = StrToWStr(readed_text);

		AddNewAdvertiseToTable(wnickname, wtext);
		free((void*)wnickname);
		free((void*)wtext);
		
	}
	fclose(file);
}

void AddNewAdvertiseToTable(const wchar_t* nickname, const wchar_t* text) {

	LVITEM lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(g_hListViewAdvertations);
	lvi.iSubItem = 0;
	lvi.pszText = (LPWSTR)nickname;

	int index = ListView_InsertItem(g_hListViewAdvertations, &lvi);
	ListView_SetItemText(g_hListViewAdvertations, index, 1, (LPWSTR)text);

}

const wchar_t* StrToWStr(const char* str) {
	wchar_t* wstr = (wchar_t*)malloc((strlen(str) + 1) * sizeof(wchar_t));
	for (int i = 0; i < strlen(str) + 1; i++) {
		wstr[i] = L'\0';
	}
	mbstowcs(wstr, str, strlen(str));
	return wstr;
}