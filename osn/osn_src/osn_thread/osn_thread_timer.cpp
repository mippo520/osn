//
//  osn_thread.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_thread_timer.h"
#include <unistd.h>
#include "osn_start.h"
#include "osn_socket_manager.h"

OsnTimerThread::OsnTimerThread()
{
	m_pThread = new std::thread(&OsnTimerThread::onWork, this);
}

OsnTimerThread::~OsnTimerThread()
{
    
}

void OsnTimerThread::onWork()
{
	for (;;) {
        if (g_OsnStart.checkAbort()) {
            break;
        }
        usleep(2500);
        g_OsnStart.wakeup(0);
    }
    
    g_SocketManager.exit();
    s_isQuit = true;
}

