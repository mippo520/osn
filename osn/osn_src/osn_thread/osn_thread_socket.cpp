//
//  osn_thread_socket.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_thread_socket.h"
#include "osn_socket_manager.h"
#include "osn_start.h"
#include <unistd.h>

OsnSocketThread::OsnSocketThread()
{
    m_pThread = new std::thread(&OsnSocketThread::onWork, this);
}

OsnSocketThread::~OsnSocketThread()
{
    
}

void OsnSocketThread::onWork()
{
    for (;;)
    {
        oINT32 nRet = g_SocketManager.poll();
        if (0 == nRet)
        {
            break;
        }
        else if (nRet < 0)
        {
            if (g_OsnStart.checkAbort()) {
                break;
            }
            continue;
        }
        
        g_OsnStart.wakeup(0);
    }
}
