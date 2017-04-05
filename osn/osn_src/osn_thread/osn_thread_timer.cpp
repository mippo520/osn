//
//  osn_thread.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_thread_timer.h"
#include <unistd.h>
#include "osn_start.h"

OsnTimerThread::OsnTimerThread()
{
    
}

OsnTimerThread::~OsnTimerThread()
{
    
}

void OsnTimerThread::work()
{
    for (; ; ) {
        if (g_OsnStart.checkAbort()) {
            break;
        }

        printf("OsnTimerThread::work! 2.5 id = %d\n", getId());
        usleep(2500000);
    }
    
    s_isQuit = true;
}

