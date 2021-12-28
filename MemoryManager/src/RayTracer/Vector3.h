#pragma once
#include "../MemoryManagement/ManagedObject.h"
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>

#include "../GlobalDefines.h"
#include "../MemoryManagement/HeapManager.h"

template<typename T>
class Vector3 : public ManagedObject
{
public:
	T x, y, z;
	Vector3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vector3(T xx) : x(xx), y(xx), z(xx) {}
	Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vector3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vector3<T> operator * (const T& f) const { return Vector3<T>(x * f, y * f, z * f); }
	Vector3<T> operator * (const Vector3<T>& v) const { return Vector3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vector3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
	Vector3<T> operator - (const Vector3<T>& v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
	Vector3<T> operator + (const Vector3<T>& v) const { return Vector3<T>(x + v.x, y + v.y, z + v.z); }
	Vector3<T>& operator += (const Vector3<T>& v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vector3<T>& operator *= (const Vector3<T>& v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vector3<T> operator - () const { return Vector3<T>(-x, -y, -z); }

	bool operator == (const Vector3<T>& v) const { return v.x == x && v.y == y && v.z == z; }
	bool operator < (const Vector3<T>& v) const { return length2() < v.length2(); }

	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	static Vector3<T> lerp(const Vector3<T>& vec1, const Vector3<T>& vec2, float ratio)
	{
		Vector3<T> diff = vec2 - vec1;
		return vec1 + (diff * ratio);
	}
	friend std::ostream& operator << (std::ostream& os, const Vector3<T>& v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}

	static void* operator new(size_t size)
	{
		VERBOSE_OUTPUT(std::cout << "{Vector3<" << typeid(T).name() << ">} New Called" << std::endl;);
		return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Vector"));
	}

	static void operator delete(void* pMem)
	{
		VERBOSE_OUTPUT(std::cout << "{Vector3<" << typeid(T).name() << ">} Delete Called" << std::endl;);
		::operator delete(pMem);
	}

	static void* operator new[](size_t size) 
	{
		VERBOSE_OUTPUT(std::cout << "{Vector3<" << typeid(T).name() << ">} New[] Called" << std::endl;);
		return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Vector"));
	}

	static void operator delete[](void* pMem)
	{
		VERBOSE_OUTPUT(std::cout << "{Vector3<" << typeid(T).name() << ">} Delete[] Called" << std::endl;);
		::operator delete[](pMem);
	}
};

