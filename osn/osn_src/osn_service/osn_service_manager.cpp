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

oINT32 OsnServiceManager::getId()
{
    return 0;
}

void OsnServiceManager::addService(oINT32 nId, OsnService *pService)
{
    oUINT32 nCurCount = m_vecServices.size();
    if (nId >= nCurCount) {
        m_vecServices.resize(nCurCount * 2);
    }
    m_vecServices[nId] = pService;
}

void OsnServiceManager::init()
{
    if (0 == m_vecServices.size()) {
        m_vecServices.resize(s_nServiceCountBegin);
    }
}

void OsnServiceManager::removeService(oINT32 nId)
{
    if (m_vecServices.size() > nId) {
        OsnService *pService = m_vecServices[nId];
        pService->exit();
        SAFE_DELETE(pService);
        m_vecServices[nId] = NULL;
    }
}


