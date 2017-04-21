#include "TestService.h"
#include "osn_service_manager.h"



TestService::TestService()
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
    
    if (n > 0) {
        OsnPreparedStatement msg;
        msg.setUInt32(0, 100);
        msg.setString(1, "abc");
        msg.setInt8(2, 20);
        msg = g_ServiceManager.call(n, ePType_Lua, msg);
//      printf("call func=========>");
//      msg.printContext();
    
        msg.clear();
        msg.setUInt32(0, 101);
        msg = g_ServiceManager.call(n, ePType_Lua, msg);
//      printf("call func=========>");
//      msg.printContext();
    
//      OsnPreparedStatement *pMsg = NULL;
//      oUINT8 nCount = pMsg->getPreparedStatementDataCount();
//      printf("eeeeeeeeeeeeeeeeee! %d\n", nCount);
    }
}

void TestService::start(const OsnPreparedStatement &stmt)
{
	registDispatchFunc(ePType_Lua, static_cast<CO_MEMBER_FUNC>(&TestService::dispatchLua));
}

