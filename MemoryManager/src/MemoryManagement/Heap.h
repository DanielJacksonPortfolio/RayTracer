#pragma once

#include <iostream>
#include "ManagedObject.h"
#include <mutex>

class Heap : public ManagedObject
{
public:
	Heap(const char* name);
	~Heap() = default;

	void AddBytesAllocated(size_t numBytes);
	void RemoveBytesAllocated(size_t numBytes);
	size_t GetNumberOfBytesAllocated();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	const char* GetType();

	Header* GetCurrentHeader();
	void SetCurrentHeader(Header* header);

	void RemoveHeader(Header* header);

	std::recursive_mutex* GetMutex();

	void Walk(bool forward = true);

protected:
	const char* name = "";
	size_t numberOfBytesAllocated = 0;
	Header* currentHeader = nullptr;
	Header* firstHeader = nullptr;
	std::recursive_mutex mutex;
};

