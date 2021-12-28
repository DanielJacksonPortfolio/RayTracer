#pragma once

#include "../MemoryManagement/ManagedObject.h"
#include "Sphere.h"

class SphereAnimation : public ManagedObject
{
public:
	SphereAnimation();
	~SphereAnimation();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	std::vector<Sphere*>* keyframes;

	int GetLastFrame();
	void AddNewKeyFrame(Sphere* keyframe);
	Sphere GetSphereAtFrame(int frameNum);
};

