//
//  osn_coroutine.cpp
//  osn
//
//  Created by zenghui on 17/4/10.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_coroutine.h"

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


SHARED_PTR_STMT OsnCoroutine::run(SHARED_PTR_STMT arg)
{
    return m_Func(getId(), arg);
}

void OsnCoroutine::printInfo()
{
    printf("coroutine info : id = %llu, state = %d\n", getId(), getState());
}



