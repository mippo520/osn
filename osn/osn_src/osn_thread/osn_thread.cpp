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

OsnThread::OsnThread()
    : m_Thread(&OsnThread::work, this)
    , m_nId(0)
{
    
}

OsnThread::~OsnThread()
{
    
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

void OsnThread::work()
{
    while (!s_isQuit) {
        sleep(1);
        printf("OsnThread::work!\n");

    }
}

void OsnThread::setId(oINT32 id)
{
    m_nId = id;
}

oINT32 OsnThread::getId()
{
    return m_nId;
}

