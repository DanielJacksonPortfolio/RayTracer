#pragma once
#include<iostream>
#include<unordered_map>
#include "ManagedObject.h"

class DefaultHeap;
class Heap;

class HeapManager : public ManagedObject
{
public:
	static HeapManager* Instance();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	static void BeginHeapScope(Heap* heap);
	static void EndHeapScope();

	static Heap* GetDefaultHeap();
	Heap* GetHeap(const char* heapName);

	static Heap* CurrentHeapScope;

	~HeapManager();
private:

	static DefaultHeap* defaultHeap;

	std::unordered_map<const char*, Heap*> heaps;

	static HeapManager* instance;
	static std::stack<Heap*>* HeapStack;

	HeapManager();
};

