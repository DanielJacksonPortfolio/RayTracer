#pragma once
#pragma once

#include "../MemoryManagement/ManagedObject.h"
#include "SphereAnimation.h"

class Animation : public ManagedObject
{
public:
	Animation();
	~Animation();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	std::vector<SphereAnimation*>* spheres;

	int GetLastFrame();

	void AddNewSphereAnimation(SphereAnimation* keyframe);
	std::vector<SphereAnimation*>* GetSpheres();
};

