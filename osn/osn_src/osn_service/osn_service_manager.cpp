//
//  osn_service_manager.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_service_manager.h"
#include "osn_service.h"

OsnServiceManager::OsnServiceManager()
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    
}

void OsnServiceManager::init()
{
    OsnArrManager::init();
}

oINT32 OsnServiceManager::send(oINT32 nTargetId, const OSN_SERVICE_MSG &msg)
{
    return pushMsg(nTargetId, msg);
}

oINT32 OsnServiceManager::pushMsg(oINT32 nTargetId, const OSN_SERVICE_MSG &msg)
{
    oINT32 nSession = 0;
    lock();
    OsnService *pService = getObject(nTargetId);
    if (NULL != pService)
    {
        nSession = pService->pushMsg(msg);
        if (!pService->getIsInGlobal())
        {
            pService->setIsInGlobal(true);
            m_queHadMsgIds.push(nTargetId);
        }
    }

    unlock();
    return nSession;
}

OsnService* OsnServiceManager::popWorkingService()
{
    OsnService *pService = NULL;
    lock();
    if (m_queHadMsgIds.size() > 0)
    {
        pService = getObject(m_queHadMsgIds.front());
        m_queHadMsgIds.pop();
    }
    unlock();
    return pService;
}

void OsnServiceManager::pushWarkingService(oINT32 nId)
{
    if (nId > 0)
    {
        lock();
        m_queHadMsgIds.push(nId);
        unlock();
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
    
    oUINT32 n = 1;
    for (oUINT32 i = 0; i < n; ++i)
    {
        if (!pService->dispatch())
        {
            return popWorkingService();
        }
        else if(0 == i && nWeight > 0)
        {
            n = pService->getMsgSize();
            n >>= nWeight;
        }
        
    }
    
    OsnService *pNextService = popWorkingService();
    if (NULL != pNextService)
    {
        pushWarkingService(pService->getId());
        pService = pNextService;
    }
    
    return pService;
}



