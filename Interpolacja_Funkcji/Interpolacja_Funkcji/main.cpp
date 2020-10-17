#include <windows.h>

LPCWSTR NazwaKlasy = (LPCWSTR)L"Klasa Okienka";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
MSG Komunikat;
HWND g_hBtn_Asm, g_hBtn_C, g_hRamka1, g_hRamka2, h_xCoord, h_yCoord;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = {
    wc.cbSize = sizeof(WNDCLASSEX),
    wc.style = 0,
    wc.lpfnWndProc = WndProc,
    wc.cbClsExtra = 0,
    wc.cbWndExtra = 0,
    wc.hInstance = hInstance,
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION),
    wc.hCursor = LoadCursor(NULL, IDC_ARROW),
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    wc.lpszMenuName = NULL,
    wc.lpszClassName = NazwaKlasy,
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, (LPCWSTR)L"Tworzenie okna nie powiod³o siê!", (LPCWSTR)L"Niestety...",
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    HWND hwnd;

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, (LPCWSTR)L"Oto okienko", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    g_hBtn_Asm = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w ASM", WS_CHILD | WS_VISIBLE,
        30, 100, 150, 30, hwnd, NULL, hInstance, NULL);
    g_hBtn_C = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w C", WS_CHILD | WS_VISIBLE,
        200, 100, 150, 30, hwnd, NULL, hInstance, NULL);
    g_hRamka1 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wprowadzanie danych", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 5, 280, 350, hwnd, NULL, hInstance, NULL);
    g_hRamka2 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wyniki pomiarów", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        285, 5, 280, 350, hwnd, NULL, hInstance, NULL);
    h_xCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 50, 150, 20, hwnd, NULL, hInstance, NULL);
    h_yCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 80, 150, 20, hwnd, NULL, hInstance, NULL);
    
    if (hwnd == NULL)
    {
        MessageBox(NULL, (LPCWSTR)L"Okno odmówi³o przyjœcia na œwiat!", (LPCWSTR)L"Ale kicha...", MB_ICONEXCLAMATION);
        return 1;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Komunikat, NULL, 0, 0))
    {
        TranslateMessage(&Komunikat);
        DispatchMessage(&Komunikat);
    }

    return Komunikat.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if ((HWND)lParam == g_hBtn_Asm)
            MessageBox(hwnd, (LPCWSTR)L"Realizacja w Asm", (LPCWSTR)L"Ok", MB_ICONINFORMATION);
        if ((HWND)lParam == g_hBtn_C)
            MessageBox(hwnd, (LPCWSTR)L"Realizacja w C", (LPCWSTR)L"Ok", MB_ICONINFORMATION);

        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}