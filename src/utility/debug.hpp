#ifndef HELPERS_H
#define HELPERS_H
#include "Arduino.h"

#ifndef DEBUG_PROPERTY
#define DEBUG_PROPERTY Serial
#endif

template<typename... Args>
void PRINT(Args... args);

template<typename T, typename... Args>
void PRINT(T t, Args... args)
{
    DEBUG_PROPERTY.print(t);
    PRINT(args...);
}

template<> void PRINT()
{
    DEBUG_PROPERTY.println();
}




#endif