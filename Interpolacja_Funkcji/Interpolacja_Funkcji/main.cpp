#include <windows.h>
#include <vector>

#define ID_BTN_ASM 501
#define ID_BTN_C 502

LPCWSTR NazwaKlasy = (LPCWSTR)L"Klasa Okienka";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
MSG Komunikat;
HWND g_hBtn_Asm, g_hBtn_C, g_hRamka1, g_hRamka2, h_xCoord, h_yCoord, hText1, hText2, hResAsm, hResCpp, h_xParam;
HFONT hNormalFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
DWORD dlugosc;
LPWSTR Bufor;
double* xCoords, * yCoords, xParam, resultLaG, resultAit;
int  xCount, yCount;
typedef double(*MYPROC)(double*,double*, int,double);
MYPROC LaGC;

double cvec_to_double(std::vector<char> c_vec) {
    int point_loc, point_dist;
    point_loc = 0;
    point_dist = 0;
    double result, val_holder;
    result = 0;
    val_holder = 0;
    for (int i = 0; i < c_vec.size(); i++) {
        if (c_vec[i] == '.') {
            point_loc = i;
            break;
        }
    }
    for (int i = 0; i < c_vec.size(); i++) {
        if (c_vec[i] == '.') {
            continue;
        }
        point_dist = point_loc - i;
        val_holder = (int)c_vec[i] - 48;
        if (point_dist > 0) {
            for (int i = 0; i < (point_dist - 1); i++) {
                val_holder = val_holder * 10;
            }
            result += val_holder;
        }
        else {
            for (int i = 0; i < (point_dist * (-1)); i++) {
                val_holder = val_holder / 10;
            }
            result += val_holder;
        }
    }
    return result;
}
double* convert_buffer(LPWSTR buffer, int length, int& count) {
    std::vector<char> c_vec;
    std::vector<double> d_vec;
    double* result;
    double helper;
    for (int i = 0 ; i < length; i++) {
        if (buffer[i] == ',') {
            d_vec.push_back(cvec_to_double(c_vec));
            c_vec.clear();
        }
        else if (buffer[i] == '.') {
            c_vec.push_back(buffer[i]);
        }
        else if ((int)buffer[i] < 58 && (int)buffer[i] > 47) {
                c_vec.push_back(buffer[i]);
            
        }
        else {
            return NULL;
        }
    }
    d_vec.push_back(cvec_to_double(c_vec));
    result = new double[d_vec.size()];
    count = d_vec.size();
    for (int i = 0; i < d_vec.size(); i++) {
        helper = d_vec[i];
        result[i] = helper;
    }
    return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HINSTANCE hDll;
    hDll = LoadLibrary((LPCWSTR)L"InterpolacjaCpp");
    LaGC = (MYPROC)GetProcAddress(hDll, "LaGrangeFinal");

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
        MessageBox(NULL, (LPCWSTR)L"Tworzenie okna nie powiod�o si�!", (LPCWSTR)L"Niestety...",
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    HWND hwnd;

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, (LPCWSTR)L"Test interpolacji funkcji", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    g_hBtn_Asm = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w ASM", WS_CHILD | WS_VISIBLE,
        30, 250, 150, 30, hwnd, (HMENU)ID_BTN_ASM, hInstance, NULL);
    SendMessage(g_hBtn_Asm, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hBtn_C = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w C", WS_CHILD | WS_VISIBLE,
        200, 250, 150, 30, hwnd, (HMENU)ID_BTN_C, hInstance, NULL);
    SendMessage(g_hBtn_C, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka1 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wprowadzanie danych", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 5, 280, 350, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka1, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka2 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wyniki pomiar�w", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        285, 5, 280, 350, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka2, WM_SETFONT, (WPARAM)hNormalFont, 0);
    h_xCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 50, 150, 20, hwnd, NULL, hInstance, NULL);
    h_yCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 100, 150, 20, hwnd, NULL, hInstance, NULL);
    h_xParam = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 150, 40, 20, hwnd, NULL, hInstance, NULL);
    hText1 = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 30, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hText1, (LPCWSTR)L"Wprowad� wsp�rz�dne x:");
    SendMessage(hText1, WM_SETFONT, (WPARAM)hNormalFont, 0);
    hText2 = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 80, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hText2, (LPCWSTR)L"Wprowad� wsp�rz�dne y:");
    SendMessage(hText2, WM_SETFONT, (WPARAM)hNormalFont, 0);
    hResAsm = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 50, 200, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResAsm, (LPCWSTR)L"Rezultat Asm:");
    SendMessage(hResAsm, WM_SETFONT, (WPARAM)hNormalFont, 0);
    hResCpp = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 80, 200, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResCpp, (LPCWSTR)L"Rezultat c++:");
    SendMessage(hResCpp, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    if (hwnd == NULL)
    {
        MessageBox(NULL, (LPCWSTR)L"Okno odm�wi�o przyj�cia na �wiat!", (LPCWSTR)L"Ale kicha...", MB_ICONEXCLAMATION);
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
        switch (wParam) {
        case ID_BTN_ASM:
            dlugosc = GetWindowTextLength(h_xParam);
            Bufor = (LPWSTR)GlobalAlloc(GPTR, dlugosc + 1);
            GetWindowText(h_xParam, Bufor, dlugosc + 1);
            xParam = convert_buffer(Bufor, dlugosc, xCount)[0];
            dlugosc = GetWindowTextLength(h_xCoord);
            Bufor = (LPWSTR)GlobalAlloc(GPTR, dlugosc + 1);
            GetWindowText(h_xCoord, Bufor, dlugosc + 1);
            xCoords = convert_buffer(Bufor, dlugosc, xCount);
            dlugosc = GetWindowTextLength(h_yCoord);
            GetWindowText(h_yCoord, Bufor, dlugosc + 1);
            yCoords = convert_buffer(Bufor, dlugosc, yCount);
            if (xCount != yCount) {
                MessageBox(hwnd, (LPCWSTR)L"B��dna ilo�� wsp�rz�dnych", (LPCWSTR)L"Ok", MB_ICONSTOP);
                break;
            }
            resultLaG = LaGC(xCoords, yCoords, xCount, xParam);
            MessageBox(hwnd, (LPCWSTR)Bufor, (LPCWSTR)L"Ok", MB_ICONINFORMATION);
            break;
        case ID_BTN_C:
            MessageBox(hwnd, (LPCWSTR)L"Realizacja w C", (LPCWSTR)L"Ok", MB_ICONINFORMATION);
            SetWindowText(hResCpp, (LPCWSTR)L"Aaaa");
            break;
        default:
            break;
        }
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