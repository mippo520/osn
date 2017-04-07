//
//  osn_service_manager.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_service_manager.h"
#include "osn_service.h"

oINT32 OsnServiceManager::s_nServiceIdx = 0;

OsnServiceManager::OsnServiceManager()
{
    
}

OsnServiceManager::~OsnServiceManager()
{
    
}

oINT32 OsnServiceManager::createId()
{
    oINT32 nId = 0;
    lock();

    if (m_queIds.size() > 0) {
        nId = m_queIds.front();
        m_queIds.pop();
    }
    else
    {
        nId = ++s_nServiceIdx;
    }
    
    unlock();
    return nId;
}

void OsnServiceManager::addService(oINT32 nId, OsnService *pService)
{
    lock();
    oUINT32 nCurCount = m_vecServices.size();
    if (nId >= nCurCount) {
        if (0 == nCurCount) {
            nCurCount = s_nServiceCountBegin;
        }
        m_vecServices.resize(nCurCount * 2);
    }
    m_vecServices[nId] = pService;
    unlock();
}

void OsnServiceManager::init()
{
    if (0 == m_vecServices.size()) {
        m_vecServices.resize(s_nServiceCountBegin);
    }
}

void OsnServiceManager::removeService(oINT32 nId)
{
    lock();
    if (m_vecServices.size() > nId) {
        OsnService *pService = m_vecServices[nId];
        pService->exit();
        SAFE_DELETE(pService);
        m_vecServices[nId] = NULL;
        m_queIds.push(nId);
    }
    unlock();
}

void OsnServiceManager::send(oINT32 nTargetId, oINT32 nValue)
{
    OsnMessage msg;
    msg.setInt(nValue);
    pushMsg(nTargetId, msg);
}

OsnService* OsnServiceManager::getService(oINT32 nId)
{
    OsnService *pService = NULL;
    if (nId < m_vecServices.size()) {
        pService = m_vecServices[nId];
    }
    return pService;
}

void OsnServiceManager::pushMsg(oINT32 nTargetId, const OsnMessage &msg)
{
    lock();
    OsnService *pService = getService(nTargetId);
    if (NULL == pService) {
        return;
    }
    
    pService->pushMsg(msg);
    if (!pService->getIsInGlobal()) {
        pService->setIsInGlobal(true);
        m_queHadMsgIds.push(nTargetId);
    }
    unlock();
}

OsnService* OsnServiceManager::popWorkingService()
{
    OsnService *pService = NULL;
    lock();
    if (m_queHadMsgIds.size() > 0) {
        pService = getService(m_queHadMsgIds.front());
        m_queHadMsgIds.pop();
    }
    unlock();
    return pService;
}

void OsnServiceManager::pushWarkingService(oINT32 nId)
{
    if (nId > 0) {
        lock();
        m_queHadMsgIds.push(nId);
        unlock();
    }
}

OsnService* OsnServiceManager::dispatchMessage(OsnService* pService, oINT32 nWeight)
{
    if (NULL == pService) {
        pService = popWorkingService();
        if (NULL == pService) {
            return pService;
        }
    }
    
    oUINT32 n = 1;
    for (oUINT32 i = 0; i < n; ++i) {
        if (!pService->dispatch()) {
            return popWorkingService();
        }
        else if(0 == i && nWeight > 0)
        {
            n = pService->getMsgSize();
            n >>= nWeight;
        }
        
    }
    
    OsnService *pNextService = popWorkingService();
    if (NULL != pNextService) {
        pushWarkingService(pService->getId());
        pService = pNextService;
    }
    
    return pService;
}

void OsnServiceManager::lock()
{
    m_Mutex.lock();
}

void OsnServiceManager::unlock()
{
    m_Mutex.unlock();
}


