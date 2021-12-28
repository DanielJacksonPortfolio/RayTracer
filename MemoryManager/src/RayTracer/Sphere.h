#pragma once
#include "../MemoryManagement/ManagedObject.h"
#include "../GlobalDefines.h"
#include "Vector3.h"

class Sphere : public ManagedObject
{
public:
	Vector3f center;							/// position of the sphere
	float radius, radius2;						/// sphere radius and radius^2
	Vector3f surfaceColor, emissionColor;		/// surface color and emission (light)
	float transparency, reflection;				/// surface transparency and reflectivity
	int frameNum;
	Sphere();
	Sphere(const int frameNum, const Vector3f& center, const float& radius, const Vector3f& surfaceColor, const float& reflection = 0, const float& transparency = 0, const Vector3f& emissionColor = 0);
	~Sphere();

	bool Intersect(const Vector3f& rayorig, const Vector3f& raydir, float& t0, float& t1) const;

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	bool operator <(const Sphere& sphere) const
	{
		if (radius != sphere.radius)
		{
			return radius < sphere.radius;
		}
		return false;
	}
	bool operator ==(const Sphere& sphere) const
	{
		return (
			center == sphere.center &&
			radius == sphere.radius &&
			surfaceColor == sphere.surfaceColor &&
			emissionColor == sphere.emissionColor &&
			transparency == sphere.transparency &&
			reflection == sphere.reflection &&
			frameNum == sphere.frameNum);
	}

	bool operator !=(const Sphere& sphere) const
	{
		return !(
			center == sphere.center &&
			radius == sphere.radius &&
			surfaceColor == sphere.surfaceColor &&
			emissionColor == sphere.emissionColor &&
			transparency == sphere.transparency &&
			reflection == sphere.reflection &&
			frameNum == sphere.frameNum);
	}
};
namespace std 
{

	template <>
	struct hash<Vector3f>
	{
		std::size_t operator()(const Vector3f& vec3f) const
		{
			using std::size_t;
			using std::hash;

			return ((hash<float>()(vec3f.x)
				^ (hash<float>()(vec3f.y) << 1)) >> 1)
				^ (hash<float>()(vec3f.z) << 1);
		}
	};

	template <>
	struct hash<Sphere>
	{
		std::size_t operator()(const Sphere& sphere) const
		{
			using std::size_t;
			using std::hash;


			Vector3f center;							/// position of the sphere
			float radius, radius2;						/// sphere radius and radius^2
			Vector3f surfaceColor, emissionColor;		/// surface color and emission (light)
			float transparency, reflection;				/// surface transparency and reflectivity
			int frameNum;
			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((((((((((hash<Vector3f>()(sphere.center)
				^ (hash<float>()(sphere.radius) << 1)) >> 1)
				^ (hash<float>()(sphere.transparency) << 1)) >> 1)
				^ (hash<float>()(sphere.reflection) << 1)) >> 1)
				^ (hash<int>()(sphere.frameNum) << 1)) >> 1)
				^ (hash<Vector3f>()(sphere.surfaceColor) << 1)) >> 1)
				^ (hash<Vector3f>()(sphere.emissionColor) << 1);
		}
	};
}