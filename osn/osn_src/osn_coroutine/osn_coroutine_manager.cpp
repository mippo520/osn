//
//  osn_coroutine_manager.cpp
//  osn
//
//  Created by liqing on 17/4/10.
//  Copyright © 2017年 liqing. All rights reserved.
//


#include "osn_coroutine_manager.h"
#include "osn_coroutine.h"
#include <stdio.h>

OsnCoroutineManager::OsnCoroutineManager()
{
}

OsnCoroutineManager::~OsnCoroutineManager()
{
    
}

oUINT32 OsnCoroutineManager::create(const OSN_COROUTINE_FUNC &func)
{
    oUINT32 unId = m_arrCoroutine.makeObj<OsnCoroutine>();
    OsnCoroutine *pCo = m_arrCoroutine.getObject(unId);
    if (NULL != pCo) {
        pCo->setFunc(func);
        pCo->setState(eCS_Ready);
    }
    return unId;
}

const OSN_CO_ARG& OsnCoroutineManager::yield(const OSN_CO_ARG &arg)
{
    stCoThreadInfo *pInfo = getThreadInfo();
    
    if (!pInfo->isRunning()) {
        return pInfo->getArg();
    }
    
    OsnCoroutine *pCo = m_arrCoroutine.getObject(pInfo->getRunning());
    if (NULL == pCo) {
        return pInfo->getArg();
    }

    pCo->setState(eCS_Suspend);
    pInfo->setRunning(0);
    pInfo->setArg(arg);
    swapcontext(pCo->getCtxPtr(), pInfo->getContextPtr());
    pInfo = getThreadInfo();
    return pInfo->getArg();
}

const OSN_CO_ARG& OsnCoroutineManager::resume(oUINT32 unId, const OSN_CO_ARG &arg)
{
    stCoThreadInfo *pInfo = getThreadInfo();

    if (pInfo->isRunning()) {
        return pInfo->getArg();
    }
    
    OsnCoroutine *pCo = m_arrCoroutine.getObject(unId);
    if (NULL == pCo) {
        return pInfo->getArg();
    }

    switch (pCo->getState()) {
        case eCS_Ready:
            getcontext(pCo->getCtxPtr());
            pCo->createContext(pInfo->getContextPtr());
            pCo->setState(eCS_Running);
            pInfo->setRunning(unId);
            pInfo->setArg(arg);
            makecontext(pCo->getCtxPtr(), (void (*)())OsnCoroutineManager::mainFunc, 1, this);
            swapcontext(pInfo->getContextPtr(), pCo->getCtxPtr());
            break;
        case eCS_Suspend:
            pCo->setState(eCS_Running);
            pInfo->setRunning(unId);
            pInfo->setArg(arg);
            pCo->setLastContext(pInfo->getContextPtr());
            swapcontext(pInfo->getContextPtr(), pCo->getCtxPtr());
            break;
        default:

            break;
    }
    return pInfo->getArg();
}


void OsnCoroutineManager::mainFunc(OsnCoroutineManager *pManager)
{
    if (NULL == pManager) {
        return;
    }
    
    stCoThreadInfo *pInfo = pManager->getThreadInfo();
    
    OsnCoroutine *pCo = pManager->m_arrCoroutine.getObject(pInfo->getRunning());
    if (NULL == pCo) {
        return;
    }
    OSN_CO_ARG arg = pCo->run(pInfo->getArg());
    pInfo = pManager->getThreadInfo();
    pInfo->setArg(arg);
    pManager->m_arrCoroutine.removeObj(pInfo->getRunning());
    pInfo->setRunning(0);
}

stCoThreadInfo* OsnCoroutineManager::getThreadInfo()
{
    std::thread::id curThread = std::this_thread::get_id();
    return &m_mapInfo[curThread];
}


