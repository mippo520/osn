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
#include<execinfo.h>

__thread stCoThreadInfo *OsnCoroutineManager::s_pThreadInfo = NULL;

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
    if (NULL != pCo)
    {
        pCo->setFunc(func);
        pCo->setState(eCS_Ready);
    }
    return unId;
}

const OSN_CO_ARG& OsnCoroutineManager::yield(const OSN_CO_ARG &arg)
{
    stCoThreadInfo *pInfo = getThreadInfo();
    
    if (!pInfo->isRunning())
    {
        printf("OsnCoroutineManager::yield Error! Not Running Coroutine! thread id = %lx\n", std::this_thread::get_id());
        pInfo->printInfo();
        printThreadInfo();
        return pInfo->getArg();
    }
    
    OsnCoroutine *pCo = m_arrCoroutine.getObject(pInfo->getRunning());
    if (NULL == pCo)
    {
        printf("OsnCoroutineManager::yield Error! Can not found running coroutine %u! thread id = %lx\n", pInfo->getRunning(), std::this_thread::get_id());
        pInfo->printInfo();
        printThreadInfo();
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

    if (pInfo->isRunning())
    {
        printf("OsnCoroutineManager::resume Error! Coroutine is already running! thread id = %lx\n", std::this_thread::get_id());
        pInfo->printInfo();
        printThreadInfo();
        return pInfo->getArg();
    }
    
    OsnCoroutine *pCo = m_arrCoroutine.getObject(unId);
    if (NULL == pCo)
    {
        printf("OsnCoroutineManager::resume Error! Can not found coroutine %u!\n", unId);
        pInfo->printInfo();
        printThreadInfo();
        return pInfo->getArg();
    }

    switch (pCo->getState())
    {
        case eCS_Ready:
            getcontext(pCo->getCtxPtr());
            pCo->createContext(pInfo->getContextPtr());
            pCo->setState(eCS_Running);
            pInfo->setRunning(unId);
            pInfo->setArg(arg);
            makecontext(pCo->getCtxPtr(), (void (*)())&OsnCoroutineManager::mainFunc, 1, this);
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
        {
            printf("OsnCoroutineManager::resume Error! Coroutine %d state error! threadId = %lx\n", unId, std::this_thread::get_id());
            pCo->printInfo();
            pInfo->printInfo();
            printThreadInfo();
            void *buffer[100];
            int nptrs;
            char **str;
            nptrs = backtrace(buffer, 100);
            str = backtrace_symbols(buffer, nptrs);
            for (int i = 0; i < nptrs; ++i) {
                printf("%s\n", str[i]);
            }
        }
            break;
    }
    return pInfo->getArg();
}


void OsnCoroutineManager::mainFunc(OsnCoroutineManager *pManager)
{
    if (NULL == pManager)
    {
        printf("OsnCoroutineManager::mainFunc Error! Coroutine manager is NULL\n");
        return;
    }
    
    stCoThreadInfo *pInfo = pManager->getThreadInfo();
    
    OsnCoroutine *pCo = pManager->m_arrCoroutine.getObject(pInfo->getRunning());
    if (NULL == pCo)
    {
        printf("OsnCoroutineManager::mainFunc Error! Can not found coroutine %u!\n", pInfo->getRunning());
        pInfo->printInfo();
        pManager->printThreadInfo();
        return;
    }
    OSN_CO_ARG arg = pCo->run(pInfo->getArg());
    pInfo = pManager->getThreadInfo();
    pInfo->setRunning(0);
    pInfo->setArg(arg);
    pManager->m_arrCoroutine.removeObj(pInfo->getRunning());
}

stCoThreadInfo* OsnCoroutineManager::getThreadInfo()
{
//    std::thread::id curThread = std::this_thread::get_id();
//    return &m_mapInfo[curThread];
    return s_pThreadInfo;
}

void OsnCoroutineManager::addThread()
{
    s_pThreadInfo = new stCoThreadInfo();
//    std::thread::id curThread = std::this_thread::get_id();
//    if (m_mapInfo.find(curThread) == m_mapInfo.end())
//    {
//        m_SpinLock.lock();
//        m_mapInfo[curThread];
//        m_SpinLock.unlock();
//    }
}

void OsnCoroutineManager::printThreadInfo()
{
//    printf("thread count is %d\n", m_mapInfo.size());
//    MAP_CO_THREAD_INFO_ITR itr = m_mapInfo.begin();
//    for (; itr != m_mapInfo.end(); itr++)
//    {
//        printf("thread id = %lx, info arrd = %x, running = %u \n", itr->first, &itr->second, itr->second.getRunning());
//        itr->second.getArg().printContext();
//    }
}


