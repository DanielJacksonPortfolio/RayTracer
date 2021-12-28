#pragma once
#include<unordered_map>
#include "MemoryPool.h"
#include "ManagedObject.h"

class PoolManager : public ManagedObject
{
public:
	static PoolManager* Instance();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

	static bool instanceIsNull;
	static bool creationInProgress;

	~PoolManager();
private:
	std::unordered_map<size_t, MemoryPool*> pools;
	static PoolManager* instance;

	PoolManager();

};

