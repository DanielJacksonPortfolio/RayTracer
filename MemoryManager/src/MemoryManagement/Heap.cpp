#include "Heap.h"
#include "HeapManager.h"
#include "../GlobalDefines.h"

Heap::Heap(const char* name)
{
	this->name = name;
}

void Heap::AddBytesAllocated(size_t numBytes)
{
	mutex.lock();
	this->numberOfBytesAllocated += numBytes;
	mutex.unlock();
}
void Heap::RemoveBytesAllocated(size_t numBytes)
{
	mutex.lock();
	this->numberOfBytesAllocated -= numBytes;
	mutex.unlock();
}

size_t Heap::GetNumberOfBytesAllocated()
{
	mutex.lock();
	size_t numBytes = this->numberOfBytesAllocated;
	mutex.unlock();
	return numBytes;
}

void* Heap::operator new(size_t size)
{
	VERBOSE_OUTPUT(std::cout << "{Heap} New Called" << std::endl;)
	return ManagedObject::operator new(size, HeapManager::GetDefaultHeap());
}

void Heap::operator delete(void* pMem)
{
	VERBOSE_OUTPUT(std::cout << "{Heap} Delete Called" << std::endl;)
	::operator delete(pMem);
}
void* Heap::operator new[](size_t size)
{
	VERBOSE_OUTPUT(std::cout << "{Heap} New[] Called" << std::endl;)
	return ManagedObject::operator new[](size, HeapManager::GetDefaultHeap());
}
void Heap::operator delete[](void* pMem)
{
	VERBOSE_OUTPUT(std::cout << "{Heap} Delete[] Called" << std::endl;)
	::operator delete[](pMem);
}

const char* Heap::GetType()
{
	mutex.lock();
	const char* name = this->name;
	mutex.unlock();
	return name;
}

Header* Heap::GetCurrentHeader()
{
	mutex.lock();
	Header* header = this->currentHeader;
	mutex.unlock();
	return header;
}

void Heap::SetCurrentHeader(Header* header)
{
	mutex.lock();
	if (this->firstHeader == nullptr)
	{
		firstHeader = header;
	}
	this->currentHeader = header;
	mutex.unlock();
}

void Heap::RemoveHeader(Header* header)
{
	mutex.lock();
	if (this->firstHeader == header)
	{
		firstHeader = header->nextHeader;
	}
	if (this->currentHeader == header)
	{
		currentHeader = header->previousHeader;
	}
	if (header->previousHeader != nullptr)
	{
		header->previousHeader->nextHeader = header->nextHeader;
	}
	if (header->nextHeader != nullptr)
	{
		header->nextHeader->previousHeader = header->previousHeader;
	}
	mutex.unlock();
}

std::recursive_mutex* Heap::GetMutex()
{
	return &this->mutex;
}

void Heap::Walk(bool forward)
{
	mutex.lock();
	std::cout << "Walk "<< this->name <<" Heap" << std::endl;
	Header* header = forward ? this->firstHeader : this->currentHeader;
	while (header != nullptr)
	{
		std::cout << header << ", Is Correct: " << (header->IsCorrect() ? "TRUE" : "FALSE") << ", Size: " << header->size << std::endl;
		header = forward ? header->nextHeader : header->previousHeader;
	}
	std::cout << std::endl;
	mutex.unlock();
}
