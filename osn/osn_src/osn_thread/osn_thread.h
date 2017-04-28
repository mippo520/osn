//
//  osn_thread.hpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_thread_hpp
#define osn_thread_hpp
#include "osn.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class OsnThread {
public:
    static std::condition_variable s_Cond;
    static oINT32 s_SleepCount;
protected:
    static oBOOL s_isQuit;
    static std::mutex s_Mutex;
public:
    OsnThread();
    virtual ~OsnThread();
public:
    void init(oBOOL isMainWait = true);
protected:
    std::thread *m_pThread;
    MEMBER_VALUE(oINT32, Weight)
    MEMBER_VALUE(oUINT32, Id)
};

#endif /* osn_thread_hpp */
