#include "HeapManager.h"
#include "PoolManager.h"
#include "../GlobalDefines.h"
#include <iostream>

PoolManager* PoolManager::instance = nullptr;
bool PoolManager::instanceIsNull = PoolManager::instance == nullptr;
bool PoolManager::creationInProgress = false;

PoolManager* PoolManager::Instance()
{
    if (instance == nullptr)
    {
        creationInProgress = true;
        instance = new PoolManager();
        creationInProgress = false;
        instanceIsNull = false;
    }
    return instance;
}

void* PoolManager::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{PoolManager} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::GetDefaultHeap(), false);
}
void PoolManager::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{PoolManager} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* PoolManager::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{PoolManager} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::GetDefaultHeap(), false);
}
void PoolManager::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{PoolManager} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

void* PoolManager::Allocate(size_t size)
{
    if (pools.find(size) == pools.end())
    {
        creationInProgress = true;
        HeapManager::BeginHeapScope(HeapManager::GetDefaultHeap());
        pools[size] = new MemoryPool(16);
        HeapManager::EndHeapScope();
        creationInProgress = false;
    }
    return pools[size]->Allocate(size);
}

void PoolManager::Deallocate(void* ptr, size_t size)
{
    return pools[size]->Deallocate(ptr, size);
}

PoolManager::PoolManager()
{
    pools = std::unordered_map<size_t, MemoryPool*>(); 
}

PoolManager::~PoolManager()
{
    for (auto pool : pools)
    {
        delete pool.second;
    }
}
