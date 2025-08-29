#ifndef HELPERS_H
#define HELPERS_H
#include "Arduino.h"


template<typename... Args>
void PRINT(Args... args);

template<typename T, typename... Args>
void PRINT(T t, Args... args)
{
    DEBUG.print(t);
    PRINT(args...);
}

template<> void PRINT()
{
    DEBUG.println();
}




#endif