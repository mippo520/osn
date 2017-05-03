#include "TestService2.h"
#include "osn_service_manager.h"
#include <unistd.h>
#include "osn_coroutine_manager.h"


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
//	OsnPreparedStatement msg;
//	if (100 == stmt.getUInt32(0))
//	{
//		msg.setFloat32(0, 0.5f);
//		msg.setInt32(1, 101);
//		msg.setString(2, "def");
//		g_ServiceManager.ret(msg);
//	}
//	else
//	{
//		msg.setFloat32(0, 1.5f);
//		msg.setInt32(1, 201);
//		msg.setString(2, "ghi");
//		g_ServiceManager.ret(msg);
//        g_ServiceManager.exit();
//    }
    oINT32 tag = stmt.getInt32(0);
    if (100 == tag)
    {
        
        OsnPreparedStatement msg2;
        msg2.setInt32(0, 101);
        msg2.setUInt32(1, g_CorotineManager.running());
        g_ServiceManager.send(2, ePType_Lua, msg2);
        printf("%lu ==========> sleep!\n", getId());
        g_ServiceManager.wait();
        printf("%lu ==========> wake up!\n", getId());
    }
    else if(101 == tag)
    {
        sleep(1);
        OsnPreparedStatement msg3;
        msg3.setInt32(0, 102);
        msg3.setUInt32(1, stmt.getUInt32(1));
        printf("%lu ==========> timeup to send wakeup command!\n", getId());
        g_ServiceManager.send(1, ePType_Lua, msg3);
        g_ServiceManager.exit();
    }
    else if(102 == tag)
    {
        sleep(1);
        printf("%lu ==========> receive wakeup command!\n", getId());
        g_ServiceManager.wakeup(stmt.getUInt32(1));
        printf("%lu ==========> call wakeup func!\n", getId());
        g_ServiceManager.exit();
    }
}

void TestService2::start(const OsnPreparedStatement &stmt)
{
    printf("start %d\n", getId());
	RegistDispatchFunc(ePType_Lua, &TestService2::dispatchLua, this);
//    OsnPreparedStatement msg;
//    msg.setString(0, "send begin");
//    msg.setUInt32(1, getId());
//
//    oUINT32 unId = (getId() - 1) % 1000 + 1;
//    g_ServiceManager.send(unId, ePType_Lua, msg);
    
    if (1 == getId())
    {
//        sleep(1);
        OsnPreparedStatement msg;
        msg.setInt32(0, 100);
        g_ServiceManager.send(1, ePType_Lua, msg);
    }
}

void TestService2::exit()
{
    g_ServiceManager.startService<TestService2>();
}
