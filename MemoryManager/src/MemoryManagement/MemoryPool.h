#pragma once
#include "ManagedObject.h"

struct Chunk 
{
	Chunk* nextChunk;
};

class MemoryPool : public ManagedObject
{
public:
	MemoryPool() : chunksPerBlock(1) {}
	MemoryPool(size_t chunksPerBlock) : chunksPerBlock(chunksPerBlock) {}

	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

	static void* operator new(size_t size);
	static void operator delete(void* pMem);
	static void* operator new[](size_t size);
	static void operator delete[](void* pMem);

private:
	size_t chunksPerBlock;

	Chunk* nextAllocPtr = nullptr;

	Chunk* AllocateBlock(size_t chunkSize);
};

