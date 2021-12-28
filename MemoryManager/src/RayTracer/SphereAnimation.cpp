#include "SphereAnimation.h"
#include "../GlobalDefines.h"
#include "../MemoryManagement/HeapManager.h"

SphereAnimation::SphereAnimation()
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} Constructor : " << this-sizeof(Header) << std::endl;);
    keyframes = new std::vector<Sphere*>();
}

SphereAnimation::~SphereAnimation()
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} Deconstructor" << std::endl;);

    for (auto keyFrame : *keyframes)
    {
        delete keyFrame;
    }
    delete keyframes;
}

void* SphereAnimation::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Animation"));
}
void SphereAnimation::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* SphereAnimation::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Animation"));
}
void SphereAnimation::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{SphereAnimation} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

int SphereAnimation::GetLastFrame()
{
    int lastFrame = 0;
    for (auto keyframe : *keyframes)
    {
        if (keyframe->frameNum > lastFrame)
        {
            lastFrame = keyframe->frameNum;
        }
    }
    return lastFrame;
}

void SphereAnimation::AddNewKeyFrame(Sphere* keyframe)
{
    keyframes->push_back(keyframe);
}

float lerpFloat(const float& float1, const float& float2, float ratio)
{
    float diff = float2 - float1;
    return float1 + (diff * ratio);
}

Sphere SphereAnimation::GetSphereAtFrame(int frameNum)
{
    if (keyframes->size() > 0)
    {
        Sphere* lastKeyFrame = (*keyframes)[0];
        Sphere* nextKeyFrame = (*keyframes)[0];
        for (auto keyframe : *keyframes)
        {
            if (keyframe != lastKeyFrame)
            {
                if (keyframe->frameNum <= frameNum && keyframe->frameNum > lastKeyFrame->frameNum)
                {
                    lastKeyFrame = keyframe;
                }
            }
            if (keyframe != nextKeyFrame)
            {
                if (keyframe->frameNum > frameNum)
                {
                    if (keyframe->frameNum < nextKeyFrame->frameNum || nextKeyFrame->frameNum < frameNum)
                    {
                        nextKeyFrame = keyframe;
                    }
                }
            }
        }
        if (nextKeyFrame->frameNum < frameNum)
        {
            lastKeyFrame = nextKeyFrame;
        }
        if (lastKeyFrame == nextKeyFrame)
        {
            return *lastKeyFrame;
        }
        else
        {
            float ratio = (float)(frameNum - lastKeyFrame->frameNum) / (float)(nextKeyFrame->frameNum - lastKeyFrame->frameNum);

            Vector3f centre = Vector3f::lerp(lastKeyFrame->center, nextKeyFrame->center, ratio);
            Vector3f surfaceColor = Vector3f::lerp(lastKeyFrame->surfaceColor, nextKeyFrame->surfaceColor, ratio);
            Vector3f emissionColor = Vector3f::lerp(lastKeyFrame->emissionColor, nextKeyFrame->emissionColor, ratio);
            float radius = lerpFloat(lastKeyFrame->radius, nextKeyFrame->radius, ratio);
            float reflection = lerpFloat(lastKeyFrame->reflection, nextKeyFrame->reflection, ratio);
            float transparency = lerpFloat(lastKeyFrame->transparency, nextKeyFrame->transparency, ratio);

            return Sphere(frameNum, centre, radius, surfaceColor, reflection, transparency, emissionColor);
        }
    }
    return Sphere(0,Vector3f(0.0f, 0.0f, 0.0f), 1.0f, Vector3f(1.0f, 0.0f, 0.0f));
}
