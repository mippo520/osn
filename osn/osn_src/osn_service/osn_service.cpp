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
OsnService::stMsgCoroutineInfo OsnService::stMsgCoroutineInfo::NONE;

OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_unSessionCount(0)
{
}

void OsnService::registDispatchFunc(oINT32 nPType, DISPATCH_FUNC func)
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
    while (m_queForkInfo.size() > 0)
    {
        SAFE_DELETE(m_queForkInfo.front());
        m_queForkInfo.pop();
    }
}

void OsnService::exit()
{
    // destroy the coroutines yielded in service
    MAP_SESSION_CO_ITR itr = m_mapSessionCoroutine.begin();
    for (; itr != m_mapSessionCoroutine.end(); ++itr)
    {
        ID_COROUTINE co = itr->second;
        if (co > 0)
        {
            g_Coroutine->destroy(co);
        }
    }
    
    // return the call coroutines by default value
    std::map<ID_COROUTINE, stMsgCoroutineInfo>::iterator msgItr = m_mapCoroutineMsg.begin();
    for (; msgItr != m_mapCoroutineMsg.end(); ++msgItr)
    {
        stMsgCoroutineInfo &info = msgItr->second;
        if (0 != info.source and 0 != info.session)
        {
            g_Service->sendMessage(info.source, 0, ePType_Response, info.session);
        }
    }
    
    for (oUINT32 i = 0; i < m_vecUnreturnCall.size(); ++i)
    {
        stMsgCoroutineInfo &info = m_vecUnreturnCall[i];
        g_Service->sendMessage(info.source, 0, ePType_Response, info.session);
    }
}

void OsnService::onStart(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
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
    ID_SESSION session = stmt.popBackUInt32();
    ID_SERVICE source = stmt.popBackUInt64();
    oINT32 type = stmt.popBackInt32();
	MAP_DISPATCH_FUNC_ITR itr = m_mapDispatchFunc.find(type);
	if (itr != m_mapDispatchFunc.end())
	{
        itr->second(source, session, stmt);
	}
}

void OsnService::init(const OsnPreparedStatement &stmt)
{
	registDispatchFunc(ePType_Start, std::bind(&OsnService::onStart, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
            setCoroutineMsg(co, pMsg->unSession, pMsg->unSource);
            pMsg->stmt->pushBackInt32(pMsg->nType);
            pMsg->stmt->pushBackUInt64(pMsg->unSource);
            pMsg->stmt->pushBackUInt32(pMsg->unSession);
            nType = suspend(co, g_Coroutine->resume(co, pMsg->stmt));
		}

        SAFE_DELETE(pMsg);
        
        while (m_queForkInfo.size() > 0)
        {
            stForkInfo *pInfo = m_queForkInfo.front();
            m_queForkInfo.pop();
            ID_COROUTINE co = createCO(pInfo->func);
            pInfo->stmt->printContext();
            suspend(co, g_Coroutine->resume(co, pInfo->stmt));
            SAFE_DELETE(pInfo);
        }
        
        return true;
    }
    else{
        return false;
    }
    

}

ID_SESSION OsnService::newSession()
{
    ID_SESSION session = ++m_unSessionCount;
    if (0 == session)
    {
        m_unSessionCount = 1;
        return 1;
    }
    return session;
}

void OsnService::pushMsg(stServiceMessage *pMsg)
{   
    m_QueMsgSpinLock.lock();
    m_queMsg.push(pMsg);
    
    if (!getIsInGlobal())
    {
        setIsInGlobal(true);
        g_Service->pushWarkingService(getId());
    }
    
    m_QueMsgSpinLock.unlock();
}

oUINT32 OsnService::getMsgSize()
{
    m_QueMsgSpinLock.lock();
    oUINT32 unSize = m_queMsg.size();
    m_QueMsgSpinLock.unlock();
    return unSize;
}

ID_COROUTINE OsnService::createCO(VOID_STMT_FUNC func)
{
	ID_COROUTINE curCo = popFromCoroutinePool();

    if (0 == curCo)
    {       
        curCo = g_Coroutine->create([=](ID_COROUTINE co, SHARED_PTR_STMT arg){
            func(*arg);
            while (true) {
                SHARED_PTR_STMT stmt(new OsnPreparedStatement());
                stmt->pushBackInt32(eYT_Exit);
                SHARED_PTR_STMT stmtFunc = g_Coroutine->yield(stmt);
                VOID_STMT_FUNC funcNew = stmtFunc->getFunction(0);
                SHARED_PTR_STMT stmtArg = g_Coroutine->yield();
                funcNew(*stmtArg);
            }
            return arg;
        });
    }
    else
    {
        SHARED_PTR_STMT stmt(new OsnPreparedStatement());
        stmt->setFunction(0, func);
		g_Coroutine->resume(curCo, stmt);
    }
    
    return curCo;
}

oINT32 OsnService::suspend(ID_COROUTINE co, SHARED_PTR_STMT arg)
{
	if (arg->isEmpty())
	{
	}
    oINT32 nType = arg->popBackInt32();
    switch (nType) {
        case eYT_Call:
		{
			ID_SESSION unSession = arg->getUInt32(0);
			m_mapSessionCoroutine[unSession] = co;
		}
            break;
        case eYT_Sleep:
        {
            ID_SESSION unSession = arg->popBackUInt32();
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
            ID_SESSION unSession = arg->popBackUInt32();
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
            ID_COROUTINE unCo = arg->popBackUInt64();
            if (m_mapSleepCoSession.find(unCo) != m_mapSleepCoSession.end())
            {
                m_queWakeup.push(unCo);
            }
            nType = suspend(co, g_Coroutine->resume(co));
        }
            break;
        case eYT_Return:
		{
            stMsgCoroutineInfo &info = getCoroutineMsg(co);
            if (!info.isNone())
            {
                g_Service->sendMessage(info.source, 0, ePType_Response, info.session, *arg);
                info.clear();
            }
            else
            {
                printf("OsnService::suspend return Error! return without a call!\n");
                assert(0);
            }
            nType = suspend(co, g_Coroutine->resume(co));
		}
            break;
        case eYT_Exit:
        {
            stMsgCoroutineInfo &info = getCoroutineMsg(co);
            if (0 != info.source)
            {
                printf("a call session:%lu from service:%llu to service:%llu does not return!\n", info.session, info.source, getId());
                m_vecUnreturnCall.push_back(info);
                assert(0);
            }
            removeCoroutineMsg(co);
            pushToCoroutinePool(co);
        }
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
    SHARED_PTR_STMT stmt(new OsnPreparedStatement());
    stmt->setBool(0, false);
    stmt->setString(1, "BREAK");
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

void OsnService::setCoroutineMsg(ID_COROUTINE co, ID_SESSION session, ID_SERVICE source)
{
    stMsgCoroutineInfo &info = m_mapCoroutineMsg[co];
    info.source = source;
    info.session = session;
}

OsnService::stMsgCoroutineInfo& OsnService::getCoroutineMsg(ID_COROUTINE co)
{
    std::map<ID_COROUTINE, stMsgCoroutineInfo>::iterator itr = m_mapCoroutineMsg.find(co);
    if (itr != m_mapCoroutineMsg.end())
    {
        return itr->second;
    }
    else
    {
        return stMsgCoroutineInfo::NONE;
    }
}

void OsnService::removeCoroutineMsg(ID_COROUTINE co)
{
    std::map<ID_COROUTINE, stMsgCoroutineInfo>::iterator itr = m_mapCoroutineMsg.find(co);
    if (itr != m_mapCoroutineMsg.end())
    {
        m_mapCoroutineMsg.erase(co);
    }
}

ID_COROUTINE OsnService::fork(const VOID_STMT_FUNC &func, const OsnPreparedStatement &stmt)
{
    stForkInfo *pInfo = new stForkInfo();
    pInfo->func = func;
    pInfo->stmt = SHARED_PTR_STMT(new OsnPreparedStatement(stmt));
    m_queForkInfo.push(pInfo);
    return 0;
}

void OsnService::forkFunc(const OsnPreparedStatement &stmt)
{
    
}

