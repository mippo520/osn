#include "TestService.h"
#include "osn_service_manager.h"
#include "osn_socket_manager.h"
#include "TestService3.h"


TestService::TestService()
	: m_nListenId(-1)
{
}


TestService::~TestService()
{
}

void TestService::dispatchLua(const OsnPreparedStatement &stmt)
{
    
    oUINT32 n = 0;
    if (stmt.getString(0) == "send begin")
    {
        n = stmt.getUInt32(1);
    }
    
    if (n > 0)
    {
        OsnPreparedStatement msg;
        msg.setUInt32(0, 100);
        msg.setString(1, "abc");
        msg.setInt8(2, 20);
        const OsnPreparedStatement &ret1 = g_ServiceManager.call(n, ePType_Lua, msg);
//      printf("call func=========>");
//      msg.printContext();
    
        msg.clear();
        msg.setUInt32(0, 101);
        const OsnPreparedStatement &ret2 = g_ServiceManager.call(n, ePType_Lua, msg);
//      printf("call func=========>");
//      msg.printContext();
    
//      OsnPreparedStatement *pMsg = NULL;
//      oUINT8 nCount = pMsg->getPreparedStatementDataCount();
//      printf("eeeeeeeeeeeeeeeeee! %d\n", nCount);
    }
}

void TestService::dispatchSocket(const OsnPreparedStatement &stmt)
{
	const stOsnSocketMsg *pSM = (stOsnSocketMsg*)stmt.getUInt64(0);
	if (NULL != pSM)
	{
		switch (pSM->type)
		{
		case eOST_Connect:
			if (pSM->id == m_nListenId)
			{
//				g_ServiceManager.startService<TestService3>();
				break;
			}
			if (m_setConnectedId.find(pSM->id) == m_setConnectedId.end())
			{
				printf("close unknown connection %d!\n", pSM->id);
				g_SocketManager.close(getId(), pSM->id);
			}
			break;
		case eOST_Accept:
		{
			assert(pSM->id == m_nListenId);
			m_setConnectedId.insert(pSM->ud);
			g_SocketManager.start(getId(), pSM->ud);
// 			OsnPreparedStatement arg;
// 			arg.setInt32(0, pSM->ud);
// 			arg.setString(1, std::string(pSM->pBuffer, pSM->nSize));
// 			g_ServiceManager.send(getId(), ePType_Text, arg);
		}
			break;
		case eOST_Data:
			printf("id = %d, ud = %d, msg size is %d!\n", pSM->id, pSM->ud, pSM->nSize);
            g_SocketManager.send(pSM->id, pSM->pBuffer, pSM->nSize);
			break;
		case eOST_Close:
		case eOST_Error:
		{
			std::set<oINT32>::iterator itr = m_setConnectedId.find(pSM->id);
			if (itr != m_setConnectedId.end())
			{
				oINT32 id = *itr;
				m_setConnectedId.erase(itr);
				g_SocketManager.close(getId(), id);
			}
		}
			break;
		default:
			printf("socket message type error ====> %d\n", pSM->type);
			break;
		}
	}
	else
	{
		printf("OsnSocketMsg is NULL!\n");
	}
	SAFE_DELETE(pSM);
}

void TestService::dispatchText(const OsnPreparedStatement &stmt)
{
	std::string strCommand = stmt.getString(1);
	if (strCommand.size() == 0)
	{
		return;
	}

	if ("start" == strCommand)
	{
		g_SocketManager.start(getId(), stmt.getInt32(0));
	}
}

void TestService::start(const OsnPreparedStatement &stmt)
{
	RegistDispatchFunc(ePType_Socket, &TestService::dispatchSocket, this);
	RegistDispatchFunc(ePType_Text, &TestService::dispatchText, this);
	// 	registDispatchFunc(ePType_Lua, static_cast<CO_MEMBER_FUNC>(&TestService::dispatchLua));
	m_nListenId = g_SocketManager.listen(getId(), "", 18523);
    g_SocketManager.start(getId(), m_nListenId);
}

