//
//  osn_coroutine.cpp
//  osn
//
//  Created by zenghui on 17/4/10.
//  Copyright © 2017年 zenghui. All rights reserved.
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
    printf("coroutine info : id = %lu, state = %d\n", getId(), getState());
}



