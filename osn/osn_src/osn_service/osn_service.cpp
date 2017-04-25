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
#include <functional>
#include "osn_prepared_statement.h"
#include <signal.h>

std::queue<oUINT32> OsnService::s_queCoroutine;
OsnSpinLock OsnService::s_CoQueSpinLock;
oUINT64 OsnService::s_u64CoroutineCount = 0;
OsnSpinLock OsnService::s_CoCountLock;

OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_unSessionCount(0)
{
}

void OsnService::registDispatchFunc(oINT32 nPType, CO_MEMBER_FUNC funcPtr)
{
	m_mapDispatchFunc[nPType] = std::bind(funcPtr, this, std::placeholders::_1);
}


oBOOL OsnService::getIsInGlobal()
{
    oBOOL bInGlobal = m_IsInGlobal;
	return bInGlobal;
}


void OsnService::setIsInGlobal(oBOOL value)
{
    m_IsInGlobal = value;
}

OsnService::~OsnService()
{
}

void OsnService::exit()
{
}


void OsnService::dispatch(const OsnPreparedStatement &stmt)
{
	MAP_DISPATCH_FUNC_ITR itr = m_mapDispatchFunc.find(stmt.popBackInt32());
	if (itr != m_mapDispatchFunc.end())
	{
        itr->second(stmt);
	}
}

void OsnService::init()
{
    registDispatchFunc(ePType_Start, &OsnService::start);
    g_ServiceManager.send(getId(), ePType_Start);
}

stServiceMessage* OsnService::popMessage()
{
    stServiceMessage *pMsg = NULL;
    m_QueMsgSpinLock.lock();
    if (m_queMsg.size() > 0) {
        pMsg = m_queMsg.front();
        m_queMsg.pop();
    }
    
    if (NULL == pMsg)
    {
        this->setIsInGlobal(false);
    }
    
    m_QueMsgSpinLock.unlock();
    return pMsg;
}

oBOOL OsnService::dispatchMessage(oINT32 &nType)
{
    stServiceMessage *pMsg = popMessage();
    if (NULL != pMsg)
    {

		if (ePType_Response == pMsg->nType)
		{
			MAP_SESSION_CO_ITR itr = m_mapSessionCoroutine.find(pMsg->unSession);
			if (itr == m_mapSessionCoroutine.end())
			{
				printf("Osn Return Error! unknown session : %lu from %lx\n", pMsg->unSession, pMsg->unSource);
			}
			else
			{
				oUINT32 co = itr->second;
				m_mapSessionCoroutine.erase(itr);
				nType = suspend(co, g_CorotineManager.resume(co, pMsg->stmt));
			}
		}
		else
		{
			oUINT32 co = createCO(std::bind(&OsnService::dispatch, this, std::placeholders::_1));
			m_mapCoroutineSession[co] = pMsg->unSession;
			m_mapCoroutineSource[co] = pMsg->unSource;
			pMsg->stmt.pushBackInt32(pMsg->nType);
			nType = suspend(co, g_CorotineManager.resume(co, pMsg->stmt));
		}

        SAFE_DELETE(pMsg);
        return true;
    }
    else{
        return false;
    }
}

oUINT32 OsnService::pushMsg(stServiceMessage *pMsg)
{
    oUINT32 unSession = pMsg->unSession;
	if (0 == unSession)
	{
        unSession = ATOM_INC(&m_unSessionCount);
		pMsg->unSession = unSession;
	}
    
    m_QueMsgSpinLock.lock();
    m_queMsg.push(pMsg);
    
    if (!getIsInGlobal())
    {
        setIsInGlobal(true);
        g_ServiceManager.pushWarkingService(getId());
    }
    
    m_QueMsgSpinLock.unlock();
    
	return unSession;
}

oUINT32 OsnService::getMsgSize()
{
    m_QueMsgSpinLock.lock();
    oUINT32 unSize = m_queMsg.size();
    m_QueMsgSpinLock.unlock();
    return unSize;
}

oUINT32 OsnService::createCO(OSN_SERVICE_CO_FUNC func)
{
	oUINT32 curCo = popFromCoroutinePool();

    if (0 == curCo)
    {
        s_CoCountLock.lock();
        ++s_u64CoroutineCount;
        s_CoCountLock.unlock();
        printf("coroutine count is %llu\n", s_u64CoroutineCount);
        
        curCo = g_CorotineManager.create([=](oUINT32 co, const OsnPreparedStatement &arg){
            func(arg);
            while (true) {
                OsnPreparedStatement stmt;
                stmt.pushBackInt32(eYT_Exit);
                const OsnPreparedStatement &stmtFunc = g_CorotineManager.yield(stmt);
                OSN_SERVICE_CO_FUNC funcNew = stmtFunc.getFunction(0);
                const OsnPreparedStatement &stmtArg = g_CorotineManager.yield();
                funcNew(stmtArg);
            }
            return arg;
        });

    }
    else
    {
        OsnPreparedStatement stmt;
        stmt.setFunction(0, func);
		g_CorotineManager.resume(curCo, stmt);
    }
    
    return curCo;
}

oINT32 OsnService::suspend(oUINT32 co, const OSN_CO_ARG &arg)
{
	if (arg.isEmpty())
	{
	}
    oINT32 nType = arg.popBackInt32();
    switch (nType) {
        case eYT_Call:
		{
			oUINT32 unSession = arg.getUInt32(0);
			m_mapSessionCoroutine[unSession] = co;
		}
            break;
        case eYT_Return:
		{
			oUINT32 unSession = m_mapCoroutineSession[co];
			oUINT32 unSource = m_mapCoroutineSource[co];
			g_ServiceManager.sendMessage(unSource, 0, ePType_Response, unSession, arg);
			nType = suspend(co, g_CorotineManager.resume(co));
		}
            break;
        case eYT_Exit:
            m_mapCoroutineSession[co] = 0;
            m_mapCoroutineSource[co] = 0;
            pushToCoroutinePool(co);
            break;
        case eYT_Response:
            
            break;
        case eYT_Quit:
            suspend(co, g_CorotineManager.resume(co));
            break;
        default:
            break;
    }
    return nType;
}

void OsnService::pushToCoroutinePool(oUINT32 co)
{
    s_CoQueSpinLock.lock();
    s_queCoroutine.push(co);
    s_CoQueSpinLock.unlock();
}

oUINT32 OsnService::popFromCoroutinePool()
{
    oUINT32 co = 0;
    s_CoQueSpinLock.lock();
    if (s_queCoroutine.size() > 0)
    {
        co = s_queCoroutine.front();
        s_queCoroutine.pop();
    }
    s_CoQueSpinLock.unlock();
    return co;
}



