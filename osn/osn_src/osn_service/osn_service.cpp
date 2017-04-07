//
//  osn_service.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_service.h"
#include "osn_service_manager.h"
#include <set>


OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_nCount(0)
{
    
}

OsnService::~OsnService()
{
    
}

void OsnService::exit()
{
    printf("OsnService::exit id = %d\n", getId());
}

void OsnService::init()
{
    printf("OsnService::init id = %d\n", getId());
}

oBOOL OsnService::dispatch()
{
    if (m_queMsg.size() > 0) {
        OsnMessage &msg = m_queMsg.front();
        oINT32 nValue = msg.getInt();
        m_queMsg.pop();

        static std::set<oINT32> setInt;
        static std::mutex m_Mutex;

        if (nValue != getId())
        {
            printf("OsnService::dispatch error! id = %d,repeat value = %d\n", getId(), nValue);
        }
        printf("OsnService::dispatch id = %d, value = %d\n", getId(), nValue);

        if (m_nCount < 10000) {
            for (oINT32 i = 1; i <= 100; ++i, ++m_nCount) {
                g_ServiceManager.send(i, i);
            }
        }

        return true;
    }
    else{
        return false;
    }
}

void OsnService::pushMsg(const OsnMessage &msg)
{
//    printf("OsnService::pushMsg id = %d, value = %d\n", getId(), msg.getInt());
    m_queMsg.push(msg);
}

oUINT32 OsnService::getMsgSize()
{
    return m_queMsg.size();
}



