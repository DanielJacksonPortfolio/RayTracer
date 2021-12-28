#include "DefaultHeap.h"
#include "../GlobalDefines.h"

DefaultHeap::DefaultHeap(const char* name) : Heap(name)
{
}

void* DefaultHeap::operator new(size_t size)
{
	VERBOSE_OUTPUT(std::cout << "{DefaultHeap} New Called" << std::endl;)
	return ManagedObject::operator new(size, nullptr, false);
}

void DefaultHeap::operator delete(void* pMem)
{
	VERBOSE_OUTPUT(std::cout << "{DefaultHeap} Delete Called" << std::endl;)
	::operator delete(pMem);
}

void* DefaultHeap::operator new[](size_t size)
{
	VERBOSE_OUTPUT(std::cout << "{DefaultHeap} New[] Called" << std::endl;)
    return ManagedObject::operator new[](size, nullptr, false);
}
void DefaultHeap::operator delete[](void* pMem)
{
	VERBOSE_OUTPUT(std::cout << "{DefaultHeap} Delete[] Called" << std::endl;)
    ::operator delete[](pMem);
}