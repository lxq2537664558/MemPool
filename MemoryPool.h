#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include "MemChunk.h"
/** @ MemoryPool.h
 * ����ʵ���ڴ��
 * ���ù̶���С���Խ����ڴ���������
 * ���������С�ڴ���䵼�µ��ڴ���Ƭ����
 */
struct HeapHeader
{
	size_t size;
};
struct MemoryHeap
{
	HeapHeader header;
	char pBuffer;
};

class MemoryPool
{
public:
	typedef enum{MAX_SIZE=1024,MIN_SIZE=sizeof(MemoryChunk*)};
	MemoryPool()
	{
		//��������ĸ���
		chunkcount=0;
		for(size_t size=MIN_SIZE; size<=MAX_SIZE; size*=2)
			++chunkcount;

		pChunkList=new MemoryChunk*[chunkcount];

		//ÿ����ڴ��С��2�Ĵη�������4 8 16....
		int index=0;
		for(size_t size=MIN_SIZE; size<=MAX_SIZE; size*=2)
		{
			pChunkList[index++]=new MemoryChunk(size,1000);
		}
	}

	~MemoryPool()
	{
		for(int index=0; index<chunkcount; ++index)
		{
			delete pChunkList[index];
		}
		delete[] pChunkList;
	}

	void* Malloc(size_t size)
	{
		//������������飬����������ڴ�
		if(size>MAX_SIZE)
		{
			return malloc(size);
		}
		//���Ҷ�Ӧ�����ڴ��С����������ڴ��С�Ŀ�
		int index=0;
		for(size_t tsize=MIN_SIZE; tsize<=MAX_SIZE; tsize*=2)
		{
			if(tsize>=size)break;
			++index;
		}
		//���ض�Ӧ�ڴ��
		return pChunkList[index]->malloc();
	}

	//�ͷ��ڴ��
	void Free(void* pMem)
	{
		if(!free(pMem))MemoryChunk::free(pMem);
	}
protected:
	void* malloc(size_t size)
	{
		//��������ڴ棬������buffer��ַ����Ϊ����ʹ�õ��ڴ�鿪ʼ��ַ
		MemoryHeap* pHeap=(MemoryHeap*)::malloc(sizeof(HeapHeader)+size);
		if(pHeap)
		{
			pHeap->header.size=size;
			return &pHeap->pBuffer;
		}
		return NULL;
	}

	bool free(void* pMem)
	{
		//�ͷŶ��������ڴ��ַ�����е�ַƫ�ƣ��ҵ�ͷ����ʼ��ַ����ͷ��һͬ�ͷ�
		MemoryHeap* pHeap=(MemoryHeap*)((char*)pMem-sizeof(HeapHeader));
		if(pHeap->header.size>MAX_SIZE)
		{
			::free(pHeap);
			return true;
		}
		return false;
	}
private:
	MemoryChunk** pChunkList;//�ڴ�ؿ�����
	int chunkcount;//�ڴ�ؿ�����
};
#endif //MEMORYPOOL_H