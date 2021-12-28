#pragma once
#include "Heap.h"
class DefaultHeap :  public Heap
{
public:
	DefaultHeap(const char* name);
	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);
};

