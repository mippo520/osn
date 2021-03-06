//
//  osn_coroutine_head.h
//  osn
//
//  Created by zenghui on 17/4/10.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_coroutine_head_h
#define osn_coroutine_head_h

#include <functional>
#include <map>
#include <thread>
#include <ucontext.h>
#include "osn_prepared_statement.h"
#include "osn_common.h"

enum eCoroutineState
{
    eCS_None = 0,
    eCS_Ready,
    eCS_Running,
    eCS_Suspend,
};


struct stCoThreadInfo {

    MEMBER_VALUE(ID_COROUTINE, Running);
    ucontext_t m_MainCtx;
    
    stCoThreadInfo()
        : m_Running(0)
        , m_pArg(SHARED_PTR_STMT_NONE)
    {
    }
    
    void setArg(SHARED_PTR_STMT arg)
    {
        m_pArg = arg;
    }
    
    SHARED_PTR_STMT getArg()
    {
        return m_pArg;
    }
    
    oBOOL isRunning()
    {
        return 0 != m_Running;
    }
    
    ucontext_t* getContextPtr()
    {
        return &m_MainCtx;
    }
    
    void printInfo()
    {
        printf("stCoThreadInfo ==========> addr = %llx, running = %llu, \n", (oUINT64)this, m_Running);
        m_pArg->printContext("stCoThreadInfo printInfo");
    }
private:
    SHARED_PTR_STMT m_pArg;
};

//typedef std::map<std::thread::id, stCoThreadInfo> MAP_CO_THREAD_INFO;
//typedef MAP_CO_THREAD_INFO::iterator MAP_CO_THREAD_INFO_ITR;

#endif /* osn_coroutine_head_h */
