// Windows only

#include "MemoryManagement/Test/SoundObject.h"
#include "MemoryManagement/Test/GraphicsObject.h"
#include "MemoryManagement/Heap.h"
#include "MemoryManagement/PoolManager.h"

#include "RayTracer/RayTracer.h"
#include "ThreadManager.h"

#include <chrono>

void RayTracerTest()
{
    HeapManager::BeginHeapScope(HeapManager::Instance()->GetHeap("RayTracerDefault"));

    std::cout << std::endl << "Ray Tracer Test" << std::endl << "---------------------------------" << std::endl << std::endl;

    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Initial Ray Tracer Default Heap Num Bytes : " << HeapManager::Instance()->GetHeap("RayTracerDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Vector Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Vector")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Shape Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Shape")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Animation Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Shape")->GetNumberOfBytesAllocated() << std::endl;

    RayTracer* rayTracer = new RayTracer();

    // This sample only allows one choice per program execution. Feel free to improve upon this
    srand(13);

    rayTracer->AnimatedRender(ROOT_DIRECTORY + "/Scenes/octreeTest.xml");
    //rayTracer->AnimatedRender(ROOT_DIRECTORY + "/Scenes/spheres.xml");
    //rayTracer->AnimatedRender(ROOT_DIRECTORY + "/Scenes/sphereAnim.xml");
    //rayTracer->AnimatedRender(ROOT_DIRECTORY + "/Scenes/sphereAnim2.xml");
    //rayTracer->AnimatedRender(ROOT_DIRECTORY + "/Scenes/sphereAnim3.xml");

    delete rayTracer;

    VERBOSE_OUTPUT(std::cout << std::endl;);
    HeapManager::Instance()->GetDefaultHeap()->Walk();
    HeapManager::Instance()->GetHeap("RayTracerDefault")->Walk();
    HeapManager::Instance()->GetHeap("Shape")->Walk();
    HeapManager::Instance()->GetHeap("Vector")->Walk();
    HeapManager::Instance()->GetHeap("Animation")->Walk();


    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Final Ray Tracer Default Heap Num Bytes: " << HeapManager::Instance()->GetHeap("RayTracerDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Vector Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Vector")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Shape Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Shape")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Animation Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Animation")->GetNumberOfBytesAllocated() << std::endl;

    HeapManager::EndHeapScope();
}

void MemoryManagerTest()
{
    HeapManager::BeginHeapScope(HeapManager::Instance()->GetHeap("MemoryManagerDefault"));

    std::cout << std::endl << "Memory Manager Test" << std::endl << "---------------------------------" << std::endl << std::endl;

    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Initial Memory Manager Default Heap Num Bytes : " << HeapManager::Instance()->GetHeap("MemoryManagerDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Graphics Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Graphics")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Sound Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl << std::endl;

    std::cout << "Create Objects" << std::endl << std::endl;

    int* intVal = new int;
    VERBOSE_OUTPUT(std::cout << std::endl;);
    int* intValList = new int[10];
    VERBOSE_OUTPUT(std::cout << std::endl;);
    MyObject* object = new MyObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    MyObject* objectList = new MyObject[10];
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObject = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObjectList = new SoundObject[10];
    VERBOSE_OUTPUT(std::cout << std::endl;);
    GraphicsObject* gObject = new GraphicsObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::vector<GraphicsObject*>* gObjectVector = new std::vector<GraphicsObject*>();

    for (int i = 0; i < 10; ++i)
    {
        gObjectVector->push_back(new GraphicsObject());
    }

    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Post-New Default Heap Num Bytes: " << HeapManager::GetDefaultHeap()->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Post-New MemoryManagerDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("MemoryManagerDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Post-New Graphics Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Graphics")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Post-New Sound Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl << std::endl;

    //HeapManager::Instance()->GetHeap("MemoryManagerDefault")->Walk();

    std::cout << "Delete IntValList" << std::endl << std::endl;

    delete[] intValList;

    std::cout << "Post-Delete Default Heap Num Bytes: " << HeapManager::GetDefaultHeap()->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Post-Delete MemoryManagerDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("MemoryManagerDefault")->GetNumberOfBytesAllocated() << std::endl;

    ////VERBOSE_OUTPUT(std::cout << std::endl;);
    //HeapManager::GetDefaultHeap()->Walk();
    //HeapManager::Instance()->GetHeap("MemoryManagerDefault")->Walk();
    //HeapManager::Instance()->GetHeap("Sound")->Walk();
    //HeapManager::Instance()->GetHeap("Graphics")->Walk();

    std::cout << "Delete Objects" << std::endl << std::endl;

    delete intVal;
    delete object;
    delete[] objectList;
    delete sObject;
    delete[] sObjectList;
    delete gObject;
    for (int i = 0; i < 10; ++i)
    {
        delete (*gObjectVector)[i];
    }
    delete gObjectVector;

    //VERBOSE_OUTPUT(std::cout << std::endl;);
    //HeapManager::GetDefaultHeap()->Walk();
    //HeapManager::Instance()->GetHeap("MemoryManagerDefault")->Walk();
    //HeapManager::Instance()->GetHeap("Sound")->Walk();
    //HeapManager::Instance()->GetHeap("Graphics")->Walk();

    std::cout << "Final MemoryManagerDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("MemoryManagerDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Graphics Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Graphics")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Sound Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl << std::endl;

    HeapManager::EndHeapScope();
}


void MemoryPoolTest()
{
    HeapManager::BeginHeapScope(HeapManager::Instance()->GetHeap("MemoryPoolDefault"));

    std::cout << std::endl << "Memory Pool Test" << std::endl << "---------------------------------" << std::endl << std::endl;

    std::cout << "Initial Memory Pool Default Heap Num Bytes : " << HeapManager::Instance()->GetHeap("MemoryPoolDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Initial Sound Heap Num Bytes : " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl;

    std::cout << std::endl << "Create SObject 1-5" << std::endl << std::endl;
    SoundObject* sObject = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObject2 = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObject3 = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObject4 = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);
    SoundObject* sObject5 = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);

    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Post-New MemoryPoolDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("MemoryPoolDefault")->GetNumberOfBytesAllocated() << std::endl << std::endl;
    std::cout << "Post-New Sound Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl << std::endl;

    VERBOSE_OUTPUT(std::cout << std::endl;);
    HeapManager::Instance()->GetHeap("Sound")->Walk();

    std::cout << "Delete SObject 3" << std::endl << std::endl;
    delete sObject3; 

    HeapManager::Instance()->GetHeap("Sound")->Walk();

    std::cout << "Create SObject 6" << std::endl << std::endl;
    SoundObject* sObject6 = new SoundObject();
    VERBOSE_OUTPUT(std::cout << std::endl;);

    HeapManager::Instance()->GetHeap("Sound")->Walk();

    delete sObject;
    delete sObject2;
    delete sObject4;
    delete sObject5;
    delete sObject6;

    std::cout << "Delete SObject 1-6" << std::endl << std::endl;
    VERBOSE_OUTPUT(std::cout << std::endl;);
    HeapManager::Instance()->GetHeap("Sound")->Walk();

    std::cout << "Final MemoryPoolDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("MemoryPoolDefault")->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "Final Sound Heap Num Bytes: " << HeapManager::Instance()->GetHeap("Sound")->GetNumberOfBytesAllocated() << std::endl;

    HeapManager::EndHeapScope();
}

void ThreadingTest()
{
    HeapManager::BeginHeapScope(HeapManager::Instance()->GetHeap("ThreadingDefault"));

    std::cout << "Initial Threading Default Heap Num Bytes : " << HeapManager::Instance()->GetHeap("ThreadingDefault")->GetNumberOfBytesAllocated() << std::endl;

    ThreadManager* threadManager = new ThreadManager();
    auto start = std::chrono::steady_clock::now();

    //threadManager->TimedOutput(123);
    threadManager->ThreadStart();

    auto finish = std::chrono::steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
    std::cout << "Thread Test Complete, Duration: " << elapsed_seconds << " Seconds" << std::endl;

    delete threadManager;

    std::cout << "Final ThreadingDefault Heap Num Bytes: " << HeapManager::Instance()->GetHeap("ThreadingDefault")->GetNumberOfBytesAllocated() << std::endl;

    HeapManager::EndHeapScope();
}

int main()
{
    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Start Program" << std::endl;

    VERBOSE_OUTPUT(std::cout << std::endl;);
    std::cout << "Initial Default Heap Num Bytes : " << HeapManager::GetDefaultHeap()->GetNumberOfBytesAllocated() << std::endl;

    //MemoryManagerTest();
    //MemoryPoolTest();
    //ThreadingTest();
    RayTracerTest();

    delete HeapManager::Instance();
    delete PoolManager::Instance();
    std::cout << "Final Default Heap Num Bytes: " << HeapManager::GetDefaultHeap()->GetNumberOfBytesAllocated() << std::endl;
    std::cout << "End Program" << std::endl;
}