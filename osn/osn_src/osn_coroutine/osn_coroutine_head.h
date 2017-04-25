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
#include <ucontext.h>
#include "osn.h"
#include <map>
#include <thread>
#include "osn_prepared_statement.h"

typedef OsnPreparedStatement OSN_CO_ARG;

typedef std::function<OSN_CO_ARG(oUINT32 co, const OSN_CO_ARG&)> OSN_COROUTINE_FUNC;

enum eCoroutineState
{
    eCS_None = 0,
    eCS_Ready,
    eCS_Running,
    eCS_Suspend,
};


struct stCoThreadInfo {

    MEMBER_VALUE(oUINT32, Running);
    ucontext_t m_MainCtx;
    
    stCoThreadInfo()
        : m_Running(0)
    {
        m_pArg = &m_ArgCache;
    }
    
    void setArg(const OSN_CO_ARG *pArg)
    {
        if(NULL != pArg)
        {
            m_pArg = pArg;
        }
        else
        {
            m_pArg = &m_ArgCache;
        }
    }
    
    const OSN_CO_ARG& getArg()
    {
        return *m_pArg;
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
        printf("stCoThreadInfo ==========> addr = %x, running = %lu, \n", this, m_Running);
        m_pArg->printContext();
    }
private:
    const OSN_CO_ARG *m_pArg;
    OSN_CO_ARG m_ArgCache;
};

typedef std::map<std::thread::id, stCoThreadInfo> MAP_CO_THREAD_INFO;
typedef MAP_CO_THREAD_INFO::iterator MAP_CO_THREAD_INFO_ITR;

#endif /* osn_coroutine_head_h */
