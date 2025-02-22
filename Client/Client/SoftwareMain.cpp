#include "SoftwareMain.h"

HWND g_hStaticDeclaration, g_hStaticNickname;
HWND g_hButtonCreateDeclaration, g_hButtonNextDeclaration, g_hButtonPrevDeclaration, g_hButtonUpdate;
HMENU g_hMenuLogout, g_hMenuAccount; 

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);
	
	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"Client", WS_VISIBLE | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 720, 500, NULL, NULL, NULL, NULL);
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
	HMENU Account = CreateMenu();

	AppendMenu(Settings, MF_STRING, NULL, L"Изменить порт");
	AppendMenu(Settings, MF_STRING, NULL, L"Изменить адрес");

	AppendMenu(Account, MF_STRING, NULL, L"Войти");
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