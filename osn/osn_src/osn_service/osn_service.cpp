//
//  osn_service.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_service.h"
#include "osn_service_manager.h"
#include "osn_coroutine_manager.h"
#include <set>
#include "osn_prepared_statement.h"


OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_nSessionCount(0)
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
        OSN_SERVICE_MSG &msg = m_queMsg.front();
        oINT32 nValue = msg.getInt32(0);
        m_queMsg.pop();

        static std::set<oINT32> setInt;
        static std::mutex m_Mutex;

        if (nValue != getId())
        {
            printf("OsnService::dispatch error! id = %d,repeat value = %d\n", getId(), nValue);
        }
        printf("OsnService::dispatch id = %d, value = %d\n", getId(), nValue);



        return true;
    }
    else{
        return false;
    }
}

oINT32 OsnService::pushMsg(const OSN_SERVICE_MSG &msg)
{
//    printf("OsnService::pushMsg id = %d, value = %d\n", getId(), msg.getInt());
    m_queMsg.push(msg);
    return ++m_nSessionCount;
}

oUINT32 OsnService::getMsgSize()
{
    return m_queMsg.size();
}

oINT32 OsnService::createCO(OSN_COROUTINE_FUNC func)
{
    oINT32 co = -1;
    if (m_queCO.size() > 0)
    {
        co = m_queCO.front();
        m_queCO.pop();
    }
    
    if (-1 == co)
    {
        co = g_CorotineManager.create([&](const OSN_CO_ARG &arg){
            func(arg);
            while (true) {
                m_queCO.push(co);
                OsnPreparedStatement stmt;
                stmt.setInt32(0, eYT_Exit);
                stmt = g_CorotineManager.yield(stmt);
                func = stmt.getFunction(0);
                func(g_CorotineManager.yield());
            }
            return arg;
        });
    }
    else
    {
        OsnPreparedStatement stmt;
        stmt.setFunction(0, func);
        g_CorotineManager.resume(co, stmt);
    }
    
    return co;
}

void OsnService::suspend(oINT32 co, const OSN_CO_ARG &arg)
{
    oINT32 nType = arg.getInt32(0);
    switch (nType) {
        case eYT_Call:
            
            break;
        case eYT_Return:
            
            break;
        case eYT_Exit:
            
            break;
        case eYT_Response:
            
            break;
        default:
            break;
    }
}




