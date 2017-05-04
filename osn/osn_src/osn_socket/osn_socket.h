#pragma once
#include <map>
#include "osn.h"
#include "osn_prepared_statement.h"
#include "osn_socket_head.h"

struct stOsnSocketMsg;
struct stSocketInfo;

class OsnSocket
{
public:
	OsnSocket();
	~OsnSocket();
public:
	void init();
	oINT32 open(const oINT8 *addr, oINT32 port);
	oINT64 write(oINT32 sock, const void *pBuff, oINT32 sz);
    oINT32 listen(oUINT32 opaque, const oINT8 *addr, oINT32 nPort = 0, oINT32 nBackLog = s_nBacklog);
private:
	void suspend(stSocketInfo &info);
	void wakeup(stSocketInfo &info);
	oINT32 connect(oINT32 id);
	void dispatchSocket(const OsnPreparedStatement &stmt);

	void funcSocketData(const stOsnSocketMsg *msg);
	void funcSocketConnect(const stOsnSocketMsg *msg);
	void funcSocketClose(const stOsnSocketMsg *msg);
	void funcSocketAccept(const stOsnSocketMsg *msg);
	void funcSocketError(const stOsnSocketMsg *msg);
	void funcSocketWarning(const stOsnSocketMsg *msg);
private:
	typedef std::function<void(const stOsnSocketMsg *)> SOCKET_MSG_FUNC;
	typedef std::map<oINT32, SOCKET_MSG_FUNC> MAP_SOCKET_MSG_FUNC;
	typedef MAP_SOCKET_MSG_FUNC::iterator MAP_SOCKET_MSG_FUNC_ITR;
	MAP_SOCKET_MSG_FUNC m_mapDispatchFunc;
	typedef std::map<oINT32, stSocketInfo> MAP_SOCKET_INFO;
	typedef MAP_SOCKET_INFO::iterator MAP_SOCKET_INFO_ITR;
	MAP_SOCKET_INFO m_mapSocketInfo;
};

