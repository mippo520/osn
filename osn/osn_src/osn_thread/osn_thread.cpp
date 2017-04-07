//
//  osn_thread.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_thread.h"
#include <thread>
#include <unistd.h>

oBOOL OsnThread::s_isQuit = false;
std::mutex OsnThread::s_Mutex;
std::condition_variable OsnThread::s_Cond;
oINT32 OsnThread::s_SleepCount = 0;

OsnThread::OsnThread()
    : m_Thread(&OsnThread::work, this)
    , m_Id(0)
{
    
}

OsnThread::~OsnThread()
{
    std::condition_variable condvar;
}

void OsnThread::init(oBOOL isMainWait)
{
    if (isMainWait) {
        m_Thread.join();
    }
    else
    {
        m_Thread.detach();
    }
}



