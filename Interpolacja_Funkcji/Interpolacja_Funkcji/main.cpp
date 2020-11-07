#include <windows.h>
#include <strsafe.h>
#include <vector>
#include <chrono>
#include <thread>

#define ID_BTN_ASM 501
#define ID_BTN_C 502

LPCWSTR NazwaKlasy = (LPCWSTR)L"Klasa Okienka";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
MSG Komunikat;
HWND g_hBtn_Asm, g_hBtn_C, g_hRamka1, g_hRamka2, g_hRamka3, g_hRamka4, g_hRadio_Auto, g_hRadio_Input, h_xCoord, h_yCoord, hText1, hText2, hLblLC, hLblAC,
hResLC, hResAC, hLblLA, hLblAA, hResLA, hResAA, hResultLbl, hResultVal, h_xParam, h_Cores, g_hRamkaCores, g_hRamkaResult, h_xParamCap, h_coreInfo;
HFONT hNormalFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
DWORD dlugosc;
LPWSTR Bufor;
double* xCoords, * yCoords, xParam, resultLaG, resultAit;
int  xCount, yCount;
typedef double(*MYPROC2)(double*, double*, int, double, int);
typedef double(*MYPROC)(double*,double*, int,double);
typedef int(_fastcall* MyProc1)(double, double, double*, double*, int, int);
typedef int(_fastcall* MianLaG)(double*, double*, int, int);
typedef int(_fastcall* Aitk)(double*, double, double ,double, double, double);
MyProc1 LicznikLaG;
MYPROC2 LaGC, AitC;
MianLaG MianownikLaG;
Aitk Aitken;
int avThreads;

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
    int sign = 1;
    for (int i = 0 ; i < length; i++) {
        if (buffer[i] == ',') {
            d_vec.push_back((cvec_to_double(c_vec) * (sign)));
            c_vec.clear();
            sign = 1;
        }
        else if (buffer[i] == '.') {
            c_vec.push_back(buffer[i]);
        }
        else if (buffer[i] == '-') {
            if ((i != 0) && buffer[i - 1] != ',') {
                return NULL;
            }
            sign = -1;
        }
        else if ((int)buffer[i] < 58 && (int)buffer[i] > 47) {
                c_vec.push_back(buffer[i]);
            
        }
        else {
            return NULL;
        }
    }
    d_vec.push_back((cvec_to_double(c_vec) * (sign)));
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
    LaGC = (MYPROC2)GetProcAddress(hDll, "LaGrangeFinal");
    AitC = (MYPROC2)GetProcAddress(hDll, "Aitken");
    HINSTANCE dllHandle = NULL;
    dllHandle = LoadLibrary(L"InterpolacjaAsm.dll");
    LicznikLaG = (MyProc1)GetProcAddress(dllHandle, "LicznikLaGrange");
    MianownikLaG = (MianLaG)GetProcAddress(dllHandle, "MianownikLaGrange");
    Aitken = (Aitk)GetProcAddress(dllHandle, "Aitken");


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

    avThreads = std::thread::hardware_concurrency();

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, (LPCWSTR)L"Tworzenie okna nie powiod³o siê!", (LPCWSTR)L"Niestety...",
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    HWND hwnd;

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, (LPCWSTR)L"Test interpolacji funkcji", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    g_hBtn_Asm = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w ASM", WS_CHILD | WS_VISIBLE,
        290, 280, 270, 30, hwnd, (HMENU)ID_BTN_ASM, hInstance, NULL);
    SendMessage(g_hBtn_Asm, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hBtn_C = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrealizuj w C", WS_CHILD | WS_VISIBLE,
        290, 320, 270, 30, hwnd, (HMENU)ID_BTN_C, hInstance, NULL);
    SendMessage(g_hBtn_C, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka1 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wprowadzanie danych", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 5, 280, 180, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka1, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka2 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wyniki pomiarów", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        285, 5, 280, 350, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka2, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka3 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Czasy C++", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        290, 25, 275, 90, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka3, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamka4 = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Czasy Asm", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        290, 105, 275, 100, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamka4, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamkaCores = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wybór rdzeni", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 180, 280, 180, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamkaCores, WM_SETFONT, (WPARAM)hNormalFont, 0);
    g_hRamkaResult = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Wynik", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        290, 200, 275, 80, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRamkaResult, WM_SETFONT, (WPARAM)hNormalFont, 0);
    h_xCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 50, 150, 20, hwnd, NULL, hInstance, NULL);
    h_yCoord = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 100, 150, 20, hwnd, NULL, hInstance, NULL);
    h_xParam = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 150, 40, 20, hwnd, NULL, hInstance, NULL);
    g_hRadio_Auto = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Auto", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        50, 200, 60, 20, hwnd, NULL, hInstance, NULL);
    g_hRadio_Input = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Iloœæ:", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        50, 220, 60, 20, hwnd, NULL, hInstance, NULL);
    h_Cores = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        120, 220, 40, 20, hwnd, NULL, hInstance, NULL);
    hResultLbl = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 30, 150, 20, hwnd, NULL, hInstance, NULL);
    hResultVal = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 30, 150, 20, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hRadio_Auto, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(g_hRadio_Auto, WM_SETFONT, (WPARAM)hNormalFont, 0);
    SendMessage(g_hRadio_Input, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hText1 = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 30, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hText1, (LPCWSTR)L"WprowadŸ wspó³rzêdne x:");
    SendMessage(hText1, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hText2 = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 80, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hText2, (LPCWSTR)L"WprowadŸ wspó³rzêdne y:");
    SendMessage(hText2, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hLblLC = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 50, 100, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hLblLC, (LPCWSTR)L"Czas LaGrange:");
    SendMessage(hLblLC, WM_SETFONT, (WPARAM)hNormalFont, 0);
   
    hResLC = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 400, 50, 60, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResLC, (LPCWSTR)L"0.0000");
    SendMessage(hResLC, WM_SETFONT, (WPARAM)hNormalFont, 0);
   
    hResAC = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 400, 80, 60, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResAC, (LPCWSTR)L"0.0000");
    SendMessage(hResAC, WM_SETFONT, (WPARAM)hNormalFont, 0);
   
    hLblAC = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 80, 100, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hLblAC, (LPCWSTR)L"Czas Aitken:");
    SendMessage(hLblAC, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hLblLA = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 130, 100, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hLblLA, (LPCWSTR)L"Czas LaGrange:");
    SendMessage(hLblLA, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hResLA = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 400, 130, 60, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResLA, (LPCWSTR)L"0.0000");
    SendMessage(hResLA, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hResAA = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 400, 160, 60, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResAA, (LPCWSTR)L"0.0000");
    SendMessage(hResAA, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hLblAA = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 160, 100, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hLblAA, (LPCWSTR)L"Czas Aitkena:");
    SendMessage(hLblAA, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    hResultVal = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 400, 230, 60, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResultVal, (LPCWSTR)L"0.0000");
    SendMessage(hResultVal, WM_SETFONT, (WPARAM)hNormalFont, 0);
   
    hResultLbl = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 300, 230, 100, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hResultLbl, (LPCWSTR)L"Wynik:");
    SendMessage(hResultLbl , WM_SETFONT, (WPARAM)hNormalFont, 0);
    
    h_xParamCap = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 130, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(h_xParamCap, (LPCWSTR)L"WprowadŸ parametr x:");
    SendMessage(h_xParamCap, WM_SETFONT, (WPARAM)hNormalFont, 0);

    TCHAR coreInfo[32];
    StringCchPrintf(coreInfo, sizeof(coreInfo) / sizeof(TCHAR), TEXT("Dostêpna iloœæ w¹tków: %d"), avThreads);
    h_coreInfo = hResultLbl = CreateWindowEx(0, (LPCWSTR)L"STATIC", NULL, WS_CHILD | WS_VISIBLE |
        SS_LEFT, 50, 250, 150, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(h_coreInfo, (LPCWSTR)coreInfo);
    SendMessage(h_coreInfo, WM_SETFONT, (WPARAM)hNormalFont, 0);
    
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
    double timeElapsed;
    TCHAR buff[32];
    switch (msg)
    {
    case WM_COMMAND:
        switch (wParam) {
        case ID_BTN_ASM:{

            if (SendMessage(g_hRadio_Input, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                dlugosc = GetWindowTextLength(h_Cores);
                Bufor = (LPWSTR)GlobalAlloc(GPTR, dlugosc + 1);
                GetWindowText(h_Cores, Bufor, dlugosc + 1);
                if ((int)Bufor[0] < 58 && (int)Bufor[0] > 48) {
                    avThreads = (int)Bufor[0] - 48;
                }
                else {
                    MessageBox(hwnd, (LPCWSTR)L"B³êdna iloœæ w¹tków", (LPCWSTR)L"Ok", MB_ICONSTOP);
                }
            }
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
                MessageBox(hwnd, (LPCWSTR)L"B³êdna iloœæ wspó³rzêdnych", (LPCWSTR)L"Ok", MB_ICONSTOP);
                break;
            }
            
            double test = 0;
            LicznikLaG(yCoords[0], xParam, xCoords, &(test), 0, xCount);
            MianownikLaG(xCoords, &(test), 0, xCount);
            //Aitken(wynik, Wij, Wik, xk, xj, xParam)
            //Aitken(wynik, yi, yj, xj, xi, xParam)
            Aitken(&(test),yCoords[0],yCoords[1],xCoords[1],xCoords[0],xParam);

            MessageBox(hwnd, (LPCWSTR)L"Realizacja w ASM", (LPCWSTR)L"Ok", MB_ICONINFORMATION);
        }   break;
        case ID_BTN_C: {
            if (SendMessage(g_hRadio_Input, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                dlugosc = GetWindowTextLength(h_Cores);
                Bufor = (LPWSTR)GlobalAlloc(GPTR, dlugosc + 1);
                GetWindowText(h_Cores, Bufor, dlugosc + 1);
                if ((int)Bufor[0] < 58 && (int)Bufor[0] > 48) {
                    avThreads = (int)Bufor[0] - 48;
                }
                else {
                    MessageBox(hwnd, (LPCWSTR)L"B³êdna iloœæ w¹tków", (LPCWSTR)L"Ok", MB_ICONSTOP);
                }
            }
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
                MessageBox(hwnd, (LPCWSTR)L"B³êdna iloœæ wspó³rzêdnych", (LPCWSTR)L"Ok", MB_ICONSTOP);
                break;
            }
            
            
            auto start_lc = std::chrono::steady_clock::now();
            resultLaG = LaGC(xCoords, yCoords, xCount, xParam, avThreads);
            auto end_lc = std::chrono::steady_clock::now();
            timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_lc - start_lc).count();
            StringCchPrintf(buff, sizeof(buff) / sizeof(TCHAR), TEXT("%0.10f"), timeElapsed);
            SetWindowText(hResLC, (LPCWSTR)buff);
            auto start_ac = std::chrono::steady_clock::now();
            resultAit = AitC(xCoords, yCoords, xCount, xParam, avThreads);
            auto end_ac = std::chrono::steady_clock::now();
            timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ac - start_ac).count();
            StringCchPrintf(buff, sizeof(buff) / sizeof(TCHAR), TEXT("%0.10f"), timeElapsed);
            SetWindowText(hResAC, (LPCWSTR)buff);
            if (abs((resultAit - resultLaG)) < 0.0001) {

                StringCchPrintf(buff, sizeof(buff) / sizeof(TCHAR), TEXT("%f"), resultAit);
                SetWindowText(hResultVal, (LPCWSTR)buff);
            }
            else {
                MessageBox(hwnd, (LPCWSTR)L"Coœ posz³o nie tak!", (LPCWSTR)L"B³¹d!", MB_ICONINFORMATION);
            }
           
        } break;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        delete[] xCoords;
        delete[] yCoords;
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