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
    clearThread();
    
    createThread<OsnTimerThread>();
    
    for (oINT32 i = 0; i < s_nWorkerCount; ++i) {
        createThread<OsnWorkerThread>();
    }
}

void OsnStart::start()
{
    
    for (oINT32 i = 0; i < m_vecThread.size(); ++i) {
        m_vecThread[i]->init();
    }
    
    printf("OsnStart::start \n");
}

void OsnStart::exit()
{
    clearThread();
    printf("OsnStart::exit \n");
}

oBOOL OsnStart::checkAbort()
{
    static oINT32 i = 0;
    ++i;
    if (i > 10) {
        return  true;
    }
    else
    {
        return false;
    }
}

void OsnStart::clearThread()
{
    for (oINT32 i = 0; i < m_vecThread.size(); ++i) {
        SAFE_DELETE(m_vecThread[i]);
    }
    m_vecThread.clear();
}

