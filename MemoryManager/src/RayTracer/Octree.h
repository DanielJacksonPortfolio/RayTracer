#pragma once
#include "../MemoryManagement/ManagedObject.h"
#include "../GlobalDefines.h"
#include "../MemoryManagement/HeapManager.h"
#include "Sphere.h"

#include <vector>
#include <unordered_set>

class OctreeNode : public ManagedObject
{
public:
	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);
	void SetBounds(Vector3f dimensions, Vector3f position);
	void Subdivide(int layer);
	bool SetChild(const Sphere& sphere);
	void RayIntersect(std::unordered_set<Sphere>& possibleSpheres, Vector3f rayOrigin, Vector3f rayDirection, Vector3f invDir);
	bool AABBSphereIntersection(const Sphere& sphere);

	~OctreeNode();
private:
	Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f dimensions = Vector3f(0.0f, 0.0f, 0.0f);
	std::vector<Sphere> containedSpheres;
	std::vector<OctreeNode*> childCells;
};

class Octree : public ManagedObject
{
public:
	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	static Octree* ConstructTree(Vector3f dimensions, Vector3f position, std::unordered_set<Sphere>& spheres);

	~Octree();

	std::unordered_set<Sphere> Intersect(Vector3f rayOrigin, Vector3f rayDirection);

private:
	OctreeNode* rootNode;
	std::unordered_set<Sphere> externalSpheres;
};

