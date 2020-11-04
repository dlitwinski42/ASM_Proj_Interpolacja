#include "Interpolacja.h"
#include <windows.h>

DllClass::DllClass()
{

}

DllClass::~DllClass()
{

}

BOOL APIENTRY DllMain(HINSTANCE hInst /* Library instance handle. */,
    DWORD reason /* Reason this function is being called. */,
    LPVOID reserved /* Not used. */)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_PROCESS_DETACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

double kwadrat(double liczba)
{
    return liczba * liczba;
}

double LaGrangePartial(double* xCoord, double* yCoord, int length, int index, double xParam, double &final) {
    double result = 1;
    result *= yCoord[index];
    for (int i = 0; i < length; i++) {
        if (i != index) {
            result *= (xParam - xCoord[i]);
            result /= (xCoord[index] - xCoord[i]);
        }
    }
    final += result;
    return result;
}

double LaGrangeFinal(double* xCoord, double* yCoord, int length, double xParam, int threads) {
    double result = 0;
    std::thread* myThreads = new std::thread[length];
    if (threads > length) {
        threads = length;
    }
    int done = 0;
    while (done < length) {
        if ((done + threads) > length) {
            threads = length - done;
        }
        for (int i = 0; i < threads; i++) {
            myThreads[i] = std::thread(LaGrangePartial, xCoord, yCoord, length, i + done, xParam, std::ref(result));
        }
        for (int i = 0; i < threads; i++) {
            myThreads[i].join();
        }
        done += threads;
    }
    delete[] myThreads;
    return result;
}

/*
double LaGrangePartial(double* xCoord, double* yCoord, int length, int index, double xParam) {
    double result = 1;
    result *= yCoord[index];
    for (int i = 0; i < length; i++) {
        if (i != index) {
            result *= (xParam - xCoord[i]);
            result /= (xCoord[index] - xCoord[i]);
        }
    }
    return result;
}

double LaGrangeFinal(double* xCoord, double* yCoord, int length, double xParam, int threads) {
    double result = 0;
    for (int i = 0; i < length; i++) {
       result += LaGrangePartial(xCoord, yCoord, length, i, xParam);
    }
    return result;
}
*/
double AitkenPart(double Wij, double Wik, double xParam, double xj, double xk, double& final) {
    final = ((Wij * (xk - xParam) - Wik * (xj - xParam)) / (xk - xj));
    return final;
}

double Aitken(double* xCoord, double* yCoord, int count, double xParam, int threads) {
    double result = 0;
    if (threads > count) {
        threads = count;
    }
    std::thread* myThreads = new std::thread[threads];
    //Przygotowanie tablicy dwuwymiarowej
    int tLength = count - 1;
    double** tab = new double* [tLength];
    for (int i = 0; i < tLength; i++) {
        tab[i] = new double[tLength];
    }
    //Wype³nienie tablicy zerami
    for (int i = 0; i < tLength; i++) {
        for (int j = 0; j < tLength; j++) {
            tab[i][j] = 0;
        }
    }
    //Przygotowanie pierwszego rzêdu
    for (int i = 0; i < tLength; i++) {

        tab[0][i] = (yCoord[0] * (xCoord[i + 1] - xParam));
        tab[0][i] -= (yCoord[i + 1] * (xCoord[0] - xParam));
        tab[0][i] /= (xCoord[i + 1] - xCoord[0]);
    }
    //Wype³nienie reszty kaskadowo
    for (int x = 1; x < tLength; x++) {
        int y = x;
        while (y < tLength) {
            if ((y + threads) > tLength) {
                threads = tLength - y;
            }
            for (int i = 0; i < threads; i++) {
                myThreads[i] = std::thread(AitkenPart, tab[x - 1][x - 1], tab[x - 1][y + i], xParam, xCoord[x], xCoord[y + 1 + i], std::ref(tab[x][y + i]));
            }
            for (int i = 0; i < threads; i++) {
                myThreads[i].join();
            }
            y += threads;
        }
    }
    result = tab[tLength - 1][tLength - 1];
    for (int i = 0; i < tLength; i++) {
        delete[] tab[i];
    }
    delete[] tab;
    return result;
}

/*
double Aitken(double* xCoord, double* yCoord, int count, double xParam) {
    //Przygotowanie tablicy dwuwymiarowej
    int tLength = count - 1;
    double** tab = new double* [tLength];
    for (int i = 0; i < tLength; i++) {
        tab[i] = new double[tLength];
    }
    //Wype³nienie tablicy zerami
    for (int i = 0; i < tLength; i++) {
        for (int j = 0; j < tLength; j++) {
            tab[i][j] = 0;
        }
    }
    //Przygotowanie pierwszego rzêdu
    for (int i = 0; i < tLength; i++) {

        tab[0][i] = (yCoord[0] * (xCoord[i + 1] - xParam));
        tab[0][i] -= (yCoord[i + 1] * (xCoord[0] - xParam));
        tab[0][i] /= (xCoord[i + 1] - xCoord[0]);
    }
    //Wype³nienie reszty kaskadowo
    for (int x = 1; x < tLength; x++) {
        for (int y = x; y < tLength; y++) {
            double Wij, Wik, xj, xk;
            Wij = tab[x - 1][x - 1];
            Wik = tab[x - 1][y];
            xj = xCoord[x];
            xk = xCoord[y + 1];
            tab[x][y] = ((Wij * (xk - xParam) - Wik * (xj - xParam)) / (xk - xj));
        }
    }
    return tab[tLength - 1][tLength - 1];
}
*/

