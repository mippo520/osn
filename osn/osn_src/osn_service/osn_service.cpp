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

std::queue<ID_COROUTINE> OsnService::s_queCoroutine;
OsnSpinLock OsnService::s_CoQueSpinLock;
oUINT64 OsnService::s_u64CoroutineCount = 0;
OsnSpinLock OsnService::s_CoCountLock;

OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_unSessionCount(0)
{
    m_vecCoroutineSession.resize(OsnArrTools::s_nObjCountBegin);
    m_vecCoroutineService.resize(OsnArrTools::s_nObjCountBegin);
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
    MAP_SESSION_CO_ITR itr = m_mapSessionCoroutine.begin();
    for (; itr != m_mapSessionCoroutine.end(); ++itr)
    {
        ID_COROUTINE co = itr->second;
        if (co > 0)
        {
            g_Coroutine->destroy(co);
        }
    }
}

void OsnService::onStart(const OsnPreparedStatement &stmt)
{
    oINT32 nType = stmt.popBackInt32();
    this->start(stmt);
    if (eYT_Call == nType)
    {
        g_Osn->ret();
    }
}

void OsnService::dispatch(const OsnPreparedStatement &stmt)
{
	MAP_DISPATCH_FUNC_ITR itr = m_mapDispatchFunc.find(stmt.popBackInt32());
	if (itr != m_mapDispatchFunc.end())
	{
        itr->second(stmt);
	}
}

void OsnService::init(const OsnPreparedStatement &stmt)
{
	registDispatchFunc(ePType_Start, std::bind(&OsnService::onStart, this, std::placeholders::_1));
    if (g_Service->getCurService() > 0)
    {
        stmt.pushBackInt32(eYT_Call);
        g_Osn->call(getId(), ePType_Start, stmt);
    }
    else
    {
        stmt.pushBackInt32(eYT_None);
        g_Osn->send(getId(), ePType_Start, stmt);
    }
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
				printf("Osn Return Error! unknown session : %lu from %llx\n", pMsg->unSession, pMsg->unSource);
			}
			else
			{
				ID_COROUTINE co = itr->second;
                m_mapSessionCoroutine.erase(itr);
                if (0 != co)
                {
                    nType = suspend(co, g_Coroutine->resume(co, pMsg->stmt));
                }
			}
		}
		else
		{
			ID_COROUTINE co = createCO(std::bind(&OsnService::dispatch, this, std::placeholders::_1));
            setCoroutineSession(co, pMsg->unSession);
            setCoroutineService(co, pMsg->unSource);
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

ID_SESSION OsnService::pushMsg(stServiceMessage *pMsg)
{
    ID_SESSION unSession = pMsg->unSession;
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

ID_COROUTINE OsnService::createCO(OSN_SERVICE_CO_FUNC func)
{
	ID_COROUTINE curCo = popFromCoroutinePool();

    if (0 == curCo)
    {
        s_CoCountLock.lock();
        ++s_u64CoroutineCount;
        s_CoCountLock.unlock();
        
        curCo = g_Coroutine->create([=](ID_COROUTINE co, const OsnPreparedStatement &arg){
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

oINT32 OsnService::suspend(ID_COROUTINE co, const OSN_CO_ARG &arg)
{
	if (arg.isEmpty())
	{
	}
    oINT32 nType = arg.popBackInt32();
    switch (nType) {
        case eYT_Call:
		{
			ID_SESSION unSession = arg.getUInt32(0);
			m_mapSessionCoroutine[unSession] = co;
		}
            break;
        case eYT_Sleep:
        {
            ID_SESSION unSession = arg.popBackUInt32();
            m_mapSessionCoroutine[unSession] = co;
            m_mapSleepCoSession[co] = unSession;
        }
            break;
        case eYT_CleanSleep:
        {
            std::map<ID_COROUTINE, ID_SESSION>::iterator itr = m_mapSleepCoSession.find(co);
            if (itr != m_mapSleepCoSession.end())
            {
                m_mapSleepCoSession.erase(itr);
            }
            ID_SESSION unSession = arg.popBackUInt32();
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
            ID_COROUTINE unCo = arg.popBackUInt64();
            if (m_mapSleepCoSession.find(unCo) != m_mapSleepCoSession.end())
            {
                m_queWakeup.push(unCo);
            }
            nType = suspend(co, g_Coroutine->resume(co));
        }
            break;
        case eYT_Return:
		{
			ID_SESSION unSession = getCoroutineSession(co);
			ID_SERVICE unSource = getCoroutineService(co);
			g_Service->sendMessage(unSource, 0, ePType_Response, unSession, arg);
			nType = suspend(co, g_Coroutine->resume(co));
		}
            break;
        case eYT_Exit:
            setCoroutineSession(co, 0);
            setCoroutineService(co, 0);
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
    
    ID_COROUTINE co = m_queWakeup.front();
    m_queWakeup.pop();
    std::map<ID_COROUTINE, ID_SESSION>::iterator itr = m_mapSleepCoSession.find(co);
    if (itr == m_mapSleepCoSession.end())
    {
        return eYT_None;
    }
    
    ID_SESSION unSession = itr->second;
    m_mapSessionCoroutine[unSession] = 0;
    OsnPreparedStatement stmt;
    stmt.setBool(0, false);
    stmt.setString(1, "BREAK");
    return suspend(co, g_Coroutine->resume(co, stmt));
}

void OsnService::pushToCoroutinePool(ID_COROUTINE co)
{
    s_CoQueSpinLock.lock();
    s_queCoroutine.push(co);
    s_CoQueSpinLock.unlock();
}

ID_COROUTINE OsnService::popFromCoroutinePool()
{
    ID_COROUTINE co = 0;
    s_CoQueSpinLock.lock();
    if (s_queCoroutine.size() > 0)
    {
        co = s_queCoroutine.front();
        s_queCoroutine.pop();
    }
    s_CoQueSpinLock.unlock();
    return co;
}

void OsnService::setCoroutineSession(ID_COROUTINE co, ID_SESSION session)
{
    oUINT64 pos = OsnArrTools::getPos(co);
    if (pos > m_vecCoroutineSession.size())
    {
        m_vecCoroutineSession.resize(pos);
    }
    m_vecCoroutineSession[pos] = session;
}

ID_SESSION OsnService::getCoroutineSession(ID_COROUTINE co)
{
    ID_SESSION session = 0;
    oUINT64 pos = OsnArrTools::getPos(co);
    if (m_vecCoroutineSession.size() > pos)
    {
        session = m_vecCoroutineSession[pos];
    }
    return session;
}

void OsnService::setCoroutineService(ID_COROUTINE co, ID_SERVICE service)
{
    if (co > m_vecCoroutineService.size())
    {
        m_vecCoroutineService.resize(co);
    }
    m_vecCoroutineService[co] = service;
}

ID_SERVICE OsnService::getCoroutineService(ID_COROUTINE co)
{
    ID_SERVICE service = 0;
    if (m_vecCoroutineService.size() > co)
    {
        service = m_vecCoroutineService[co];
    }
    return service;
}

