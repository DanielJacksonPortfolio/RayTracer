#include "SoundObject.h"
#include "../../GlobalDefines.h"

void* SoundObject::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} New Called" << std::endl;)
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Sound"), "{SoundObject}");
}
void SoundObject::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} Delete Called" << std::endl;)
    ::operator delete(pMem);
}
void* SoundObject::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} New[] Called" << std::endl;)
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Sound"), "{SoundObject}");
}
void SoundObject::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} Delete[] Called" << std::endl;)
    ::operator delete[](pMem);
}

SoundObject::SoundObject()
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} Constructor : " << this-sizeof(Header) << std::endl;)
}

SoundObject::~SoundObject()
{
    VERBOSE_OUTPUT(std::cout << "{SoundObject} Deconstructor" << std::endl;)
}