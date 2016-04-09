#pragma once

//尝试自己开发一个内存池，用于内存管理。
//在高性能服务器中，内存的管理和分配也是导致系统成败的重要因素。因此我决定下经历研究它。
//2年前，我曾经写出过一个内存池。1年前，我重构了一个机遇IOCP服务器的内存管理池。
//今天，我想做到一个通用的，可以嵌入我的新系统中的内存管理体系。
//加油吧，freeeyes，你能做到。

#include <stdio.h>
#include <stdlib.h>
#include "ThreadLock.h"

#define MAX_MEMORYHEAD_SIZE 12         //4个字节的指针块地址 + 4个字节的链表首地址 + 4位验证码
#define MAGIC_CODE          0x123456   //验证码

#define UINT32 unsigned int

struct _MemoryBlock  //内存块的结构,双向链表
{
	_MemoryBlock* m_pNext;
	_MemoryBlock* m_pPrev;
	void*         m_pBrick;

	void Init()
	{
		m_pNext   = NULL;
		m_pPrev   = NULL;
		m_pBrick  = NULL;
	};

	_MemoryBlock()
	{
		Init();
	};
};

struct _MemoryList    //内存管理列表
{
	_MemoryList*  m_pMemLNext;         
	_MemoryBlock* m_pMemoryFree;       //自由的内存块
	_MemoryBlock* m_pMemoryFreeLast;   //自由的内存块链表末尾
	_MemoryBlock* m_pMemoryUsed;       //使用的内存块
	_MemoryBlock* m_pMemoryUsedLast;   //使用的内存块链表末尾

	int m_nSize;

	void Init()
	{
		m_pMemLNext       = NULL;
		m_pMemoryFree     = NULL;
		m_pMemoryUsed     = NULL;
		m_pMemoryUsedLast = NULL;
		m_pMemoryFreeLast = NULL;
		m_nSize           = 0;
	};

	_MemoryList()
	{
		Init();
	};
};

class CMemoryPools
{
public:
	static CMemoryPools& Instance()
	{
		if(m_pMemoryPools == NULL)
		{
			m_pMemoryPools = (CMemoryPools* )malloc(sizeof(CMemoryPools));
			m_pMemoryPools->Init();
		}

		return *m_pMemoryPools;
	}

public:
	~CMemoryPools(void);

	void* GetBuff(size_t szBuffSize);
	bool DelBuff(size_t szBuffSize, void* pBuff);
	bool DelBuff(void* pBuff);
	void DisplayMemoryList();

private:
	CMemoryPools(void);
	void Close();
	void Init();
	void* SetMemoryHead(void* pBuff, _MemoryList* pList, _MemoryBlock* pBlock);
	void* GetMemoryHead(void* pBuff);
	bool GetHeadMemoryBlock(void* pBuff, _MemoryList*& pList, _MemoryBlock*& pBlock);
	

private:
	static CMemoryPools* m_pMemoryPools;
	_MemoryList*         m_pMemoryList;
	_MemoryList*         m_pMemoryListLast;    //最后一个内存管理链表指针
	CThreadLock          m_ThreadLock;
};
