//
//  osn_coroutine.cpp
//  osn
//
//  Created by liqing on 17/4/10.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_coroutine.h"
#include <stdlib.h>

OsnCoroutine::OsnCoroutine()
    : m_Id(0)
    , m_State(eCS_Ready)
{
}

OsnCoroutine::~OsnCoroutine()
{
    
}


void OsnCoroutine::init()
{
}

void OsnCoroutine::exit()
{
    
}

void OsnCoroutine::createContext(ucontext_t *pNextCtx)
{
    getcontext(&m_Ctx);

    m_Ctx.uc_stack.ss_sp = m_szStack;
    m_Ctx.uc_stack.ss_size = s_nStackSize;
    m_Ctx.uc_link = pNextCtx;
}

void OsnCoroutine::setLastContext(ucontext_t *pLastCtx)
{
    m_Ctx.uc_link = pLastCtx;
}

ucontext_t* OsnCoroutine::getCtxPtr()
{
    return &m_Ctx;
}


OSN_CO_ARG OsnCoroutine::run(const OSN_CO_ARG &arg)
{
    return m_Func(getId(), arg);
}

void OsnCoroutine::printInfo()
{
    printf("coroutine info : id = %u, state = %d\n", getId(), getState());
}



