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


OsnService::OsnService()
    : m_IsInGlobal(false)
    , m_Id(0)
    , m_unSessionCount(0)
{
    
}


oUINT32 OsnService::send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
	return g_ServiceManager.send(addr, getId(), type, 0, msg);
}


const OsnPreparedStatement& OsnService::call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg)
{
	oUINT32 unSession = g_ServiceManager.send(addr, getId(), type, 0, msg);
	OSN_CO_ARG arg;
	arg.setInt32(0, eYT_Call);
	arg.setUInt32(1, unSession);
	return g_CorotineManager.yield(arg);
}


const OsnPreparedStatement& OsnService::ret(const OsnPreparedStatement &msg)
{
	OSN_CO_ARG arg;
	arg.setInt32(0, eYT_Return);
	arg.setPoint(1, &msg);
	return g_CorotineManager.yield(arg);
}


void OsnService::registDispatchFunc(oINT32 nPType, CO_MEMBER_FUNC funcPtr)
{
	m_mapDispatchFunc[nPType] = std::bind(funcPtr, this, std::placeholders::_1);
}


oBOOL OsnService::getIsInGlobal()
{
	return m_IsInGlobal;
}


void OsnService::setIsInGlobal(oBOOL value)
{
	m_Mutex.lock();
	m_IsInGlobal = value;
	m_Mutex.unlock();
}

OsnService::~OsnService()
{
    
}

void OsnService::exit()
{
    printf("OsnService::exit id = %d\n", getId());
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
    printf("OsnService::init id = %d\n", getId());
}

oBOOL OsnService::dispatchMessage()
{
    if (m_queMsg.size() > 0) {
		m_Mutex.lock();
        stServiceMessage msg = m_queMsg.front();
		m_queMsg.pop();
		m_Mutex.unlock();
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
				suspend(co, g_CorotineManager.resume(co, msg.stmt));
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
			suspend(co, g_CorotineManager.resume(co, arg));
		}

        return true;
    }
    else{
		this->setIsInGlobal(false);
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
    
    m_Mutex.lock();
    m_queMsg.push(msg);
    m_Mutex.unlock();
    
	return unSession;
}

oUINT32 OsnService::getMsgSize()
{
    return m_queMsg.size();
}

oUINT32 OsnService::createCO(OSN_SERVICE_CO_FUNC func)
{
	oUINT32 curCo = 0;
    if (m_queCO.size() > 0)
    {
		curCo = m_queCO.front();
        m_queCO.pop();
    }
    
    if (0 == curCo)
    {
		curCo = g_CorotineManager.create([=](oUINT32 co, const OsnPreparedStatement &arg) {
			func(arg);
			while (true) {
				m_queCO.push(co);
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

void OsnService::suspend(oUINT32 co, const OSN_CO_ARG &arg)
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
				g_ServiceManager.send(unSource, getId(), ePType_Response, unSession, *pStmt);
				suspend(co, g_CorotineManager.resume(co));
			}
		}
            break;
        case eYT_Exit:
            
            break;
        case eYT_Response:
            
            break;
        default:
            break;
    }
}



