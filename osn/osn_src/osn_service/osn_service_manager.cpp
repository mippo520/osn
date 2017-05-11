//
//  osn_service_manager.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_service_manager.h"
#include "osn_service.h"
#include "osn_coroutine_manager.h"
#include <iostream>

__thread oUINT32 OsnServiceManager::s_unThreadCurService = 0;

OsnServiceManager::OsnServiceManager()
    : m_u64DistroyCount(0)
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    
}

oUINT32 OsnServiceManager::send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
    return sendMessage(addr, 0, type, 0, &msg);
}

const OsnPreparedStatement& OsnServiceManager::call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
    oUINT32 unSession = sendMessage(addr, getCurService(), type, 0, &msg);
    OSN_CO_ARG arg;
	arg.setUInt32(0, unSession);
	arg.pushBackInt32(OsnService::eYT_Call);
    return g_CorotineManager.yield(arg);
}


void OsnServiceManager::ret(const OsnPreparedStatement &msg)
{
//     OSN_CO_ARG arg;
//     arg.setInt32(0, OsnService::eYT_Return);
//     arg.setPoint(1, &msg);
	msg.pushBackInt32(OsnService::eYT_Return);
    g_CorotineManager.yield(msg);
}

void OsnServiceManager::exit()
{
    OsnPreparedStatement stmt;
    stmt.pushBackInt32(OsnService::eYT_Quit);
    g_CorotineManager.yield(stmt);
}

void OsnServiceManager::wait(oUINT32 unId)
{
    oUINT32 unSession = genId();
    OsnPreparedStatement stmt;
    stmt.pushBackUInt32(unSession);
    stmt.pushBackInt32(OsnService::eYT_Sleep);
    g_CorotineManager.yield(stmt);
    
    stmt.clear();
    stmt.pushBackUInt32(unSession);
    stmt.pushBackInt32(OsnService::eYT_CleanSleep);
    g_CorotineManager.yield(stmt);
}

oBOOL OsnServiceManager::wakeup(oUINT32 unId)
{
    OsnPreparedStatement stmt;
    stmt.pushBackUInt32(unId);
    stmt.pushBackInt32(OsnService::eYT_Wakeup);
    const OsnPreparedStatement &ret = g_CorotineManager.yield(stmt);
    return ret.getBool(0);
}

oUINT32 OsnServiceManager::genId()
{
    return sendMessage(getCurService(), 0, ePType_None, 0, NULL);
}

void OsnServiceManager::init()
{
    OsnArrManager::init();
}

oUINT32 OsnServiceManager::sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement *pMsg)
{
	stServiceMessage *pServiceMsg = new stServiceMessage(pMsg);
	pServiceMsg->unSession = unSession;
	pServiceMsg->unSource = unSource;
	pServiceMsg->nType = type;

    return pushMsg(unTargetId, pServiceMsg);
}

oUINT32 OsnServiceManager::pushMsg(oUINT32 unTargetId, stServiceMessage *pMsg)
{
    oUINT32 unSession = 0;
    OsnService *pService = getObject(unTargetId);
    if (NULL != pService)
    {
        unSession = pService->pushMsg(pMsg);
    }
    else
    {
        printf("OsnServiceManager::pushMsg Error! can not found service, id is %lu\n", unTargetId);
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

void OsnServiceManager::registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC funcPtr)
{
	oUINT32 unId = getCurService();
	OsnService *pService = getObject(unId);
	if (NULL != pService)
	{
		pService->registDispatchFunc(nPType, funcPtr);
	}
}

void OsnServiceManager::unregistDispatchFunc(oINT32 nPType)
{
    oUINT32 unId = getCurService();
    OsnService *pService = getObject(unId);
    if (NULL != pService)
    {
        pService->unregistDispatchFunc(nPType);
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
            setCurService(0);
            m_CountLock.lock();
            ++m_u64DistroyCount;
            if (0 == m_u64DistroyCount % 100000) {
                printf("destroy count is %llu\n", m_u64DistroyCount);
            }
            m_CountLock.unlock();
            removeObj(pService->getId());
            return popWorkingService();
        }
        else
        {
            if (!bRet)
            {
                setCurService(0);
                return popWorkingService();
            }
            
			if(0 == i && nWeight > 0)
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
    if (NULL == pService) {
        pService = NULL;
    }
    return pService;
}

void OsnServiceManager::addThread()
{
//    std::thread::id curThread = std::this_thread::get_id();
//    m_CurServiceLock.lock();
//    m_mapThreadCurService[curThread] = 0;
//    m_CurServiceLock.unlock();
    s_unThreadCurService = 0;
}

void OsnServiceManager::setCurService(oUINT32 unId)
{
//    std::thread::id curThread = std::this_thread::get_id();
//    m_mapThreadCurService[curThread] = unId;
    s_unThreadCurService = unId;
}

oUINT32 OsnServiceManager::getCurService()
{
//    std::thread::id curThread = std::this_thread::get_id();
//    return m_mapThreadCurService[curThread];
    return s_unThreadCurService;
}

void OsnServiceManager::printThreadInfo()
{
//    std::map<std::thread::id, oUINT32>::iterator itr = m_mapThreadCurService.begin();
//    for (; itr != m_mapThreadCurService.end(); itr++)
//    {
//        printf("threadid = %lx, serviceId = %u | ", itr->first, itr->second);
//    }
//    printf("\n");
}



