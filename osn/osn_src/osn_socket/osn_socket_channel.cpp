//
//  osn_socket_channel.cpp
//  osn
//
//  Created by Mippo on 2017/8/24.
//  Copyright © 2017年 Zenghui. All rights reserved.
//

#include "osn_socket_channel.h"
#include "I_osn_coroutine.h"
#include "I_osn_socket.h"

const std::string OsnSocketChannel::s_strSocketErr = "[Error: socket]";

OsnSocketChannel::OsnSocketChannel(const stChannelDesc &desc)
    : m_Desc(desc)
{
    
}

OsnSocketChannel::~OsnSocketChannel()
{
    
}

void OsnSocketChannel::init()
{
    m_Socket.init();
}

oBOOL OsnSocketChannel::connect(oBOOL bOnce)
{
    if (m_Desc.bClosed)
    {
        if (0 != m_Desc.dispatchThread)
        {
            assert(0 == m_Desc.connectingThread);
            ID_COROUTINE co = g_Coroutine->running();
            m_Desc.connectingThread = co;
            g_Osn->wait(co);
            m_Desc.connectingThread = 0;
        }
        m_Desc.bClosed = false;
    }
    
    return blockConnect(bOnce);
}

oBOOL OsnSocketChannel::blockConnect(oBOOL bOnce)
{
    eConnectState state = checkConnect();
    if (eCS_None != state)
    {
        return eCS_True == state;
    }
    
    std::string strErr;
    if (m_Desc.queConnecting.size() > 0)
    {
        ID_COROUTINE co = g_Coroutine->running();
        m_Desc.queConnecting.push(co);
        g_Osn->wait(co);
    }
    else
    {
        strErr = tryConnect(bOnce);
        if (m_Desc.queConnecting.size() > 0)
        {
            m_Desc.queConnecting.pop();
        }
        while (m_Desc.queConnecting.size() > 0)
        {
            ID_COROUTINE co = m_Desc.queConnecting.front();
            m_Desc.queConnecting.pop();
            g_Osn->wakeup(co);
        }
    }
    
    state = checkConnect();
    if (eCS_None == state)
    {
        printf("OsnSocketChannel::blockConnect Connect to %s:%d failed (%s)\n", m_Desc.strHost.c_str(), m_Desc.nPort, strErr.c_str());
        printf("OsnSocketChannel::blockConnect %s\n", s_strSocketErr.c_str());
        assert(0);
        return false;
    }
    else
    {
        return eCS_True == state;
    }
}

OsnSocketChannel::eConnectState OsnSocketChannel::checkConnect()
{ 
    if (0 != m_Desc.nSock)
    {
        if (0 == m_Desc.authCoroutine)
        {
            return eCS_True;
        }
        
        if (g_Coroutine->running() == m_Desc.authCoroutine)
        {
            return eCS_True;
        }
    }
    
    if (m_Desc.bClosed)
    {
        return eCS_False;
    }
    
    return eCS_None;
}

std::string OsnSocketChannel::tryConnect(oBOOL bOnce)
{
    oINT32 t = 0;
    while (!m_Desc.bClosed)
    {
        std::string strErr;
        oBOOL ok = connectOnce(strErr);
        if (ok)
        {
            if (!bOnce)
            {
                printf("OsnSocketChannel::tryConnect socket : connect to %s:%d\n", m_Desc.strHost.c_str(), m_Desc.nPort);
            }
            return "";
        }
        else if (bOnce)
        {
            return strErr;
        }
        else
        {
            printf("OsnSocketChannel::tryConnect socket : connect %s\n", strErr.c_str());
        }
        
        if (t > 1000)
        {
            printf("OsnSocketChannel::tryConnect socket : try to reconnect %s:%d\n", m_Desc.strHost.c_str(), m_Desc.nPort);
            //定时器
            //g_Osn->sleep(t)
            t = 0;
        }
        else
        {
            //定时器
            //g_Osn->sleep(t)
        }
        t += 100;
    }
    return "";
}

oBOOL OsnSocketChannel::connectOnce(std::string &errCode)
{
    if (m_Desc.bClosed)
    {
        return false;
    }
    
    assert(0 == m_Desc.nSock && 0 == m_Desc.authCoroutine);
    oINT32 fd = m_Socket.open(m_Desc.strHost.c_str(), m_Desc.nPort, errCode);
    if (0 == fd)
    {
        fd = connectBackup();
        if (0 == fd)
        {
            printf("OsnSocketChannel::connectOnce Error! %s\n", errCode.c_str());
            return false;
        }
    }
    
    if (m_Desc.bNodelay)
    {
        g_SocketDriver->nodelay(fd);
    }
    
    m_Desc.nSock = fd;
    m_Desc.dispatchThread = g_Osn->fork(dispatchFunction(), (oUINT64)this);
    
    if (nullptr != m_Desc.auth)
    {
        m_Desc.authCoroutine = g_Coroutine->running();
        std::string errMsg;
        oBOOL ok = m_Desc.auth(this, errMsg);
        if (!ok)
        {
            closeChannelSocket();
            if (errMsg != s_strSocketErr)
            {
                m_Desc.authCoroutine = 0;
                printf("OsnSocketChannel::connectOnce socket : auth failed %s!\n", errMsg.c_str());
            }
        }
        
        m_Desc.authCoroutine = 0;
        if (ok && 0 == m_Desc.nSock)
        {
            return connectOnce(errCode);
        }
        return ok;
    }
    return true;
}

oINT32 OsnSocketChannel::connectBackup()
{
    oINT32 fd = 0;
    std::string strErr;
    if (m_Desc.vecBackup.size() > 0)
    {
        for (oUINT32 i = 0; i < m_Desc.vecBackup.size(); ++i)
        {
            const stSocketAddr &addr = m_Desc.vecBackup[i];
            fd = m_Socket.open(addr.strHost.c_str(), addr.nPort, strErr);
            if (fd > 0)
            {
                m_Desc.strHost = addr.strHost;
                m_Desc.nPort = addr.nPort;
                break;
            }
        }
    }
    
    return fd;
}

VOID_STMT_FUNC OsnSocketChannel::dispatchFunction()
{
    if (nullptr != m_Desc.response)
    {
        return std::bind(&OsnSocketChannel::dispatchBySession, this, std::placeholders::_1);
    }
    else
    {
        return std::bind(&OsnSocketChannel::dispatchByOrder, this, std::placeholders::_1);
    }
}

void OsnSocketChannel::dispatchBySession(const OsnPreparedStatement &stmt)
{
    stmt.printContext("OsnSocketChannel::dispatchBySession =========> ");
}

void OsnSocketChannel::dispatchByOrder(const OsnPreparedStatement &stmt)
{
    stmt.printContext("OsnSocketChannel::dispatchByOrder =========>");
}

void OsnSocketChannel::closeChannelSocket()
{
    if (m_Desc.nSock > 0)
    {
        oINT32 fd = m_Desc.nSock;
        m_Desc.nSock = 0;
        m_Socket.close(fd);
    }
}



