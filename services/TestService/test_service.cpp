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
	m_Socket.init();
    m_SockId = m_Socket.listen("127.0.0.1", 18523);
    
    g_Osn->send(getId(), ePType_User);
    std::string strError = "";
    m_Socket.start(m_SockId, strError, std::bind(&TestService::acceptFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
//	oINT32 fd = m_Socket.open("127.0.0.1", 18888);
//	m_Socket.write(fd, "aaaaa", 5);
    printf("TestService::start!\n");
}

void TestService::dispatchLua(const OsnPreparedStatement &stmt)
{
    while (true)
    {
        if (m_fd > 0)
        {
            oINT32 nSize = 0;
            oINT8 *pBuff = m_Socket.readLine(m_fd, nSize);
            if (NULL == pBuff)
            {
                m_fd = 0;
            }
            else
            {
                printf("buff Size = %d\n", nSize);
                SAFE_FREE(pBuff);
            }

        }
        else
        {
            m_curCO = g_Coroutine->running();
            g_Osn->wait(m_curCO);
        }
    }
}

void TestService::acceptFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz)
{
    std::string strErr;
    m_fd = m_Socket.start(fd, strErr);
    if (m_fd > 0)
    {
        g_Osn->wakeup(m_curCO);
    }
//    static oINT32 i = 0;
//    ++i;
//    m_Socket.closeFD(stmt.getInt32(0));
//    if (i >= 10)
//    {
//        m_Socket.close(m_SockId);
//    }
}


