#include "HeapManager.h"

#include "DefaultHeap.h"
#include "../GlobalDefines.h"

HeapManager* HeapManager::instance = nullptr;
DefaultHeap* HeapManager::defaultHeap = new DefaultHeap("DEFAULT");
Heap* HeapManager::CurrentHeapScope = nullptr;
std::stack<Heap*>* HeapManager::HeapStack = new std::stack<Heap*>();

void HeapManager::BeginHeapScope(Heap* heap)
{
    if (HeapStack != nullptr)
    {
        CurrentHeapScope = defaultHeap;
        HeapStack->push(heap);
        CurrentHeapScope = HeapStack->top();
    }
}

void HeapManager::EndHeapScope()
{
    if (HeapStack != nullptr)
    {
        HeapStack->pop();
        if (HeapStack->empty())
        {
            CurrentHeapScope = nullptr;
        }
        else
        {
            CurrentHeapScope = HeapStack->top();
        }
    }
}

HeapManager* HeapManager::Instance()
{
    if (instance == nullptr)
    {
        instance = new HeapManager();
    }
    return instance;
}

void* HeapManager::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} New Called" << std::endl;)
    return ManagedObject::operator new(size, defaultHeap);
}
void HeapManager::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} Delete Called" << std::endl;)
    ::operator delete(pMem);
}
void* HeapManager::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} New[] Called" << std::endl;)
    return ManagedObject::operator new[](size, defaultHeap);
}
void HeapManager::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} Delete[] Called" << std::endl;)
    ::operator delete[](pMem);
}

Heap* HeapManager::GetDefaultHeap()
{
    return dynamic_cast<Heap*>(defaultHeap);
}

Heap* HeapManager::GetHeap(const char* heapName)
{
    if (heaps.find(heapName) == heaps.end())
    {
        BeginHeapScope(GetDefaultHeap());
        heaps[heapName] = new Heap(heapName);
        EndHeapScope();
    }
    return heaps[heapName];
}

HeapManager::HeapManager()
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} Constructor : " << this-sizeof(Header) << std::endl;)
    heaps = std::unordered_map<const char*, Heap*>();
}
HeapManager::~HeapManager()
{
    VERBOSE_OUTPUT(std::cout << "{HeapManager} Deconstructor" << std::endl;)
    for (auto heap : heaps)
    {
        delete heap.second;
    }
    delete HeapStack;
}