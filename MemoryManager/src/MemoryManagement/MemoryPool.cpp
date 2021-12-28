#include "MemoryPool.h"
#include <iostream>
#include "../GlobalDefines.h"
#include "HeapManager.h"
#include <iostream>

void* MemoryPool::Allocate(size_t size)
{
    if (nextAllocPtr == nullptr) 
    {
        nextAllocPtr = AllocateBlock(size);
    }
    Chunk* freeChunk = nextAllocPtr;
    nextAllocPtr = nextAllocPtr->nextChunk;

    return freeChunk;
}

void MemoryPool::Deallocate(void* ptr, size_t size)
{
    reinterpret_cast<Chunk*>(ptr)->nextChunk = nextAllocPtr;

    nextAllocPtr = reinterpret_cast<Chunk*>(ptr);
}

void* MemoryPool::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{MemoryPool} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::GetDefaultHeap(), false);
}
void MemoryPool::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{MemoryPool} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* MemoryPool::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{MemoryPool} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::GetDefaultHeap(), false);
}
void MemoryPool::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{MemoryPool} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}


Chunk* MemoryPool::AllocateBlock(size_t chunkSize)
{
    VERBOSE_OUTPUT(std::cout << std::endl << "Allocating block (" << chunksPerBlock << " chunks):" << std::endl;);

    size_t blockSize = chunksPerBlock * chunkSize;

    Chunk* blockBegin = reinterpret_cast<Chunk*>(malloc(blockSize));

    Chunk* chunk = blockBegin;
    for (int i = 0; i < chunksPerBlock - 1; ++i) 
    {
        chunk->nextChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
        chunk = chunk->nextChunk;
    }
    chunk->nextChunk = nullptr;

    return blockBegin;
}
