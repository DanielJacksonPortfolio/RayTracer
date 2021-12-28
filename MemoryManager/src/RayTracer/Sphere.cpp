#include "Sphere.h"

Sphere::Sphere()
{
	center = Vector3f(0);
	radius = 0.0f;
	radius2 = 0.0f;
	surfaceColor = Vector3f(0);
	emissionColor = Vector3f(0);
	transparency = 1.0f;
	reflection = 1.0f;
	frameNum = -1;
}

Sphere::Sphere(const int frameNum, const Vector3f& center, const float& radius, const Vector3f& surfaceColor, const float& reflection, const float& transparency, const Vector3f& emissionColor) :
    frameNum(frameNum), center(center), radius(radius), radius2(radius* radius), surfaceColor(surfaceColor), emissionColor(emissionColor),
    transparency(transparency), reflection(reflection)
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} Constructor : " << this-sizeof(Header) << std::endl;)
}

void* Sphere::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Shape"));
}
void Sphere::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* Sphere::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Shape"));
}
void Sphere::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

Sphere::~Sphere()
{
    VERBOSE_OUTPUT(std::cout << "{Sphere} Deconstructor" << std::endl;)
}

//[comment]
// Compute a ray-sphere intersection using the geometric solution
//[/comment]
bool Sphere::Intersect(const Vector3f& rayorig, const Vector3f& raydir, float& t0, float& t1) const
{
	Vector3f l = center - rayorig;
	float tca = l.dot(raydir);
	if (tca < 0)
	{
		return false;
	}

	float d2 = l.dot(l) - tca * tca;
	if (d2 > radius2)
	{
		return false;
	}

	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	return true;
}