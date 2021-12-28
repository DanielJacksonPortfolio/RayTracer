#include "ThreadManager.h"
#include "GlobalDefines.h"
#include "MemoryManagement\Heap.h"
#include "MemoryManagement\HeapManager.h"
#include "RayTracer/RayTracer.h"
#include "RayTracer/Sphere.h"
#include "RayTracer/Vector3.h"

#include <chrono>
#include <thread>

ThreadManager::ThreadManager()
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} Constructor Called : " << this << std::endl;);

    for (int i = 0; i < MAX_THREADS; ++i)
    {
        availableThreads.push_back(i);
    }

    checkThread = new std::thread(&ThreadManager::CheckForDone, this);
}

ThreadManager::~ThreadManager()
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} Deconstructor Called" << std::endl;);
    delete thread1;
    delete thread2;

    managerMutex.lock();
    closing = true;

    checkThread->join();
    delete checkThread;

    managerMutex.unlock();
    for (int i = 0; i < threads.size(); ++i)
    {
        if (threads[i] != nullptr)
        {
            if (threads[i]->joinable())
            {
                threads[i]->join();
            }
            delete threads[i];
        }
    }
}

void* ThreadManager::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::GetDefaultHeap());
}
void ThreadManager::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* ThreadManager::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::GetDefaultHeap());
}
void ThreadManager::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{ThreadManager} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

void ThreadManager::TimedOutput(int num)
{
    int numIterations = 10;
    for (int i = 0; i < numIterations; ++i)
    {
        managerMutex.lock();
        std::cout << "TimedOutput: " << num << " : " << i << std::endl;
        managerMutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ThreadManager::ThreadStart()
{
    thread1 = new std::thread(&ThreadManager::TimedOutput, this, 111);
    thread2 = new std::thread(&ThreadManager::TimedOutput, this, 123);
    std::cout << "Post-New ThreadingDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("ThreadingDefault")->GetNumberOfBytesAllocated() << std::endl;
    thread1->join();
    thread2->join();
}

int ThreadManager::StartNewRenderFrameThread(int threadGroup, void (*lambda)(Animation*, unsigned int, unsigned int, int, std::string),
    Animation* animation, unsigned int width, unsigned int height, int currentFrame, std::string filepath)
{
    int threadID = -1;
    while (availableThreads.size() < 1)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    threadID = availableThreads[0];

    threadIDGroupMap[threadID] = threadGroup;
    activeThreads[threadID] = true;
    threads[threadID] = new std::thread([this, &lambda](Animation* animation, unsigned int width, unsigned int height, int currentFrame, std::string filepath,int threadID)
        {
            lambda(animation, width, height, currentFrame, filepath);
            activeThreads[threadID] = false;
            threadIDGroupMap[threadID] = -1; 
            doneThreads.push_back(threadID);
        }, animation, width, height, currentFrame, filepath,threadID); 
    availableThreads.erase(availableThreads.begin());

    return threadID;
}

int ThreadManager::StartNewRenderRowThread(int threadGroup, void (*lambda)(Vector3f* row, int y, const std::vector<Sphere>& spheres, unsigned int width, float invWidth, float invHeight, float angle, float angleXAspect),
    Vector3f* row, int y, const std::vector<Sphere>& spheres, unsigned int width, float invWidth, float invHeight, float angle, float angleXAspect)
{
    int threadID = -1;
    while (availableThreads.size() < 1)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    threadID = availableThreads[0];

    activeThreads[threadID] = true;
    threads[threadID] = new std::thread([this, &lambda](int threadID, Vector3f* row, int y, const std::vector<Sphere>& spheres, unsigned int width, float invWidth, float invHeight, float angle, float angleXAspect)
        {
            lambda(row, y, spheres, width, invWidth, invHeight, angle, angleXAspect);
            activeThreads[threadID] = false;
            doneThreads.push_back(threadID);
        }, threadID, row, y, spheres, width, invWidth, invHeight, angle, angleXAspect);
    availableThreads.erase(availableThreads.begin());

    return threadID;
}

void ThreadManager::JoinSet(std::vector<int> set)
{
    managerMutex.lock();
    for (auto threadID : set)
    {
        if (activeThreads[threadID])
        {
            threads[threadID]->join();
            delete threads[threadID];
            threads[threadID] = nullptr;
            availableThreads.push_back(threadID);
            //Remove From Done
        }
    }
    managerMutex.unlock();
}

void ThreadManager::JoinAll()
{
    managerMutex.lock();
    for (auto threadIsActive : activeThreads)
    {
        if (threadIsActive.second)
        {
            threads[threadIsActive.first]->join();
            delete threads[threadIsActive.first];
            threads[threadIsActive.first] = nullptr;
            availableThreads.push_back(threadIsActive.first);
        }
    }
    doneThreads.clear();
    managerMutex.unlock();
}

void ThreadManager::JoinGroup(int group)
{
    managerMutex.lock();
    for (auto groupedThread : threadIDGroupMap)
    {
        if (groupedThread.second == group)
        {
            if (activeThreads[groupedThread.first])
            {
                threads[groupedThread.first]->join();
                delete threads[groupedThread.first];
                threads[groupedThread.first] = nullptr;
                threadIDGroupMap[groupedThread.first] = -1;
                availableThreads.push_back(groupedThread.first);
                //Remove From Done
            }
        }
    }
    managerMutex.unlock();
}

void ThreadManager::CheckForDone()
{
    while (!closing)
    {
        if (doneThreads.size() > 0)
        {
            managerMutex.lock();
            for (int i = 0; i < doneThreads.size(); ++i)
            {
                if (threads[doneThreads[i]] != nullptr)
                {
                    threads[doneThreads[i]]->join();
                }
                delete threads[doneThreads[i]];
                threads[doneThreads[i]] = nullptr;
                availableThreads.push_back(doneThreads[i]);
            }
            doneThreads.clear();
            managerMutex.unlock();
        }
    }
}
