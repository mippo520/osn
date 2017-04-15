//
//  osn_start.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_start.h"
#include <stdio.h>
#include "osn_thread/osn_thread_timer.h"
#include "osn_thread/osn_thread_worker.h"
#include "osn_service_manager.h"
#include "osn_service.h"
#include <ucontext.h>
#include <sys/ucontext.h>

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
    printf("OsnStart::init \n");
    
//    for (oINT32 i = 0; i < 100; ++i) {
//        g_ServiceManager.startService<OsnService>();
//    }
    
//    g_ServiceManager.send(1, -1);
    
    clearThread();
    
    createThread<OsnTimerThread>();
    
    for (oINT32 i = 0; i < s_nWorkerCount; ++i) {
        createThread<OsnWorkerThread>(s_WeightArr[i]);
    }
}

void OsnStart::start()
{
    printf("OsnStart::start \n");
    
    for (oUINT32 i = 0; i < m_vecThread.size(); ++i) {
        m_vecThread[i]->init();
    }
}

void OsnStart::exit()
{
    clearThread();
    printf("OsnStart::exit \n");
}

oBOOL OsnStart::checkAbort()
{
//    static oINT32 i = 0;
//    ++i;
//    if (i > 10) {
//        return  true;
//    }
//    else
//    {
        return false;
//    }
}

void OsnStart::clearThread()
{
    for (oUINT32 i = 0; i < m_vecThread.size(); ++i) {
        SAFE_DELETE(m_vecThread[i]);
    }
    m_vecThread.clear();
}

void OsnStart::wakeup(oINT32 nSleepCount)
{
    if (OsnThread::s_SleepCount > nSleepCount) {
        OsnThread::s_Cond.notify_one();
    }
}

