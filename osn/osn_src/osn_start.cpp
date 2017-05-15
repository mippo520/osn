//
//  osn_start.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_start.h"
#include "osn_thread_timer.h"
#include "osn_thread_worker.h"
#include "osn_thread_socket.h"
#include "osn_service_manager.h"
#include "osn_socket_manager.h"

oINT32 OsnStart::s_WeightArr[] = {
    -1, -1, -1, -1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
};

oINT32 OsnStart::s_nThreadIdIdx = 0;

OsnStart::OsnStart()
{
    
}

OsnStart::~OsnStart()
{
    clearThread();
}

void OsnStart::init()
{
// 	for (oINT32 i = 0; i < 1000; ++i)
// 	{
    g_Osn->startService("TestService2");
//    g_Osn->startService("TestService");
// 	}
// 
// 	for (oINT32 i = 0; i < 1000; ++i)
// 	{
// 		g_ServiceManager.startService<TestService2>();
// 	}

//    g_ServiceManager.startService<TestService2>();
//    g_ServiceManager.startService<TestService2>();
    
    g_SocketManager.init();
    
	clearThread();
	createThread<OsnTimerThread>();
    createThread<OsnSocketThread>();
	for (oINT32 i = 0; i < s_nWorkerCount; ++i)
    {
		createThread<OsnWorkerThread>(s_WeightArr[i]);
	}
}

void OsnStart::start()
{
    for (oUINT32 i = 0; i < m_vecThread.size(); ++i)
    {
        m_vecThread[i]->init();
    }
}

void OsnStart::exit()
{
    g_SocketManager.release();
    clearThread();
}

oBOOL OsnStart::checkAbort()
{
    return false;
}

void OsnStart::clearThread()
{
    if (m_vecThread.size() > 0)
    {
        for (oUINT32 i = 0; i < m_vecThread.size(); ++i)
        {
            SAFE_DELETE(m_vecThread[i]);
        }
        m_vecThread.clear();
    }
}

void OsnStart::wakeup(oINT32 nSleepCount)
{
    if (OsnThread::s_SleepCount > nSleepCount)
    {
        OsnThread::s_Cond.notify_one();
    }
}

