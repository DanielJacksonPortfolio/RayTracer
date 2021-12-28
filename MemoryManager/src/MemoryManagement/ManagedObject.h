#pragma once

#include<string>
#include<stack>

#define CHECK_VAL 0xB00B5;

typedef unsigned char byte;

class Heap;

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, Heap* pHeap);
void* operator new[](size_t size, Heap* pHeap);
void* GlobalNew(size_t size, Heap* pHeap, bool useMemoryPool = true);

void operator delete(void* pMem);
void operator delete[](void* pMem);
void GlobalDelete(void* pMem); 

struct Header
{
	size_t size = 0;
	int checkValue;
	bool useMemoryPool;
	Heap* heapPtr;
	Header* nextHeader;
	Header* previousHeader;

	bool IsCorrect()
	{
		return this->checkValue == CHECK_VAL;
	}
};

struct Footer
{
	int reserved;
	int checkValue;

	bool IsCorrect()
	{
		return this->checkValue == CHECK_VAL;
	}
};

class ManagedObject
{
public:
    static void* operator new(size_t size, Heap* pHeap, bool useMemoryPool = true);
    static void* operator new[](size_t size, Heap* pHeap, bool useMemoryPool = true);
};

