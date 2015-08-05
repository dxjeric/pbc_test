//-------------------------------------------------------------------------------------------------
//	Created:	2015-7-7   14:57
//	File Name:	Common.h
//	Author:		Eric(沙鹰)
//	PS:			如果发现说明错误，代码风格错误，逻辑错问题，设计问题，请告诉我。谢谢！
//  Email:		frederick.dang@gmail.com
//	Purpose:	Common
//-------------------------------------------------------------------------------------------------
#pragma once
#include "System.h"

// log 相关
#define PrintLogInfoLen 8192
#define LogPrintFlush(hFile) fflush(hFile)
#define OverLappedBufferLen 10240
void LogPrint(const char* strFormat, ...);
#define Log(strFormat, ...) {LogPrint("%s - %s - %d: "##strFormat, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);}

inline void LogPrint(const char* strFormat, ...)
{
	char strLog[PrintLogInfoLen];
	va_list vlArgs;
	va_start(vlArgs, strFormat);
	int offset = vsnprintf_s(strLog, PrintLogInfoLen, strFormat, vlArgs);
	va_end(vlArgs);
	printf("%s\n", strLog);
	LogPrintFlush(stdout);
}

#ifdef _WIN32
	typedef CRITICAL_SECTION HLOCK;
#else
	typedef pthread_mutex_t  HLOCK;
#endif



class SafeLock
{
public:
	SafeLock();
	~SafeLock();

public:
	void Lock();
	void UnLock();
private:
	HLOCK	m_hLockHandle;
};

inline SafeLock::SafeLock()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_hLockHandle);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&m_hLockHandle, &attr);
#endif
}

inline SafeLock::~SafeLock()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_hLockHandle);
#else
	pthred_mutex_destroy(&m_hLockHandle);
#endif
}

inline void SafeLock::Lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_hLockHandle);
#else
	pthread_mutex_lock(&m_hLockHandle);
#endif
}

inline void SafeLock::UnLock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_hLockHandle);
#else
	pthread_mutex_unlock(&m_hLockHandle);
#endif
}


// 消息队列
struct NetMsgInfo  {
	void* msg;
	size_t len;
	SOCKET Conn;
};
typedef std::list<NetMsgInfo*> ListMsg;
typedef ListMsg::iterator ListMsgItor;

struct NetMsg
{
	SafeLock lock;
	ListMsg  msg;

public:
	NetMsg() {};
	~NetMsg() {};
public:
	void AddNewMsg(void* pMsg, size_t iLen, SOCKET Conn)
	{
		lock.Lock();
		NetMsgInfo* pInfo = new NetMsgInfo;
		pInfo->msg = new char[iLen];
		memcpy(pInfo->msg, pMsg, iLen);
		pInfo->len = iLen;
		pInfo->Conn	= Conn;
		msg.push_back(pInfo);

		LogPrint("AddNewMsg[0x%08x]: msg:[0x%08x] len:[%d].", this, pInfo->msg, iLen);
		lock.UnLock();
	}

	void* GetMsg(size_t& iLen, SOCKET& Conn)
	{
		lock.Lock();
		// TODO: 相关代码ID:201508042055
		// LogPrint("GetMsg[0x%08x] Size = [%d].", this, msg.size());
		if (msg.size() > 0)
		{
			NetMsgInfo* pReturn = msg.front();
			iLen = pReturn->len;
			Conn = pReturn->Conn;
			LogPrint("GetMsg[0x%08x]: msg:[0x%08x] len:[%d].", this, pReturn->msg, iLen);
			msg.pop_front();
			lock.UnLock();			
			return pReturn->msg;
		}
		else
		{
			lock.UnLock();
			return NULL;
		}
	}

	int GetSize()
	{
		lock.Lock();
		int iSize = msg.size();
		lock.UnLock();
		return iSize;
	}
};

typedef struct OverLapped
{
public:
	typedef enum OverLappedOperatorType
	{
		EOLOT_Accept = 0,
		EOLOT_Send,
		EOLOT_Recv,
	} OLOpType;

public:
	WSAOVERLAPPED	sysOverLapped;
	WSABUF			sysBuffer;
	char			dataBuffer[OverLappedBufferLen];
	OLOpType		opType;

public:
	OverLapped();
} OverLapped, *OverLappedPtr;