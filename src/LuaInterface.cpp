//-------------------------------------------------------------------------------------------------
//	Created:	2015-7-29   21:14
//	File Name:	LuaInterface.cpp
//	Author:		Eric(沙鹰)
//	PS:			如果发现说明错误，代码风格错误，逻辑错问题，设计问题，请告诉我。谢谢！
//  Email:		frederick.dang@gmail.com
//	Purpose:	
//-------------------------------------------------------------------------------------------------
#include "LuaInterface.h"
#include "System.h"
#include "Common.h"

// error LNK2001: 无法解析的外部符号 "int __cdecl luaopen_protobuf_c(struct lua_State *)" (?luaopen_protobuf_c@@YAHPAUlua_State@@@Z)
// TODO: 猜测的原因是，C++如果要引用C的函数时，需要添加extern C
#ifdef __cplusplus
extern "C" {
#endif
	extern int luaopen_protobuf_c(lua_State *L);
#ifdef __cplusplus
}
#endif

// lua file config
#define LuaInitFileName			"./main.lua"
#define LuaInitFunctionName		"main_enter"
#define LuaErrorFunctionName	"main_error"

static int RegisterLuaNetworkFuns(lua_State* L);
static int RegisterLuaFuns(lua_State* L);

LuaInterface* LuaInterface::m_pLuaInterface = NULL;

LuaInterface::LuaInterface()
{
	m_pLuaState = NULL;
	m_pLuaInterface = this;
}

LuaInterface::~LuaInterface()
{
	m_pLuaState = NULL;
}


static int panic(lua_State* L)
{
	(void)L;	// Linux报错

	LogPrint("PANIC: unprotected error in call to Lua API(%s)", lua_tostring(L, -1));
	return 0;
}

void LuaInterface::LoadAllLuaLibs()
{
	// 打开lua默认的lib
	luaL_openlibs(m_pLuaState);
	// 注册pbc的接口
	RegisterLuaFuns(m_pLuaState);
}

bool LuaInterface::Init()
{
	m_pLuaState = luaL_newstate();
	if (m_pLuaState)
		lua_atpanic(m_pLuaState, &panic);
	else
	{
		LogPrint("luaL_newstate error");
		return false;
	}
	
	LoadAllLuaLibs();

	// TODO: 这块没有看明白 还是需要详细了解下
	// 加载入口文件
	if (luaL_loadfile(m_pLuaState, LuaInitFileName))
	{
		LogPrint("error luaL_loadfile filename = [%s].", LuaInitFileName);
		lua_settop(m_pLuaState, 0);
		return false;
	}

	lua_pushcfunction(m_pLuaState, LuaInterface::traceback);	// push traceback function
	m_iFunTracebackRef = luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	if (m_iFunTracebackRef == LUA_NOREF || m_iFunTracebackRef == LUA_REFNIL)
	{
		LogPrint("luaL_ref LUA_REGISTRYINDEX Error [%d].", m_iFunTracebackRef);
		lua_settop(m_pLuaState, 0);
		return false;
	}

	int base = lua_gettop(m_pLuaState); // function traceback  index
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, m_iFunTracebackRef);
	lua_insert(m_pLuaState, base); // put it under chunk and args*

	int status = lua_pcall(m_pLuaState, 0, LUA_MULTRET, base);
	lua_remove(m_pLuaState, base); // remove traceback function

	if (status != 0)
	{
		LogPrint("error: luaL_loadfile Failed, filename = [%s].", LuaInitFileName);
		lua_settop(m_pLuaState, 0);
		return false;
	}

	lua_getglobal(m_pLuaState, LuaInitFunctionName);
	m_iLuaInitFunRef = luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	if (m_iLuaInitFunRef == LUA_NOREF || m_iLuaInitFunRef == LUA_REFNIL)
	{
		LogPrint("luaL_ref: LuaInitFunRef Failed [%d]", m_iLuaInitFunRef);
		lua_settop(m_pLuaState, 0);
		return false;
	}

	lua_getglobal(m_pLuaState, LuaErrorFunctionName);
	m_iLuaErrorFunRef = luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	if (m_iLuaErrorFunRef == LUA_NOREF || m_iLuaErrorFunRef == LUA_REFNIL)
	{
		LogPrint("luaL_ref: ErrorHanle Failed [%d].", m_iLuaErrorFunRef);
		lua_settop(m_pLuaState, 0);
		return false;
	}

	return true;
}

int LuaInterface::traceback(lua_State* L)
{
	// message is not a string
	if (!lua_isstring(L, 1))
		return 1;

	lua_getglobal(L, "debug");
	if (!lua_istable(L, -1))
	{
		LogPrint("lua_getglobal(L, debug) is not table!");
		lua_pop(L, 1);
		return 1;
	}

	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1))
	{
		LogPrint("lua_getglobal(L, traceback) is not function!");
		lua_pop(L, 2);
		return 1;
	}
	// push error message
	lua_pushvalue(L, 1);
	// skip this function and traceback
	lua_pushinteger(L, 2);
	// call debug.traceback
	lua_call(L, 2, 1);

	if (!lua_isnil(L, -1))
	{
		const char* msg = lua_tostring(L, -1);
		if (msg == NULL)
			msg = "error object if not a string";

		LogPrint("error: [%s].", msg);
		lua_pop(L, 1);
	}
	return 1;
}

int LuaInterface::Run()
{
	// 获取消息
	size_t iLen;
	SOCKET Conn;
	// TODO: 相关代码ID:201508042055
	void* pMsg = m_pRead->GetMsg(iLen, Conn);
	if (pMsg == NULL)
		return 1;

	LogPrint("LuaInterface Run[0x%08x]: msg:[0x%08x] len:[%d].", m_pRead, pMsg, iLen);

	// lua 入口函数压入栈中
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, m_iLuaInitFunRef);

	// 第一个参数
	// lua_pushstring(m_pLuaState, (char*)pMsg);
	lua_pushlightuserdata(m_pLuaState, pMsg);

	// 第二个参数
	lua_pushnumber(m_pLuaState, iLen);

	// 第三个参数
	lua_pushlightuserdata(m_pLuaState, &Conn);

	int narg = 3;
	// 错误回调函数
	int base = lua_gettop(m_pLuaState) - narg;
	lua_pushcfunction(m_pLuaState, LuaInterface::traceback);
	lua_insert(m_pLuaState, base);

	int status = lua_pcall(m_pLuaState, narg, 1, base);

	lua_remove(m_pLuaState, base);	// remove traceback function
	
	int ret = 0;
	if (status == 0)
	{
		ret = (int)lua_tonumber(m_pLuaState, -1);
		lua_pop(m_pLuaState, 1);
	}
	else
	{
		ret = CallErrorHandle(status);
	}
	lua_settop(m_pLuaState, 0);

	return ret;
}

int LuaInterface::CallErrorHandle(int errcode)
{
	// TODO: 这个地方为什么要设置为0
	lua_settop(m_pLuaState, 0);

	// lua错误函数压入栈中
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, m_iLuaErrorFunRef);

	// 压入参数
	lua_pushnumber(m_pLuaState, errcode);

	int narg = 1;
	// 调用函数
	int base = lua_gettop(m_pLuaState) - narg;	// function index
	lua_pushcfunction(m_pLuaState, LuaInterface::traceback);	// push traceback function
	lua_insert(m_pLuaState, base);	// put it under chunk and args

	int status = lua_pcall(m_pLuaState, narg, 1, base);
	lua_remove(m_pLuaState, base);	// remove traceback function

	int ret = 0;
	if (status == 0)
	{
		ret = (int)lua_tonumber(m_pLuaState, -1);
		lua_pop(m_pLuaState, 1);
	}
	
	lua_settop(m_pLuaState, 0);
	return ret;
}

int LuaInterface::SendMsg(lua_State* L)
{
	void*	pMsg = (void *)lua_touserdata(L, 1);
	size_t	iLen = (size_t)lua_tonumber(L, 2);
	SOCKET*	Conn = (SOCKET*)lua_touserdata(L, 3);

	int iError;
	DWORD dwBytes;
	// 发送第一个数据
	// git snap(): Erro = 
	OverLapped* pSendOver = new OverLapped;
	pSendOver->opType = OverLapped::OLOpType::EOLOT_Send;
	ZeroMemory(pSendOver->dataBuffer, OverLappedBufferLen);
	// sprintf_s(pSendOver->dataBuffer, "server new send time %d", GetTickCount());
	memcpy(pSendOver->dataBuffer, pMsg, iLen);
	pSendOver->sysBuffer.len = iLen; // strlen(pSendOver->dataBuffer);
	int nResult2 = WSASend(*Conn, &pSendOver->sysBuffer, 1, &dwBytes, 0, &pSendOver->sysOverLapped, 0);
	if (nResult2 == SOCKET_ERROR && ((iError = WSAGetLastError()) != ERROR_IO_PENDING))
	{
		Log("EOLOT_Accept [%d] WSASend Error[%d].\n", *Conn, iError);
		closesocket(*Conn);
		delete pSendOver;
	}
	return 0;
}

void LuaInterface::SetMsgBuffer(NetMsg* pRead, NetMsg* pWrite)
{
	m_pRead = pRead;
	m_pWrite = pWrite;
}

const luaL_Reg NetWork[] = {
	{"sendmsg", LuaInterface::SendMsg},
};

static int RegisterLuaNetworkFuns(lua_State* L)
{
	luaL_checkversion(L);
	luaL_newlib(L, NetWork);
	return 1;
}

static const luaL_Reg RegisterLuaLibs[] = {
	{"Network", RegisterLuaNetworkFuns},
	{"pbc", luaopen_protobuf_c},
	{NULL, NULL},
};


static int RegisterLuaFuns(lua_State* L)
{
	const luaL_Reg *lib;
	/* call open functions from 'loadedlibs' and set results to global table */
	for (lib = RegisterLuaLibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
	return 1;
}