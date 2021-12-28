#pragma once
#include "GlobalDefines.h"
#include "../src/MemoryManagement/ManagedObject.h"

#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>

//#define MAX_THREADS 1
#define MAX_THREADS std::thread::hardware_concurrency() * 10

class Animation;
class Sphere;

class ThreadManager : public ManagedObject
{
public:
	ThreadManager();
	~ThreadManager();

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

	void TimedOutput(int num);
	void ThreadStart();

	int StartNewRenderFrameThread(int threadGroup, void (*lambda)(Animation*, unsigned int, unsigned int, int, std::string),
		Animation* animation, unsigned int width, unsigned int height, int currentFrame, std::string filepath);
	int StartNewRenderRowThread(int threadGroup, void (*lambda)(Vector3f* row, int y, const std::vector<Sphere>& spheres, unsigned int width, float invWidth, float invHeight, float angle, float angleXAspect),
		Vector3f* row, int y, const std::vector<Sphere>& spheres, unsigned int width, float invWidth, float invHeight, float angle, float angleXAspect);

	void JoinSet(std::vector<int> set);
	void JoinAll();
	void JoinGroup(int group);

private:
	std::mutex managerMutex;

	std::thread* thread1;
	std::thread* thread2;

	std::thread* checkThread;

	bool closing = false;

	std::unordered_map<int,std::thread*> threads = std::unordered_map<int, std::thread*>();
	std::unordered_map<int, bool> activeThreads = std::unordered_map<int, bool>();
	std::unordered_map<int, int> threadIDGroupMap = std::unordered_map<int, int>();
	std::vector<int> availableThreads = std::vector<int>();
	std::vector<int> doneThreads = std::vector<int>();

	void CheckForDone();
};			   

