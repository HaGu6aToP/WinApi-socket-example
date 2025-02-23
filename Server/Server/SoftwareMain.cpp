#define _CRT_SECURE_NO_WARNINGS
#include "SoftwareMain.h"

HWND g_hListViewConnections, g_hListViewDeclarations;
HWND g_hWndMain, g_hWndPortChange;
HWND g_hWndPortChangeEdit;
HINSTANCE g_hInst;

INT g_Port;
INT g_DeclaresCount = 0;
HDECLARE g_hDeclares = NULL;

wchar_t g_PortBuffer[16];

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
			g_hWndPortChange = CreateChildWindow(g_hInst, hWnd, L"Port change", ConnectChangeProcedure);
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

		break;
	case WM_DESTROY:
		SaveData();
		free(g_hDeclares);
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainwndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU Settings = CreateMenu();

	AppendMenu(Settings, MF_STRING, IDM_ChangePortMenuAction, L"Изменить прослушиваемый порт");
	
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)Settings, L"Настройки");


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
	LVITEM lvI;
	lvI.mask = LVIF_TEXT;
	lvI.iItem = iItem;
	lvI.iSubItem = 0;
	lvI.pszText = text;
	ListView_InsertItem(hListView, &lvI);
}

LRESULT CALLBACK ConnectChangeProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case IDC_ChangePortButton:
			if (GetWindowText(g_hWndPortChangeEdit, g_PortBuffer, sizeof(g_PortBuffer)) > 0) {
				if (swscanf_s(g_PortBuffer, L"%d", &g_Port) != 1) {
					perror("Error occured while reading port");
				}
				SendMessage(hWnd, WM_CLOSE, wp, lp);
			}
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
	WNDCLASS w = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"ChildWindowConnectChange", Procedure);
	RegisterClass(&w);
	HWND res = CreateWindow(L"ChildWindowConnectChange", Name, WM_ACTIVATEAPP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 150, 150, 400, 110, hWnd, NULL, hInst, NULL);
	
	IntToString(g_Port, g_PortBuffer);
	CreateWindow(L"static", L"Прослушиваемый порт", WS_VISIBLE | WS_CHILD, 120, 0, 200, 20, res, NULL, NULL, NULL);
	g_hWndPortChangeEdit = CreateWindowEx(0, L"EDIT", g_PortBuffer, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 150, 20, 100, 20, res, NULL, GetModuleHandle(NULL), NULL);
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