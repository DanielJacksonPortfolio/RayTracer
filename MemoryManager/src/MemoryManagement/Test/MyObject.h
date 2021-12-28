#pragma once
#include<iostream>
#include "../ManagedObject.h"
#include "../HeapManager.h"

class MyObject : public ManagedObject
{
public:
    static void* operator new(size_t size);
    static void operator delete(void* pMem);
    static void* operator new[](size_t size);
    static void operator delete[](void* pMem);

    MyObject();
    ~MyObject();
};

