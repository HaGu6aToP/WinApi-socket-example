#include "SoftwareMain.h"

HWND g_hListViewConnections, g_hListViewDeclarations;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);
	
	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"Server", WS_VISIBLE | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 520, 500, NULL, NULL, NULL, NULL);
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

		}
		break;
	case WM_CREATE:
		MainwndAddMenus(hWnd);
		MainWndAddWidges(hWnd);

		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainwndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU Settings = CreateMenu();

	AppendMenu(Settings, MF_STRING, NULL, L"Изменить прослушиваемый порт");
	
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