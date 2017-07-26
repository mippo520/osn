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
#include "I_osn_service_factory.h"
#include <iostream>
#include <dlfcn.h>

const IOsnService *g_Service = &g_ServiceManager;
__thread ID_SERVICE OsnServiceManager::s_u64ThreadCurService = 0;

OsnServiceManager::OsnServiceManager()
    : m_u64DistroyCount(0)
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.begin();
    for (; itr != m_mapServiceFactory.end(); ++itr)
    {
        IServiceFactory *pServiceFactory = itr->second;
        SAFE_DELETE(pServiceFactory);
    }
    m_mapServiceFactory.clear();
    
    if (m_vecDylibHandles.size() > 0)
    {
        for (oINT64 i = m_vecDylibHandles.size() - 1; i >= 0; --i)
        {
            dlclose(m_vecDylibHandles[i]);
        }
        m_vecDylibHandles.clear();
    }
}

oBOOL OsnServiceManager::isServiceFactoryExist(const std::string &strName)
{
    oBOOL bRet = false;
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.find(strName);
    if (itr != m_mapServiceFactory.end())
    {
        bRet = true;
    }
    return bRet;
}

void OsnServiceManager::addServiceFactory(const std::string &strName, IServiceFactory *pFactory)
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
        assert(0);
    }
}

void OsnServiceManager::pushDylibHandle(void *handle)
{
    if (NULL != handle)
    {
        m_vecDylibHandles.push_back(handle);
    }
    else
    {
        printf("OsnServiceManager::pushDylibHandle Error! handle is NULL!\n");
        assert(false);
    }
}

ID_SERVICE OsnServiceManager::startService(const std::string &strServiceName, const OsnPreparedStatement &stmt)
{
    ID_SERVICE unId = 0;
    MAP_SERVICE_FACTORY_ITR itr = m_mapServiceFactory.find(strServiceName);
    if (itr != m_mapServiceFactory.end())
    {
        OsnService *pService = itr->second->create();
        unId = addObj(pService);
        pService->init(stmt);
    }

    return unId;
}

ID_SESSION OsnServiceManager::genId()
{
    return sendMessage(getCurService(), 0, ePType_None, 0);
}

void OsnServiceManager::init()
{
    OsnArrManager::init();
}

ID_SESSION OsnServiceManager::sendMessage(ID_SERVICE unTargetId, ID_SERVICE unSource, oINT32 type, ID_SESSION unSession, const OsnPreparedStatement &msg) const
{
    ID_SESSION unRet = 0;
    OsnService *pService = getObject(unTargetId);
    if (NULL != pService)
    {
        stServiceMessage *pServiceMsg = new stServiceMessage(msg);
        pServiceMsg->unSession = unSession;
        pServiceMsg->unSource = unSource;
        pServiceMsg->nType = type;
        unRet = pService->pushMsg(pServiceMsg);
    }
    else
    {
        printf("OsnServiceManager::pushMsg Error! can not found service, from %llu to %llu, type is %d\n", unSource, unTargetId, type);
        assert(0);
    }
    
    return unRet;
}

OsnService* OsnServiceManager::popWorkingService()
{
    OsnService *pService = NULL;
    m_QueSpinLock.lock();
    if (m_queHadMsgIds.size() > 0)
    {
        ID_SERVICE unServiceId = m_queHadMsgIds.front();
        pService = getObject(unServiceId);
        m_queHadMsgIds.pop();
    }

    m_QueSpinLock.unlock();
    return pService;
}

void OsnServiceManager::pushWarkingService(ID_SERVICE unId) const
{
    if (unId > 0)
    {
        m_QueSpinLock.lock();
        m_queHadMsgIds.push(unId);
        m_QueSpinLock.unlock();
    }
}

void OsnServiceManager::registDispatchFunc(oINT32 nPType, DISPATCH_FUNC funcPtr)
{
	ID_SERVICE unId = getCurService();
	OsnService *pService = getObject(unId);
	if (NULL != pService)
	{
		pService->registDispatchFunc(nPType, funcPtr);
	}
}

void OsnServiceManager::unregistDispatchFunc(oINT32 nPType)
{
    ID_SERVICE unId = getCurService();
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
    s_u64ThreadCurService = 0;
}

void OsnServiceManager::setCurService(ID_SERVICE unId)
{
//    std::thread::id curThread = std::this_thread::get_id();
//    m_mapThreadCurService[curThread] = unId;
    s_u64ThreadCurService = unId;
}

ID_SERVICE OsnServiceManager::getCurService() const
{
//    std::thread::id curThread = std::this_thread::get_id();
//    return m_mapThreadCurService[curThread];
    return s_u64ThreadCurService;
}




