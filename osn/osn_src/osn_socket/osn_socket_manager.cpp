//
//  osn_socket.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_socket_manager.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifdef __APPLE__
#include "osn_kqueue.h"
#elif defend __linux__
#include "osn_epoll.h"
#endif

OsnSocketManager::OsnSocketManager()
    : m_nEventFD(0)
    , m_nRecvFD(0)
    , m_nSendFD(0)
    , m_bCheckCtrl(true)
    , m_nEventN(0)
    , m_nEventIndex(0)
{
#ifdef __APPLE__
    m_pPool = new OsnKqueue();
#elif defend __linux__
#endif
}

OsnSocketManager::~OsnSocketManager()
{
    
}


void OsnSocketManager::init()
{
    create();
}

void OsnSocketManager::exit()
{
    stRequestPackage request;
    sendRequest(request, 'X', 0);
}

void OsnSocketManager::release()
{
    
}

oINT32 OsnSocketManager::poll()
{

    return 1;
}

oBOOL OsnSocketManager::create()
{
    if (NULL == m_pPool) {
        return false;
    }
    
    oINT32 nfd[2];
    m_nEventFD = m_pPool->create();
    if (m_pPool->invalid(m_nEventFD))
    {
        printf("OsnSocket::create Error! create event pool failed!\n");
        return false;
    }
    
    if (pipe(nfd))
    {
        printf("OsnSocket::create Error! create socket pair failed!\n");
        m_pPool->release(m_nEventFD);
        return false;
    }
    
    if (!m_pPool->add(m_nEventFD, nfd[0], NULL))
    {
        printf("OsnSocket::create Error! can not add server fd to event poll\n");
        close(nfd[0]);
        close(nfd[1]);
        return false;
    }
    
    m_nRecvFD = nfd[0];
    m_nSendFD = nfd[1];
    FD_ZERO(&m_fdSet);
    
    return true;
}

void OsnSocketManager::sendRequest(stRequestPackage &request, oINT8 type, oUINT8 len)
{
    request.header[6] = static_cast<oUINT8>(type);
    request.header[7] = len;
    for (;;)
    {
        oINT32 nSize = write(m_nSendFD, &request.header[6], len + 2);
        if (nSize < 0)
        {
            if (errno != EINTR)
            {
                printf("OsnSocketManager::sendRequest Error! send ctrl command error %s!\n", strerror(errno));
            }
            continue;
        }
        assert(nSize == len+2);
        return;
    }
}

oBOOL OsnSocketManager::hasCmd()
{
    timeval tv = {0, 0};
    oINT32 retval;
    
    FD_SET(m_nRecvFD, &m_fdSet);
    
    retval = select(m_nRecvFD + 1, &m_fdSet, NULL, NULL, &tv);
    
    if (1 == retval) {
        return true;
    }
    
    return false;
}

oINT32 OsnSocketManager::pollResult(stSocketMessage &result, oINT32 &nMore)
{
    for (;;)
    {
        if (m_bCheckCtrl)
        {
            if (hasCmd())
            {
                oINT32 nType = ctrlCmd(result);
                if (eSType_None != nType)
                {
                    clearClosedEvent(result, nType);
                    return nType;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                m_bCheckCtrl = false;
            }
        }
    }
    return 1;
}

oINT32 OsnSocketManager::ctrlCmd(stSocketMessage &result)
{
    oUINT8 buffer[256];
    oUINT8 header[2];
    blockReadPipe(header, sizeof(header));
    oUINT8 type = header[0];
    oUINT8 len = header[1];
    blockReadPipe(buffer, len);
    switch (type) {
        case 'X':
            result.clear();
            return eSType_Exit;
        default:
            printf("OsnSocketManager::ctrlCmd : unknown ctrl %c!\n", type);
            return eSType_None;
    }
    return eSType_None;
}

void OsnSocketManager::blockReadPipe(void *pBuffer, oINT32 sz)
{
    for (;;)
    {
        oINT32 nSize = read(m_nRecvFD, pBuffer, sz);
        if (nSize < 0)
        {
            if (errno != EINTR)
            {
                printf("OsnSocketManager::blockReadPipe Error! read from pipe error %s!\n", strerror(errno));
            }
            continue;
        }
        assert(nSize == sz);
        return;
    }
}

void OsnSocketManager::clearClosedEvent(stSocketMessage &result, oINT32 nType)
{
    if (eSType_Close == nType || eSType_Error == nType)
    {
        oINT32 nId = result.id;
        for (oINT32 i = m_nEventIndex; i < m_nEventN; ++i)
        {
            stSocketEvent &event = m_Events[i];
            OsnSocket *pSocket = static_cast<OsnSocket*>(event.socket);
            if (NULL!= pSocket)
            {
                if (eSStatus_Invalid == pSocket->getType() && pSocket->getId() == nId)
                {
                    event.socket = NULL;
                    break;
                }
            }
        }
    }
}




