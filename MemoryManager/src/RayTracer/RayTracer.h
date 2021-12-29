#pragma once
#include "../MemoryManagement/ManagedObject.h"
#include "../MemoryManagement/HeapManager.h"
#include "../GlobalDefines.h"
#include "../ThreadManager.h"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>

class Sphere;
class Animation;

class RayTracer : public ManagedObject
{
public:
    static void* operator new(size_t size);
    static void operator delete(void* pMem);
    static void* operator new[](size_t size);
    static void operator delete[](void* pMem);

    RayTracer();
    ~RayTracer();
    Animation* LoadScene(std::string filepath);
    void AnimatedRender(std::string filepath, unsigned width = 640, unsigned height = 480);
private:
    static float Mix(const float& a, const float& b, const float& mix);
    static Vector3f Trace(const Vector3f& rayorig, const Vector3f& raydir, std::unordered_set<Sphere>& spheres, const int& depth);
    static void Render(std::unordered_set<Sphere>& spheres, int iteration, unsigned width = 640, unsigned height = 480, std::string filepath = "");
    void GenerateVideo(std::string filepath);
    static void RenderFrame(Animation* animation, unsigned width, unsigned height, int frameNum, std::string filepath);
    static std::mutex frameMutex;
    static std::mutex fileMutex;

    static ThreadManager* threadManager;

    std::string filepath = "";
};

