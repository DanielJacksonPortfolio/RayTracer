#pragma once
#include "MyObject.h"
class SoundObject :  public MyObject
{
public:
    static void* operator new(size_t size);
    static void operator delete(void* pMem);
    static void* operator new[](size_t size);
    static void operator delete[](void* pMem);
    SoundObject();
    ~SoundObject();
};

