//
//  gate.cpp
//  gate
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "gate.h"
#include "osn_netpack.h"
#include "osn_gate_head.h"
#include "osn_agent_head.h"

AddService_Instance(Gate)

Gate::Gate()
    : m_SocketId(0)
    , m_isNodelay(true)
{
    
}

Gate::~Gate()
{
    
}


void Gate::funcSocketData(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        return;
    }
    oUINT8 *pBuffer = (oUINT8*)msg->pBuffer;
    oINT32 nSize = msg->nSize;
    oINT32 fd = msg->id;
    
    std::map<oINT32, stConnectInfo>::iterator itr = m_mapConnect.find(fd);
    if (itr != m_mapConnect.end())
    {
        stConnectInfo &info = itr->second;
        OsnPreparedStatement arg;
        arg.setInt32(0, fd);
        arg.setUInt64(1, (oUINT64)pBuffer);
        arg.setInt32(2, nSize);
        g_Osn->redirect(info.agent, info.client, ePType_Client, 1, arg);
    }
    else
    {
        oINT32 h = OsnNetpack::hashFD(fd);
        LST_SOCK_UNCOMPLETE &lstUncomplete = m_sockQueue.hash[h];
        OsnNetpack::filterData(m_sockQueue.queNetpack, lstUncomplete, fd, pBuffer, nSize);
        while (m_sockQueue.queNetpack.size() > 0)
        {
            stNetPack &pack = m_sockQueue.queNetpack.front();
            printf("gate fd = %d\n", pack.id);
            for (oINT32 i = 0; i < pack.size; ++i)
            {
                printf("%c", pack.pBuffer[i]);
            }
            printf("\n");
            SAFE_FREE(pack.pBuffer);
            m_sockQueue.queNetpack.pop();
        }
    }
}

void Gate::funcSocketConnect(const stOsnSocketMsg *msg)
{
    printf("Gate::funcSocketConnect ============>! fd = %d\n", msg->id);
}

void Gate::funcSocketClose(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        return;
    }
    oINT32 fd = msg->id;
    oINT32 h = OsnNetpack::hashFD(fd);
    LST_SOCK_UNCOMPLETE &lstUncomplete = m_sockQueue.hash[h];
    OsnNetpack::closeUncomplete(lstUncomplete, msg->id);
    
    if (fd != m_SocketId)
    {
        printf("socket close! fd:%d\n", fd);
        closeFD(fd);
        closeAgent(fd);
    }
    else
    {
        m_SocketId = 0;
    }
}

void Gate::funcSocketAccept(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        return;
    }
    assert(m_SocketId == msg->id);
    oINT32 fd = msg->ud;
    
    if (m_isNodelay)
    {
        g_SocketDriver->nodelay(fd);
    }
    
    std::string strContext(msg->pBuffer, msg->nSize);
    printf("Gate::funcSocketAccept ========> ud = %d, context = %s\n", msg->ud, strContext.c_str());
    stConnectInfo &connectInfo = m_mapConnect[fd];
    connectInfo.fd = fd;
    connectInfo.strIP = strContext.c_str();
    ID_SERVICE agent = g_Osn->startService("Agent");
    m_mapFDService[fd] = agent;
    OsnPreparedStatement arg;
    arg.setInt32(0, osn_agent::Func_Start); // 函数
    arg.setUInt64(1, g_Osn->self());        // gate的服务id
    arg.setInt32(2, fd);                    // fd
    g_Osn->call(agent, ePType_User, arg);
}

void Gate::funcSocketError(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        return;
    }
    oINT32 fd = msg->id;
    oINT32 h = OsnNetpack::hashFD(fd);
    LST_SOCK_UNCOMPLETE &lstUncomplete = m_sockQueue.hash[h];
    OsnNetpack::closeUncomplete(lstUncomplete, fd);
    if (fd == m_SocketId)
    {
        m_SocketId = 0;
        g_SocketDriver->close(fd);
        printf("gate close listen socket, accpet error: %s\n", msg->pBuffer);
    }
    else
    {
        printf("socket error! fd:%d %s\n", fd, msg->pBuffer);
        closeFD(fd);
        closeAgent(fd);
    }
    
}

void Gate::funcSocketWarning(const stOsnSocketMsg *msg)
{
    printf("Gate::funcSocketWarning ============>!\n");
}

void Gate::funcUserOpen(ID_SERVICE source, const OsnPreparedStatement &stmt)
{
    std::string strAddress = stmt.getString(1);
    if ("" == strAddress)
    {
        strAddress = "0.0.0.0";
    }
    oINT32 nPort = stmt.getInt32(2);
    printf("Listen on %s:%d\n", strAddress.c_str(), nPort);
    m_SocketId = g_SocketDriver->listen(strAddress.c_str(), nPort);
    g_SocketDriver->start(m_SocketId);
}

void Gate::funcUserClose(ID_SERVICE source, const OsnPreparedStatement &stmt)
{
    assert(m_SocketId != 0);
    g_SocketDriver->close(m_SocketId);
}

void Gate::funcUserKick(ID_SERVICE source, const OsnPreparedStatement &stmt)
{
    oINT32 fd = stmt.getInt32(1);
    closeClient(fd);
    closeAgent(fd);
}

void Gate::funcUserForward(ID_SERVICE source, const OsnPreparedStatement &stmt)
{
    oINT32 fd = stmt.getInt32(1);
    
    ID_SERVICE client = 0;
    if (stmt.getPreparedStatementDataCount() >= 3)
    {
        client = stmt.getUInt64(2);
    }
    
    ID_SERVICE address = 0;
    if (stmt.getPreparedStatementDataCount() >= 4)
    {
        address = stmt.getUInt64(3);
    }
    
    std::map<oINT32, stConnectInfo>::iterator itr = m_mapConnect.find(fd);
    if (itr == m_mapConnect.end())
    {
        assert(0);
        return;
    }
    
    stConnectInfo &info = itr->second;
    unforward(info);
    info.agent = address > 0 ? address : source;
    info.client = client;
    m_mapForward[info.agent] = &info;
    openClient(fd);
}

void Gate::funcUserAccept(ID_SERVICE source, const OsnPreparedStatement &stmt)
{
    oINT32 fd = stmt.getInt32(1);
    std::map<oINT32, stConnectInfo>::iterator itr = m_mapConnect.find(fd);
    if (itr == m_mapConnect.end())
    {
        assert(0);
        return;
    }
    stConnectInfo &info = itr->second;
    unforward(info);
    openClient(fd);
}

void Gate::unforward(stConnectInfo &info)
{
    if (0 != info.agent)
    {
        std::map<ID_SERVICE, stConnectInfo*>::iterator itr = m_mapForward.find(info.agent);
        if (itr != m_mapForward.end())
        {
            m_mapForward.erase(itr);
        }
        info.unforward();
    }
}

void Gate::openClient(oINT32 fd)
{
    if (m_mapConnect.find(fd) != m_mapConnect.end())
    {
        g_SocketDriver->start(fd);
    }
    else
    {
        assert(0);
        printf("Gate::openClient Error! can not found fd:%d\n", fd);
    }
}

void Gate::closeClient(oINT32 fd)
{
    std::map<oINT32, stConnectInfo>::iterator itr = m_mapConnect.find(fd);
    if (itr != m_mapConnect.end())
    {
        m_mapConnect.erase(itr);
        g_SocketDriver->close(fd);
    }
    else
    {
        printf("Gate::closeClient Error! can not found fd:%d\n", fd);
        assert(0);
    }
}

void Gate::closeFD(oINT32 fd)
{
    std::map<oINT32, stConnectInfo>::iterator itr = m_mapConnect.find(fd);
    if (itr != m_mapConnect.end())
    {
        unforward(itr->second);
        m_mapConnect.erase(itr);
    }
}

void Gate::closeAgent(oINT32 fd)
{
    std::map<oINT32, ID_SERVICE>::iterator itr = m_mapFDService.find(fd);
    if (itr != m_mapFDService.end())
    {
        ID_SERVICE agent = itr->second;
        m_mapFDService.erase(itr);
        OsnPreparedStatement arg;
        arg.setInt32(0, osn_agent::Func_Disconnect);
        g_Osn->send(agent, ePType_User, arg);
    }
}

void Gate::start(const OsnPreparedStatement &stmt)
{
    RegistDispatchFunc(ePType_User, &Gate::dispatchUser, this);
    RegistDispatchFunc(ePType_Socket, &Gate::dispatchSocket, this);
    m_vecSockDispatchFunc.resize(eOST_Warning + 1);
    m_vecSockDispatchFunc[eOST_Data] = std::bind(&Gate::funcSocketData, this, std::placeholders::_1);
    m_vecSockDispatchFunc[eOST_Connect] = std::bind(&Gate::funcSocketConnect, this, std::placeholders::_1);
    m_vecSockDispatchFunc[eOST_Accept] = std::bind(&Gate::funcSocketAccept, this, std::placeholders::_1);
    m_vecSockDispatchFunc[eOST_Error] = std::bind(&Gate::funcSocketError, this, std::placeholders::_1);
    m_vecSockDispatchFunc[eOST_Close] = std::bind(&Gate::funcSocketClose, this, std::placeholders::_1);
    m_vecSockDispatchFunc[eOST_Warning] = std::bind(&Gate::funcSocketWarning, this, std::placeholders::_1);
    
    m_vecUserDispatchFunc.resize(osn_gate::Func_Count);
    m_vecUserDispatchFunc[osn_gate::Func_Open] = std::bind(&Gate::funcUserOpen, this, std::placeholders::_1, std::placeholders::_2);
    m_vecUserDispatchFunc[osn_gate::Func_Close] = std::bind(&Gate::funcUserClose, this, std::placeholders::_1, std::placeholders::_2);
    m_vecUserDispatchFunc[osn_gate::Func_Kick] = std::bind(&Gate::funcUserKick, this, std::placeholders::_1, std::placeholders::_2);
    m_vecUserDispatchFunc[osn_gate::Func_Forward] = std::bind(&Gate::funcUserForward, this, std::placeholders::_1, std::placeholders::_2);
    m_vecUserDispatchFunc[osn_gate::Func_Accept] = std::bind(&Gate::funcUserAccept, this, std::placeholders::_1, std::placeholders::_2);
}

void Gate::dispatchUser(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    if (stmt.isEmpty())
    {
        return;
    }
    
    oINT32 cmd = stmt.getInt32(0);
    if (cmd >= osn_gate::Func_Open && cmd < osn_gate::Func_Count)
    {
        m_vecUserDispatchFunc[cmd](source, stmt);
        g_Osn->ret();
    }
}

void Gate::dispatchSocket(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    const stOsnSocketMsg *pSM = (stOsnSocketMsg*)stmt.getUInt64(0);
    if (NULL != pSM)
    {
        if (pSM->type > eOST_None && pSM->type <= eOST_Warning)
        {
            m_vecSockDispatchFunc[pSM->type](pSM);
        }
        else
        {
            printf("Gate::dispatchSocket Error! message type %d error!\n", pSM->type);
        }
    }
    SAFE_DELETE(pSM);
}


