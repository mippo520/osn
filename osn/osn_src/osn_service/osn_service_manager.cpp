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
#include "osn_service_factory.h"
#include <iostream>

const IOsnService *g_Service = &g_ServiceManager;
__thread oUINT32 OsnServiceManager::s_unThreadCurService = 0;

OsnServiceManager::OsnServiceManager()
    : m_u64DistroyCount(0)
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.begin();
    for (; itr != m_mapServiceFactory.end(); ++itr)
    {
        OsnServiceFactory *pServiceFactory = itr->second;
        SAFE_DELETE(pServiceFactory);
    }
    m_mapServiceFactory.clear();
}

void OsnServiceManager::addServiceFactory(const std::string &strName, OsnServiceFactory *pFactory)
{
    if (NULL == pFactory)
    {
        printf("OsnServiceManager::addServiceFactory Error! %s factory is NULL!\n", strName.c_str());
        assert(false);
    }
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.find(strName);
    if (itr == m_mapServiceFactory.end())
    {
        m_mapServiceFactory[strName] = pFactory;
    }
    else
    {
        printf("OsnServiceManager::addServiceFactory Error! %s factory is exist!\n", strName.c_str());
    }
}

oUINT32 OsnServiceManager::startService(const std::string &strServiceName)
{
    oUINT32 unId = 0;
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.find(strServiceName);
    if (itr != m_mapServiceFactory.end())
    {
        unId = addObj(itr->second->create());
    }
    else
    {
        printf("OsnServiceManager::startService Error! %s can not found!", strServiceName.c_str());
    }

    return unId;
}

oUINT32 OsnServiceManager::genId()
{
    return sendMessage(getCurService(), 0, ePType_None, 0, NULL);
}

void OsnServiceManager::init()
{
    OsnArrManager::init();
}

oUINT32 OsnServiceManager::sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement *pMsg) const
{
	stServiceMessage *pServiceMsg = new stServiceMessage(pMsg);
	pServiceMsg->unSession = unSession;
	pServiceMsg->unSource = unSource;
	pServiceMsg->nType = type;

    return pushMsg(unTargetId, pServiceMsg);
}

oUINT32 OsnServiceManager::pushMsg(oUINT32 unTargetId, stServiceMessage *pMsg) const
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

void OsnServiceManager::pushWarkingService(oUINT32 unId) const
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

oUINT32 OsnServiceManager::getCurService() const
{
//    std::thread::id curThread = std::this_thread::get_id();
//    return m_mapThreadCurService[curThread];
    return s_unThreadCurService;
}




