#include "test_service2.h"
#include "I_osn_service.h"
#include <unistd.h>
#include "I_osn_coroutine.h"
#include "osn_gate_head.h"
#include "osn_socket_channel.h"

AddService_Instance(TestService2)

TestService2::TestService2()
{
}


TestService2::~TestService2()
{
}

void TestService2::dispatchLua(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
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

//    OsnPreparedStatement arg;
//    arg.setUInt64(0, g_Osn->self());
//    ID_SERVICE ts = g_Osn->startService("TestService", arg);
    
//    m_Gate = g_Osn->startService("Gate");
//    OsnPreparedStatement arg;
//    arg.setInt32(0, osn_gate::Func_Open);
//    arg.setString(1, "");
//    arg.setInt32(2, 12366);
//    g_Osn->call(m_Gate, ePType_User, arg);
//    printf("gate open finish!\n");
    
//    printf("start TestService!\n");
    
//    OsnPreparedStatement msg;
//    msg.setUInt64(0, getId());
//    msg.setUInt64(1, m_Gate);
//    msg.setString(2, "abc");
//    const OsnPreparedStatement &ret = g_Osn->call(ts, ePType_User, msg);
//    printf("call finish! %s!\n", ret.getCharPtr(0));
//    g_Osn->exit();

    

//    }
    
    
//    ID_SERVICE addr = g_Osn->startService("TestService");
//    OsnPreparedStatement arg1;
//    arg1.setInt32(0, 10086);
//    SHARED_PTR_STMT argCall1 = g_Osn->call(addr, eProtoType::ePType_User, 10086, "a", arg1, 'b', eProtoType::ePType_User);
//    printf("argCall1 index 0 = %d\n", argCall1->getInt32(0));
//    
//    OsnPreparedStatement arg2;
//    arg2.setInt32(0, 10087);
//    SHARED_PTR_STMT argCall2 = g_Osn->call(addr, eProtoType::ePType_User, arg2);
//    printf("argCall2 index 0 = %d\n", argCall2->getInt32(0));
//    
//    OsnPreparedStatement arg3;
//    arg3.setInt32(0, 10088);
//    SHARED_PTR_STMT argCall3 = g_Osn->call(addr, eProtoType::ePType_User, arg3);
//    printf("argCall3 index 0 = %d\n", argCall3->getInt32(0));
//
//    printf("argCall1 index 1 = %d\n", argCall1->getInt32(1));
//    printf("argCall2 index 1 = %d\n", argCall2->getInt32(1));
//    printf("argCall3 index 1 = %d\n", argCall3->getInt32(1));
    

//    for (oINT32 i = 0; i < 100; ++i)
//    {
//        printf("test2 for ========> 1\n");
//
//        SHARED_PTR_STMT argCall = g_Osn->call(addr, ePType_User);
//        printf("test call back 1 %d\n", argCall->getInt32(0));
//
//        g_Osn->fork([=](const OsnPreparedStatement &stmt1)
//                    {
//                        oINT32 i = stmt1.getInt32(0);
//                        printf("Test2 fork %d\n", i);
//                        SHARED_PTR_STMT argCall = g_Osn->call(addr, ePType_User);
//                        printf("test call back 2 %d\n", argCall->getInt32(0));
//
//                        g_Osn->fork([=](const OsnPreparedStatement &stmt2)
//                                    {
//                                        oINT32 i = stmt2.getInt32(0);
//                                        printf("Test2 fork step2 %d\n", i);
//                                    }, stmt1);
//                    }, i);
//        printf("test2 for ========> 2\n");
//    }
    
    stChannelDesc desc;
    desc.strHost = "127.0.0.1";
    desc.nPort = 8888;
    OsnSocketChannel channel(desc);
    channel.init();
    channel.connect(false);

    int i = 0;
    ++i;
    
}

void TestService2::exit()
{
    OsnService::exit();
//    g_Osn->startService("TestService2");
}
