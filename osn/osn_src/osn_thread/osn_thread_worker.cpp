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

OsnWorkerThread::OsnWorkerThread()
{
    
}

OsnWorkerThread::~OsnWorkerThread()
{
    
}


void OsnWorkerThread::work()
{

    while (!s_isQuit) {
        sleep(1);
        printf("OsnWorkerThread::work! 1.0 id = %d\n", getId());
    }
}

