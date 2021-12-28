#include "ManagedObject.h"
#include "HeapManager.h"
#include "Heap.h"
#include "../GlobalDefines.h"
#include "PoolManager.h"
#include <thread>

void* operator new(size_t size)
{
	VERBOSE_OUTPUT(std::cout << "Global New Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, HeapManager::CurrentHeapScope == nullptr ? HeapManager::GetDefaultHeap() : HeapManager::CurrentHeapScope);
}

void* operator new[](size_t size)
{
	VERBOSE_OUTPUT(std::cout << "Global New[] Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, HeapManager::CurrentHeapScope == nullptr ? HeapManager::GetDefaultHeap() : HeapManager::CurrentHeapScope);
}

void* operator new(size_t size, Heap* pHeap)
{
	VERBOSE_OUTPUT(std::cout << "Global New (Heap) Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, pHeap);
}

void* operator new[](size_t size, Heap* pHeap)
{
	VERBOSE_OUTPUT(std::cout << "Global New[] (Heap) Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, pHeap);
}

void* ManagedObject::operator new(size_t size, Heap* pHeap, bool useMemoryPool)
{
	VERBOSE_OUTPUT(std::cout << "Global (MO) New (Heap) Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, pHeap, useMemoryPool);
}

void* ManagedObject::operator new[](size_t size, Heap* pHeap, bool useMemoryPool)
{
	VERBOSE_OUTPUT(std::cout << "Global (MO) New[] (Heap) Called on Thread " << std::this_thread::get_id() << std::endl;);
	return GlobalNew(size, pHeap, useMemoryPool);
}


void* GlobalNew(size_t size, Heap* pHeap, bool useMemoryPool)
{
	size_t totalSize = size + sizeof(Header) + sizeof(Footer);
	byte* allocatedMemory = nullptr;
	bool usingMemoryPool = !((!useMemoryPool || (PoolManager::instanceIsNull && PoolManager::creationInProgress))  || PoolManager::creationInProgress);
	usingMemoryPool = false;

	allocatedMemory = usingMemoryPool ? (byte*)PoolManager::Instance()->Allocate(totalSize) : (byte*)malloc(totalSize);

	if(!allocatedMemory)
	{
		std::bad_alloc ba;
		throw ba;
	}

	Header* pHeader = (Header*)allocatedMemory;
	pHeader->size = size;
	pHeader->checkValue = CHECK_VAL;
	pHeader->heapPtr = pHeap;
	pHeader->useMemoryPool = usingMemoryPool;
	if (pHeader->heapPtr == nullptr)
	{
		VERBOSE_OUTPUT(std::cout << "Heap Type = " << "NULL" << std::endl;)
	}
	else
	{
		pHeader->heapPtr->GetMutex()->lock();
		VERBOSE_OUTPUT(std::cout << "Heap Type = " << pHeader->heapPtr->GetType() << std::endl;)
		pHeader->heapPtr->AddBytesAllocated(totalSize);
		pHeader->nextHeader = nullptr;
		pHeader->previousHeader = pHeap->GetCurrentHeader();
		if (pHeap->GetCurrentHeader() != nullptr)
		{
			pHeap->GetCurrentHeader()->nextHeader = pHeader;
		}
		pHeap->SetCurrentHeader(pHeader);
		pHeader->heapPtr->GetMutex()->unlock();
	}

	void* pFooterAddress = allocatedMemory + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddress;
	pFooter->checkValue = CHECK_VAL;

	void* pData = allocatedMemory + sizeof(Header);
	return pData;
}

void operator delete(void* pMem)
{
	GlobalDelete(pMem);
}
void operator delete[](void* pMem)
{
	GlobalDelete(pMem);
}

void GlobalDelete(void* pMem)
{
	VERBOSE_OUTPUT(std::cout << "Global Delete Called" << std::endl;);

	Header* pHeader = (Header*)((byte*)pMem - sizeof(Header));
	Footer* pFooter = (Footer*)((byte*)pMem + pHeader->size);

	size_t totalSize = pHeader->size + sizeof(Header) + sizeof(Footer);

	if (!pHeader->IsCorrect())
	{
		std::cout << "Header Check: " << pHeader->checkValue << ", INCORRECT" << std::endl;
	}
	else
	{
		VERBOSE_OUTPUT(std::cout << "Header Check: CORRECT" << std::endl;);
		if (pHeader->heapPtr != nullptr)
		{
			pHeader->heapPtr->GetMutex()->lock();
			pHeader->heapPtr->RemoveBytesAllocated(totalSize);
			pHeader->heapPtr->RemoveHeader(pHeader);
			pHeader->heapPtr->GetMutex()->unlock();
		}
	}
	if (!pFooter->IsCorrect())
	{
		std::cout << "Footer Check: " << pFooter->checkValue << ", INCORRECT" << std::endl;
	}
	else
	{
		VERBOSE_OUTPUT(std::cout << "Footer Check: CORRECT" << std::endl;);
	}

	pHeader->useMemoryPool ? PoolManager::Instance()->Deallocate(pHeader, totalSize) : free(pHeader);
}