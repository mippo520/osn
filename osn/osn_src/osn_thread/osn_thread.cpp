//
//  osn_thread.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_thread.h"
#include <thread>
#include <unistd.h>

oBOOL OsnThread::s_isQuit = false;
std::mutex OsnThread::s_Mutex;
std::condition_variable OsnThread::s_Cond;
oINT32 OsnThread::s_SleepCount = 0;

OsnThread::OsnThread()
    : m_pThread(NULL)
	, m_Id(0)
{
    
}

OsnThread::~OsnThread()
{
	SAFE_DELETE(m_pThread);
}

void OsnThread::init(oBOOL isMainWait)
{
	if (NULL != m_pThread)
	{
		if (isMainWait) {
			m_pThread->join();
		}
		else
		{
			m_pThread->detach();
		}
	}
}


