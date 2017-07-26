//
//  agent.cpp
//  Agent
//
//  Created by zenghui on 17/6/28.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "agent.h"
#include "osn_agent_head.h"
#include "osn_gate_head.h"
#include "osn_netpack.h"

AddService_Instance(Agent)

Agent::Agent()
    : m_nClientFD(-1)
{
    
}

Agent::~Agent()
{
    
}

void Agent::funcUserStart(const OsnPreparedStatement &stmt)
{
    ID_SERVICE gate = stmt.getUInt64(1);
    m_nClientFD = stmt.getInt32(2);
    
    OsnPreparedStatement arg;
    arg.setInt32(0, osn_gate::Func_Forward);
    arg.setInt32(1, m_nClientFD);
    g_Osn->call(gate, ePType_User, arg);
    
//    ID_SERVICE ts = g_Osn->startService("TestService");
//    OsnPreparedStatement closeArg;
//    closeArg.setInt32(0, m_nClientFD);
//    closeArg.setUInt64(1, gate);
//    g_Osn->send(ts, ePType_User, closeArg);
    g_Osn->ret();
}

void Agent::funcUserDisconnect(const OsnPreparedStatement &stmt)
{
    g_Osn->exit();
}

void Agent::start(const OsnPreparedStatement &stmt)
{
    RegistDispatchFunc(ePType_User, &Agent::dispatchUser, this);
    RegistDispatchFunc(ePType_Client, &Agent::dispatchClient, this);
    m_vecUserDispatchFunc.resize(osn_agent::Func_Count);
    m_vecUserDispatchFunc[osn_agent::Func_Start] = std::bind(&Agent::funcUserStart, this, std::placeholders::_1);
    m_vecUserDispatchFunc[osn_agent::Func_Disconnect] = std::bind(&Agent::funcUserDisconnect, this, std::placeholders::_1);
}

void Agent::exit()
{
    OsnNetpack::closeUncomplete(m_lstUncomplete, m_nClientFD);
}

void Agent::dispatchClient(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    oINT32 fd = stmt.getInt32(0);
    assert(fd == m_nClientFD);
    oUINT8 *pBuffer = (oUINT8*)stmt.getUInt64(1);
    oINT32 nSize = stmt.getInt32(2);
    
//    printf("agent nsize = %d\n", nSize);
//    for (int i = 0; i < nSize; ++i)
//    {
//        printf("%c", pBuffer[i]);
//    }
//    printf("\n");
//    SAFE_FREE(pBuffer);
    OsnNetpack::filterData(m_queNetpack, m_lstUncomplete, fd, pBuffer, nSize);
    while (m_queNetpack.size() > 0)
    {
        stNetPack &pack = m_queNetpack.front();
//        printf("agent fd = %d\n", pack.id);
//        for (oINT32 i = 0; i < pack.size; ++i)
//        {
//            printf("%c", pack.pBuffer[i]);
//        }
//        printf("\n");
        SAFE_FREE(pack.pBuffer);
        m_queNetpack.pop();
        static oINT32 i = 0;
        ++i;
        if (i % 10000 == 0)
        {
            printf("agent catch %d!\n", i);
        }
    }

}

void Agent::dispatchUser(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    if (stmt.isEmpty())
    {
        return;
    }
    
    oINT32 cmd = stmt.getInt32(0);
    if (cmd >= osn_agent::Func_Start && cmd < osn_agent::Func_Count)
    {
        m_vecUserDispatchFunc[cmd](stmt);
    }
}

