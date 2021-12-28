#include "GraphicsObject.h"
#include "../../GlobalDefines.h"

void* GraphicsObject::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} New Called" << std::endl;)
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Graphics"), "{GraphicsObject}");
}
void GraphicsObject::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} Delete Called" << std::endl;)
    ::operator delete(pMem);
}
void* GraphicsObject::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} New[] Called" << std::endl;)
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Graphics"), "{GraphicsObject}");
}
void GraphicsObject::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} Delete[] Called" << std::endl;)
    ::operator delete[](pMem);
}

GraphicsObject::GraphicsObject()
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} Constructor : " << this-sizeof(Header) << std::endl;)
}

GraphicsObject::~GraphicsObject()
{
    VERBOSE_OUTPUT(std::cout << "{GraphicsObject} Deconstructor" << std::endl;)
}