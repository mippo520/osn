#include "TestService2.h"
#include "osn_service_manager.h"
#include <unistd.h>
#include <stdlib.h>


TestService2::TestService2()
{
}


TestService2::~TestService2()
{
}

void TestService2::dispatchLua(const OsnPreparedStatement &stmt)
{
//    printf("ret fun ========>");
//	stmt.printContext();
	OsnPreparedStatement msg;
	if (100 == stmt.getUInt32(0))
	{
		msg.setFloat32(0, 0.5f);
		msg.setInt32(1, 101);
		msg.setString(2, "def");
		g_ServiceManager.ret(msg);
	}
	else
	{
		msg.setFloat32(0, 1.5f);
		msg.setInt32(1, 201);
		msg.setString(2, "ghi");
		g_ServiceManager.ret(msg);
        g_ServiceManager.exit();
    }
    
}

void TestService2::start(const OsnPreparedStatement &stmt)
{
	registDispatchFunc(ePType_Lua, static_cast<CO_MEMBER_FUNC>(&TestService2::dispatchLua));
    OsnPreparedStatement msg;
    msg.setString(0, "send begin");
    msg.setUInt32(1, getId());

    g_ServiceManager.send(getId() % 1000, ePType_Lua, msg);
}

void TestService2::exit()
{
    g_ServiceManager.startService<TestService2>();
}
