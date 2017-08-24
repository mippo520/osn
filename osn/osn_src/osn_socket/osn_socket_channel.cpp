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

OsnSocketChannel::OsnSocketChannel(const stChannelDesc &desc)
    : m_Desc(desc)
{
    
}

OsnSocketChannel::~OsnSocketChannel()
{
    
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
    
    if (m_Desc.queConnecting.size() > 0)
    {
        ID_COROUTINE co = g_Coroutine->running();
        m_Desc.queConnecting.push(co);
        g_Osn->wait(co);
    }
    else
    {
        
    }
}

OsnSocketChannel::eConnectState OsnSocketChannel::checkConnect()
{ 
    if (0 != m_Desc.nSock)
    {
        if (0 != m_Desc.authCoroutine)
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

oBOOL OsnSocketChannel::tryConnect(oBOOL bOnce)
{
    oINT32 t = 0;
    while (!m_Desc.bClosed)
    {
        
    }
    return false;
}

oBOOL OsnSocketChannel::connectOnce()
{
    if (m_Desc.bClosed)
    {
        return false;
    }
    
    assert(0 == m_Desc.nSock && 0 == m_Desc.authCoroutine);
    std::string strErr;
    oINT32 fd = m_Socket.open(m_Desc.strHost.c_str(), m_Desc.nPort, strErr);
    if (0 == fd)
    {
        fd = connectBackup();
        if (0 == fd)
        {
            printf("OsnSocketChannel::connectOnce Error! %s\n", strErr.c_str());
            return false;
        }
    }
    
    if (m_Desc.bNodelay)
    {
        g_SocketDriver->nodelay(fd);
    }
    
    m_Desc.nSock = fd;
    m_Desc.dispatchThread = g_Osn->fork([=](), <#const OsnPreparedStatement &stmt#>) 
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



