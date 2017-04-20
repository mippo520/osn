//
//  osn_service_manager.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_service_manager.h"
#include "osn_service.h"
#include "osn_coroutine_manager.h"
#include <iostream>

OsnServiceManager::OsnServiceManager()
    : m_u64DistroyCount(0)
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    
}

oUINT32 OsnServiceManager::send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
    return sendMessage(addr, 0, type, 0, msg);
}

const OsnPreparedStatement& OsnServiceManager::call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
    oUINT32 unSession = sendMessage(addr, getCurService(), type, 0, msg);
    OSN_CO_ARG arg;
    arg.setInt32(0, OsnService::eYT_Call);
    arg.setUInt32(1, unSession);
    return g_CorotineManager.yield(arg);
}


void OsnServiceManager::ret(const OsnPreparedStatement &msg)
{
    OSN_CO_ARG arg;
    arg.setInt32(0, OsnService::eYT_Return);
    arg.setPoint(1, &msg);
    g_CorotineManager.yield(arg);
}

void OsnServiceManager::exit()
{
    OsnPreparedStatement stmt;
    stmt.setInt32(0, OsnService::eYT_Quit);
    g_CorotineManager.yield(stmt);
}

void OsnServiceManager::init()
{
    OsnArrManager::init();
}

oUINT32 OsnServiceManager::sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement &msg)
{
	stServiceMessage serviceMsg;
	serviceMsg.stmt = msg;
	serviceMsg.unSession = unSession;
	serviceMsg.unSource = unSource;
	serviceMsg.nType = type;

    return pushMsg(unTargetId, serviceMsg);
}

oUINT32 OsnServiceManager::pushMsg(oUINT32 unTargetId, stServiceMessage &msg)
{
    oUINT32 unSession = 0;
    OsnService *pService = getObject(unTargetId);
    if (NULL != pService)
    {
        unSession = pService->pushMsg(msg);
    }

    return unSession;
}

OsnService* OsnServiceManager::popWorkingService()
{
    OsnService *pService = NULL;
    m_QueSpinLock.lock();
    if (m_queHadMsgIds.size() > 0)
    {
        oUINT32 unServiceId = m_queHadMsgIds.front();
        pService = getObject(unServiceId);
        m_queHadMsgIds.pop();
    }

    m_QueSpinLock.unlock();
    return pService;
}

void OsnServiceManager::pushWarkingService(oUINT32 unId)
{
    if (unId > 0)
    {
        m_QueSpinLock.lock();
        m_queHadMsgIds.push(unId);
        m_QueSpinLock.unlock();
    }
}

OsnService* OsnServiceManager::dispatchMessage(OsnService* pService, oINT32 nWeight)
{
    if (NULL == pService)
    {
        pService = popWorkingService();
        if (NULL == pService)
        {
            return pService;
        }
    }
    
    setCurService(pService->getId());
    oUINT32 n = 1;
    for (oUINT32 i = 0; i < n; ++i)
    {
        oINT32 nMsgType = 0;
        oBOOL bRet = pService->dispatchMessage(nMsgType);
        if (OsnService::eYT_Quit == nMsgType) {
            removeObj(pService->getId());
            setCurService(0);
            m_CountLock.lock();
            ++m_u64DistroyCount;
            printf("destroy count is %llu\n", m_u64DistroyCount);
            m_CountLock.unlock();
            return popWorkingService();
        }
        else
        {
            if (!bRet)
            {
                setCurService(0);
                return popWorkingService();
            }
            else if(0 == i && nWeight > 0)
            {
                n = pService->getMsgSize();
                n >>= nWeight;
            }
        }
    }
    
    OsnService *pNextService = popWorkingService();
    if (NULL != pNextService)
    {
        pushWarkingService(pService->getId());
        pService = pNextService;
    }
    
    setCurService(0);
    return pService;
}

void OsnServiceManager::addThread()
{
    std::thread::id curThread = std::this_thread::get_id();
    m_CurServiceLock.lock();
    m_mapThreadCurService[curThread] = 0;
    m_CurServiceLock.unlock();
}

void OsnServiceManager::setCurService(oUINT32 unId)
{
    std::thread::id curThread = std::this_thread::get_id();
    m_mapThreadCurService[curThread] = unId;
}

oUINT32 OsnServiceManager::getCurService()
{
    std::thread::id curThread = std::this_thread::get_id();
    return m_mapThreadCurService[curThread];
}

void OsnServiceManager::printThreadInfo()
{
    std::map<std::thread::id, oUINT32>::iterator itr = m_mapThreadCurService.begin();
    for (; itr != m_mapThreadCurService.end(); itr++)
    {
        printf("threadid = %lx, serviceId = %u | ", itr->first, itr->second);
    }
    printf("\n");
}



