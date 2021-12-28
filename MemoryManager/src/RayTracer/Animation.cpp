#include "Animation.h"
#include "../GlobalDefines.h"
#include "../MemoryManagement/HeapManager.h"

Animation::Animation()
{
    VERBOSE_OUTPUT(std::cout << "{Animation} Constructor : " << this-sizeof(Header) << std::endl;);
    spheres = new std::vector<SphereAnimation*>();
}

Animation::~Animation()
{
    VERBOSE_OUTPUT(std::cout << "{Animation} Deconstructor" << std::endl;);
    for (auto sphere : *spheres)
    {
        delete sphere;
    }
    delete spheres;
}

void* Animation::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Animation} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Animation"));
}
void Animation::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Animation} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* Animation::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Animation} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Animation"));
}
void Animation::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Animation} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

int Animation::GetLastFrame()
{
    int lastFrame = 0;

    for (auto sphere : *spheres)
    {
        int lastFrameOfSphere = sphere->GetLastFrame();
        if (lastFrameOfSphere > lastFrame)
        {
            lastFrame = lastFrameOfSphere;
        }
    }

    return lastFrame;
}

void Animation::AddNewSphereAnimation(SphereAnimation* animation)
{
    spheres->push_back(animation);
}

std::vector<SphereAnimation*>* Animation::GetSpheres()
{
    return this->spheres;
}
