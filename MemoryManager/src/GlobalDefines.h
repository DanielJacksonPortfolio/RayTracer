#pragma once
#include <mutex>

#define USE_THREADING false
#define USE_OCTREE false

#define MAX_OCTREE_LAYERS 5

#ifdef WIN32
#define ROOT_DIRECTORY "."
#else
#define ROOT_DIRECTORY std::string(getenv("HOME")) + "/Documents/Repos/RayTracer/MemoryManager"
#endif

static bool VerboseOutput = false;

#define VERBOSE_OUTPUT(cout) if(VerboseOutput) {cout}

typedef unsigned char byte;

template<typename T>
class Vector3;

typedef Vector3<float> Vector3f;

#if defined __linux__ || defined __APPLE__
	// "Compiled for Linux
#else
	// Windows doesn't define these values by default, Linux does
	#define M_PI 3.141592653589793f
	#ifndef INFINITY
		#define INFINITY 1e8
	#endif
#endif

//[comment]
// This variable controls the maximum recursion depth
//[/comment]
#define MAX_RAY_DEPTH 5