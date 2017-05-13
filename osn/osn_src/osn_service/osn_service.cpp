//
//  osn_service.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_service.h"
#include <set>
#include <functional>
#include <signal.h>
#include "osn_prepared_statement.h"
#include "I_osn_coroutine.h"
#include "I_osn_service.h"

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

void OsnService::registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC func)
{
	m_mapDispatchFunc[nPType] = func;
}

void OsnService::unregistDispatchFunc(oINT32 nPType)
{
    MAP_DISPATCH_FUNC_ITR itr = m_mapDispatchFunc.find(nPType);
    if (itr != m_mapDispatchFunc.end())
    {
        m_mapDispatchFunc.erase(itr);
    }
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
	registDispatchFunc(ePType_Start, std::bind(&OsnService::start, this, std::placeholders::_1));
    g_Osn->send(getId(), ePType_Start);
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
                if (0 != co)
                {
                    nType = suspend(co, g_Coroutine->resume(co, pMsg->stmt));
                }
			}
		}
		else
		{
			oUINT32 co = createCO(std::bind(&OsnService::dispatch, this, std::placeholders::_1));
			m_mapCoroutineSession[co] = pMsg->unSession;
			m_mapCoroutineSource[co] = pMsg->unSource;
			pMsg->stmt.pushBackInt32(pMsg->nType);
			nType = suspend(co, g_Coroutine->resume(co, pMsg->stmt));
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
        g_Service->pushWarkingService(getId());
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
        
        curCo = g_Coroutine->create([=](oUINT32 co, const OsnPreparedStatement &arg){
            func(arg);
            while (true) {
                OsnPreparedStatement stmt;
                stmt.pushBackInt32(eYT_Exit);
                const OsnPreparedStatement &stmtFunc = g_Coroutine->yield(stmt);
                OSN_SERVICE_CO_FUNC funcNew = stmtFunc.getFunction(0);
                const OsnPreparedStatement &stmtArg = g_Coroutine->yield();
                funcNew(stmtArg);
            }
            return arg;
        });

    }
    else
    {
        OsnPreparedStatement stmt;
        stmt.setFunction(0, func);
		g_Coroutine->resume(curCo, stmt);
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
        case eYT_Sleep:
        {
            oUINT32 unSession = arg.popBackUInt32();
            m_mapSessionCoroutine[unSession] = co;
            m_mapSleepSession[co] = unSession;
        }
            break;
        case eYT_CleanSleep:
        {
            std::map<oUINT32, oUINT32>::iterator itr = m_mapSleepSession.find(co);
            if (itr != m_mapSleepSession.end())
            {
                m_mapSleepSession.erase(itr);
            }
            oUINT32 unSession = arg.popBackUInt32();
            MAP_SESSION_CO_ITR coItr = m_mapSessionCoroutine.find(unSession);
            if (coItr != m_mapSessionCoroutine.end())
            {
                m_mapSessionCoroutine.erase(coItr);
            }
            nType = suspend(co, g_Coroutine->resume(co));
        }
            break;
        case eYT_Wakeup:
        {
            oUINT32 unCo = arg.popBackUInt32();
            if (m_mapSleepSession.find(unCo) != m_mapSleepSession.end())
            {
                m_queWakeup.push(unCo);
            }
            nType = suspend(co, g_Coroutine->resume(co));
        }
            break;
        case eYT_Return:
		{
			oUINT32 unSession = m_mapCoroutineSession[co];
			oUINT32 unSource = m_mapCoroutineSource[co];
			g_Service->sendMessage(unSource, 0, ePType_Response, unSession, &arg);
			nType = suspend(co, g_Coroutine->resume(co));
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
            suspend(co, g_Coroutine->resume(co));
            break;
        default:
            break;
    }
    oINT32 wakeupType = dispatchWakeup();
    if (eYT_Quit == wakeupType && wakeupType != nType)
    {
        nType = wakeupType;
    }
    return nType;
}

oINT32 OsnService::dispatchWakeup()
{
    if (0 == m_queWakeup.size())
    {
        return eYT_None;
    }
    
    oUINT32 co = m_queWakeup.front();
    m_queWakeup.pop();
    std::map<oUINT32, oUINT32>::iterator itr = m_mapSleepSession.find(co);
    if (itr == m_mapSleepSession.end())
    {
        return eYT_None;
    }
    
    oUINT32 unSession = itr->second;
    m_mapSessionCoroutine[unSession] = 0;
    OsnPreparedStatement stmt;
    stmt.setBool(0, false);
    stmt.setString(1, "BREAK");
    return suspend(co, g_Coroutine->resume(co, stmt));
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



