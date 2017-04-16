//
//  osn_coroutine_head.h
//  osn
//
//  Created by liqing on 17/4/10.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_coroutine_head_h
#define osn_coroutine_head_h

#include <functional>
#include <ucontext.h>
#include "osn.h"
#include <map>
#include <thread>
#include "osn_prepared_statement.h"

typedef OsnPreparedStatement OSN_CO_ARG;

typedef std::function<OsnPreparedStatement(oUINT32 co, const OsnPreparedStatement &)> OSN_COROUTINE_FUNC;

enum eCoroutineState
{
    eCS_None = 0,
    eCS_Ready,
    eCS_Running,
    eCS_Suspend,
};


struct stCoThreadInfo {

    MEMBER_VALUE(oUINT32, Running);
    MEMBER_VALUE(OSN_CO_ARG, Arg);
    MEMBER_VALUE(std::thread::id, ThreadId);
    ucontext_t m_MainCtx;
    
    stCoThreadInfo()
    : m_Running(0)
    {
        
    }
    
    oBOOL isRunning()
    {
        return 0 != m_Running;
    }
    
    ucontext_t* getContextPtr()
    {
        return &m_MainCtx;
    }
};

typedef std::map<std::thread::id, stCoThreadInfo> MAP_CO_THREAD_INFO;
typedef MAP_CO_THREAD_INFO::iterator MAP_CO_THREAD_INFO_ITR;

#endif /* osn_coroutine_head_h */
