#include "osn_socket.h"
#include "osn_socket_manager.h"
#include "osn_service_manager.h"
#include "osn_coroutine_manager.h"

OsnSocket::OsnSocket()
{
}


OsnSocket::~OsnSocket()
{
}

void OsnSocket::init()
{
	RegistDispatchFunc(ePType_Socket, &OsnSocket::dispatchSocket, this);
	m_mapDispatchFunc[eOST_Data] = std::bind(&OsnSocket::funcSocketData, this, std::placeholders::_1);
	m_mapDispatchFunc[eOST_Connect] = std::bind(&OsnSocket::funcSocketConnect, this, std::placeholders::_1);
	m_mapDispatchFunc[eOST_Accept] = std::bind(&OsnSocket::funcSocketAccept, this, std::placeholders::_1);
	m_mapDispatchFunc[eOST_Error] = std::bind(&OsnSocket::funcSocketError, this, std::placeholders::_1);
	m_mapDispatchFunc[eOST_Close] = std::bind(&OsnSocket::funcSocketClose, this, std::placeholders::_1);
	m_mapDispatchFunc[eOST_Warning] = std::bind(&OsnSocket::funcSocketWarning, this, std::placeholders::_1);
}

oINT32 OsnSocket::open(const oINT8 *addr, oINT32 port)
{
	oUINT32 svrId = g_ServiceManager.getCurService();
	oINT32 sock = g_SocketManager.connect(svrId, addr, port);
	return connect(sock);
}

oINT64 OsnSocket::write(oINT32 sock, const void *pBuff, oINT32 sz)
{
	return g_SocketManager.send(sock, pBuff, sz);
}

void OsnSocket::suspend(stSocketInfo &info)
{
	info.co = g_CorotineManager.running();
	g_ServiceManager.wait(info.co);
	if (info.closing > 0)
	{
		g_ServiceManager.wakeup(info.closing);
	}
}

void OsnSocket::wakeup(stSocketInfo &info)
{
	oUINT32 co = info.co;
	if (co > 0)
	{
		info.co = 0;
		g_ServiceManager.wakeup(co);
	}
}

oINT32 OsnSocket::connect(oINT32 id)
{
	stSocketInfo &info = m_mapSocketInfo[id];
	info.strProtocal = "TCP";
	info.bConnecting = true;
	suspend(info);
	return id;
}

void OsnSocket::dispatchSocket(const OsnPreparedStatement &stmt)
{
	const stOsnSocketMsg *pSM = (stOsnSocketMsg*)stmt.getUInt64(0);
	if (NULL != pSM)
	{
		MAP_SOCKET_MSG_FUNC_ITR itr = m_mapDispatchFunc.find(pSM->type);
		if (itr != m_mapDispatchFunc.end())
		{
			itr->second(pSM);
		}
	}
}

void OsnSocket::funcSocketData(const stOsnSocketMsg *msg)
{

}

void OsnSocket::funcSocketConnect(const stOsnSocketMsg *msg)
{
	oINT32 id = msg->id;
	MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(id);
	if (itr != m_mapSocketInfo.end())
	{
		stSocketInfo &info = itr->second;
		info.bConnected = true;
		wakeup(info);
	}
}

void OsnSocket::funcSocketClose(const stOsnSocketMsg *msg)
{

}

void OsnSocket::funcSocketAccept(const stOsnSocketMsg *msg)
{

}

void OsnSocket::funcSocketError(const stOsnSocketMsg *msg)
{

}

void OsnSocket::funcSocketWarning(const stOsnSocketMsg *msg)
{

}
