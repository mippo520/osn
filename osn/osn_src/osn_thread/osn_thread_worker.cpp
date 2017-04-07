//
//  osn_thread.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_thread_worker.h"
#include <unistd.h>
#include <stdio.h>
#include <mutex>
#include "osn_service_manager.h"

OsnWorkerThread::OsnWorkerThread()
{
    
}

OsnWorkerThread::~OsnWorkerThread()
{
    
}


void OsnWorkerThread::work()
{
    OsnService *pService = NULL;
    while (!s_isQuit) {
        pService = g_ServiceManager.dispatchMessage(pService, getWeight());
        if (NULL == pService) {
            std::unique_lock<std::mutex> lock(s_Mutex);
            ++s_SleepCount;
            s_Cond.wait(lock);
            --s_SleepCount;
            lock.unlock();
        }
    }
}

