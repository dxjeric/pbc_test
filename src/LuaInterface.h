//-------------------------------------------------------------------------------------------------
//	Created:	2015-7-29   20:14
//	File Name:	LuaInterface.h
//	Author:		Eric(沙鹰)
//	PS:			如果发现说明错误，代码风格错误，逻辑错问题，设计问题，请告诉我。谢谢！
//  Email:		frederick.dang@gmail.com
//	Purpose:	C和Lua粘接层
//-------------------------------------------------------------------------------------------------
#include "Common.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Lua 包含的头文件
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#ifdef __cplusplus
}
#endif // __cplusplus

// TODO: 相关代码ID:201508042055
//static NetMsg ReadMsg;
//static NetMsg WriteMsg;

typedef int(*FuncLua)(lua_State*);

class LuaInterface
{
public:
	LuaInterface();
	~LuaInterface();
public:
	// 初始化lua_State
	bool Init();

	// 获取lua_State
	lua_State* GetLuaState() {return m_pLuaState;};
	
	// 调用lua接口
	int Run();

	// lua接口 发送消息
	static int SendMsg(lua_State* L);

	// 临时代码 不想去再做设计
	void SetMsgBuffer(NetMsg* pRead, NetMsg* pWrite);
private:
	static int	traceback(lua_State* L);
	int			CallErrorHandle(int errcode);
	// 加载所有需要的lua lib
	void		LoadAllLuaLibs();

private:
	lua_State*	m_pLuaState;
	int			m_iFunTracebackRef;
	int			m_iLuaInitFunRef;
	int			m_iLuaErrorFunRef;
	static LuaInterface* m_pLuaInterface;

private:
	NetMsg*		m_pRead;
	NetMsg*		m_pWrite;
};