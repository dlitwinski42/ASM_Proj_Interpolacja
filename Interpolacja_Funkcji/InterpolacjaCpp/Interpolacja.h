#pragma once
#include <thread>
#ifndef _DLL_H_
#define _DLL_H_

# define DLLIMPORT __declspec (dllexport)

extern "C" double DLLIMPORT kwadrat(double liczba);
extern "C" double DLLIMPORT LaGrangePartial(double* xCoord, double* yCoord, int length, int index, double xParam, double &final);
extern "C" double DLLIMPORT LaGrangeFinal(double* xCoord, double* yCoord, int length, double xParam, int threads);
extern "C" double DLLIMPORT Aitken(double* xCoord, double* yCoord, int length, double xParam, int threads);

class DLLIMPORT DllClass
{
public:
    DllClass();
    virtual ~DllClass(void);

private:

};

#endif /* _DLL_H_ */