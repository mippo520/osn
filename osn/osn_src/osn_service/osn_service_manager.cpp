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

oUINT32 OsnServiceManager::send(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement &msg)
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
        if (!pService->getIsInGlobal())
        {
            pService->setIsInGlobal(true);
			lock();
            m_queHadMsgIds.push(unTargetId);
			unlock();
		}
    }

    return unSession;
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

void OsnServiceManager::pushWarkingService(oUINT32 unId)
{
    if (unId > 0)
    {
        lock();
        m_queHadMsgIds.push(unId);
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
        if (!pService->dispatchMessage())
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



