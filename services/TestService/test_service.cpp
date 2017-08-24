//
//  Service.cpp
//  osn
//
//  Created by zenghui on 17/5/3.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "test_service.h"
#include "osn_socket.h"
#include "I_osn_coroutine.h"
#include <unistd.h>
#include "osn_gate_head.h"

AddService_Instance(TestService)

TestService::TestService()
    : m_fd(-1)
{
    
}

TestService::~TestService()
{
    
}

void TestService::start(const OsnPreparedStatement &stmt)
{
	RegistDispatchFunc(ePType_User, &TestService::dispatchLua, this);
//	m_Socket.init();
//    m_SockId = m_Socket.listen("", 12366);
//
//    g_Osn->send(getId(), ePType_User);
//    std::string strError = "";
//    m_Socket.start(m_SockId, strError, std::bind(&TestService::acceptFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
//	oINT32 fd = m_Socket.open("127.0.0.1", 18888);
//	m_Socket.write(fd, "aaaaa", 5);
//    printf("TestService::start!\n");
//    ID_SERVICE id = stmt.getUInt64(0);
//    g_Osn->call(id, ePType_User);
//    printf("TestService::start finish!\n");
}

void TestService::dispatchLua(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    printf("TestService::dispatchLua =========> \n");
    oINT32 value = stmt.getInt32(0);
    if (10086 == value)
    {
        stmt.printContext("TestService 10086");
        OsnPreparedStatement arg1;
        arg1.setInt32(0, 123);
        arg1.setInt32(1, 234);
        g_Osn->ret(arg1);
    }
    else if(10087 == value)
    {
        OsnPreparedStatement arg1;
        arg1.setInt32(0, 345);
        arg1.setInt32(1, 456);
        g_Osn->ret(arg1);
        
    }
    else if(10088 == value)
    {
        OsnPreparedStatement arg1;
        arg1.setInt32(0, 567);
        arg1.setInt32(1, 678);
        g_Osn->ret(arg1);
        
    }
    else
    {
        g_Osn->ret(789, 890);
    }
    
//    sleep(5);
//    oINT32 fd = stmt.getInt32(0);
//    ID_SERVICE gate = stmt.getUInt64(1);
//    OsnPreparedStatement arg;
//    arg.setInt32(0, osn_gate::Func_Kick);
//    arg.setInt32(1, fd);
//    g_Osn->call(gate, ePType_User, arg);
//    printf("kick finish!\n");  
//    ID_SERVICE ts2 = stmt.getUInt64(0);
//    ID_SERVICE gate = stmt.getUInt64(1);
//    g_Osn->redirect(gate, source, ePType_User, session, stmt);
//    printf("redirect finish!\n");
//    g_Osn->exit();
//    while (true)
//    {
//        if (m_fd > 0)
//        {
//            oINT32 nSize = 0;
//            oINT8 *pBuff = m_Socket.readLine(m_fd, nSize);
//            if (NULL == pBuff)
//            {
//                m_fd = 0;
//            }
//            else
//            {
//                printf("buff Size = %d\n", nSize);
//                SAFE_FREE(pBuff);
//            }
//
//        }
//        else
//        {
//            m_curCO = g_Coroutine->running();
//            g_Osn->wait(m_curCO);
//        }
//    }
}

void TestService::acceptFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz)
{
    std::string strErr;
    m_fd = m_Socket.start(fd, strErr);
//    if (m_fd > 0)
//    {
//        g_Osn->wakeup(m_curCO);
//    }
//    static oINT32 i = 0;
//    ++i;
//    m_Socket.closeFD(stmt.getInt32(0));
//    if (i >= 10)
//    {
//        m_Socket.close(m_SockId);
//    }
}


