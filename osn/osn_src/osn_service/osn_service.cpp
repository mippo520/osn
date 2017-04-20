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


OsnPreparedStatement OsnService::dispatch(const OsnPreparedStatement &stmt)
{
	const OsnPreparedStatement &arg = *stmt.getPoint<OsnPreparedStatement>(1);
	MAP_DISPATCH_FUNC_ITR itr = m_mapDispatchFunc.find(stmt.getInt32(0));
	if (itr != m_mapDispatchFunc.end())
	{
        itr->second(arg);
	}
	return stmt;
}

void OsnService::init()
{
    registDispatchFunc(ePType_Start, &OsnService::start);
    g_ServiceManager.send(getId(), ePType_Start);
}

oBOOL OsnService::popMessage(stServiceMessage &msg)
{
    oBOOL bRet = false;
    m_QueMsgSpinLock.lock();
    if (m_queMsg.size() > 0) {
        msg = m_queMsg.front();
        m_queMsg.pop();

        bRet = true;
    }
    
    if (!bRet)
    {
        this->setIsInGlobal(false);
    }
    
    m_QueMsgSpinLock.unlock();
    return bRet;
}

oBOOL OsnService::dispatchMessage(oINT32 &nType)
{
    stServiceMessage msg;
    if (popMessage(msg))
    {

		if (ePType_Response == msg.nType)
		{
			MAP_SESSION_CO_ITR itr = m_mapSessionCoroutine.find(msg.unSession);
			if (itr == m_mapSessionCoroutine.end())
			{
				printf("unknown session : %d from %x\n", msg.unSession, msg.unSource);
			}
			else
			{
				oUINT32 co = itr->second;
				m_mapSessionCoroutine.erase(itr);
				nType = suspend(co, g_CorotineManager.resume(co, msg.stmt));
			}
		}
		else
		{
			oUINT32 co = createCO(std::bind(&OsnService::dispatch, this, std::placeholders::_1));
			m_mapCoroutineSession[co] = msg.unSession;
			m_mapCoroutineSource[co] = msg.unSource;
			OSN_CO_ARG arg;
			arg.setInt32(0, msg.nType);
			arg.setPoint(1, &msg.stmt);
			nType = suspend(co, g_CorotineManager.resume(co, arg));
		}

        return true;
    }
    else{
        return false;
    }
}

oUINT32 OsnService::pushMsg(stServiceMessage &msg)
{
    oUINT32 unSession = msg.unSession;
	if (0 == unSession)
	{
        unSession = ATOM_INC(&m_unSessionCount);
		msg.unSession = unSession;
	}
    
    m_QueMsgSpinLock.lock();
    m_queMsg.push(msg);
    
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
	oUINT32 curCo = 0;
    s_CoQueSpinLock.lock();
    if (s_queCoroutine.size() > 0)
    {
		curCo = s_queCoroutine.front();
        s_queCoroutine.pop();
    }
    s_CoQueSpinLock.unlock();
    if (0 == curCo)
    {
        curCo = g_CorotineManager.create([=](oUINT32 co, const OsnPreparedStatement &arg){
            func(arg);
            while (true) {
                OsnPreparedStatement stmt;
                stmt.setInt32(0, eYT_Exit);
                stmt = g_CorotineManager.yield(stmt);
                OSN_SERVICE_CO_FUNC funcNew = stmt.getFunction(0);
                stmt = g_CorotineManager.yield();
                funcNew(stmt);
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
    oINT32 nType = arg.getInt32(0);
    switch (nType) {
        case eYT_Call:
		{
			oUINT32 unSession = arg.getUInt32(1);
			m_mapSessionCoroutine[unSession] = co;
		}
            break;
        case eYT_Return:
		{
			const OsnPreparedStatement *pStmt = arg.getPoint<OsnPreparedStatement>(1);
			if (NULL != pStmt)
			{
				oUINT32 unSession = m_mapCoroutineSession[co];
				oUINT32 unSource = m_mapCoroutineSource[co];
				g_ServiceManager.sendMessage(unSource, 0, ePType_Response, unSession, *pStmt);
				nType = suspend(co, g_CorotineManager.resume(co));
			}
		}
            break;
        case eYT_Exit:
            m_mapCoroutineSession[co] = 0;
            m_mapCoroutineSource[co] = 0;
            s_CoQueSpinLock.lock();
            s_queCoroutine.push(co);
            s_CoQueSpinLock.unlock();
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




