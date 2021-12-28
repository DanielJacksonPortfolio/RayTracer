#include "MyObject.h"
#include "../../GlobalDefines.h"

void* MyObject::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetDefaultHeap(), "{MyObject}");
}
void MyObject::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* MyObject::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetDefaultHeap(), "{MyObject}");
}
void MyObject::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

MyObject::MyObject()
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} Constructor : " << this-sizeof(Header) << std::endl;);
}

MyObject::~MyObject()
{
    VERBOSE_OUTPUT(std::cout << "{MyObject} Deconstructor" << std::endl;);
}