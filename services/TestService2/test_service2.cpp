#include "test_service2.h"
#include "I_osn_service.h"
#include <unistd.h>
#include "I_osn_coroutine.h"

AddService_Instance(TestService2)

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
//        printf("%lu ==========> command!\n", getId());
        g_Osn->exit();
        
//        OsnPreparedStatement msg2;
//        msg2.setInt32(0, 101);
//        msg2.setUInt32(1, g_Coroutine->running());
//        g_Osn->send(2, ePType_Lua, msg2);
//        printf("%lu ==========> sleep!\n", getId());
//        g_Osn->wait();
//        printf("%lu ==========> wake up!\n", getId());
    }
    else if(101 == tag)
    {
        sleep(1);
        OsnPreparedStatement msg3;
        msg3.setInt32(0, 102);
        msg3.setUInt32(1, stmt.getUInt32(1));
        printf("%llu ==========> timeup to send wakeup command!\n", getId());
        g_Osn->send(1, ePType_User, msg3);
        g_Osn->exit();
    }
    else if(102 == tag)
    {
        sleep(1);
        printf("%llu ==========> receive wakeup command!\n", getId());
        g_Osn->wakeup(stmt.getUInt32(1));
        printf("%llu ==========> call wakeup func!\n", getId());
        g_Osn->exit();
    }
}

void TestService2::start(const OsnPreparedStatement &stmt)
{
//    printf("start %lu\n", getId());
	RegistDispatchFunc(ePType_User, &TestService2::dispatchLua, this);
//    OsnPreparedStatement msg;
//    msg.setString(0, "send begin");
//    msg.setUInt32(1, getId());
//
//    oUINT32 unId = (getId() - 1) % 1000 + 1;
//    g_ServiceManager.send(unId, ePType_Lua, msg);
    
//    if (1 == getId())
//    {
//        sleep(1);
    static oBOOL b = false;
    if (!b)
    {
        b = true;
        g_Osn->startService("TestService");
        printf("start TestService!\n");
    }
    
    OsnPreparedStatement msg;
    msg.setInt32(0, 100);
    g_Osn->send(getId(), ePType_User, msg);
    

//    }
}

void TestService2::exit()
{
    OsnService::exit();
    g_Osn->startService("TestService2");
}
