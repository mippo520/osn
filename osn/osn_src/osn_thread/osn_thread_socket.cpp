//
//  osn_thread_socket.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_thread_socket.h"

OsnSocketThread::OsnSocketThread()
{
    m_pThread = new std::thread(&OsnSocketThread::onWork, this);
}

OsnSocketThread::~OsnSocketThread()
{
    
}

void OsnSocketThread::onWork()
{
    
}
