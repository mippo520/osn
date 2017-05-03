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
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "osn_prepared_statement.h"
#include "osn_service_manager.h"

#ifdef __APPLE__
#include "osn_kqueue.h"
#elif defined __linux__
#include "osn_epoll.h"
#endif

OsnSocketManager::OsnSocketManager()
    : m_nEventFD(0)
    , m_nRecvFD(0)
    , m_nSendFD(0)
    , m_bCheckCtrl(true)
	, m_nEventIndex(0)
	, m_nEventN(0)
    , m_nAllocId(0)
{
#ifdef __APPLE__
    m_pPoll = new OsnKqueue();
#elif defined __linux__
	m_pPoll = new OsnEpoll();
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
    oBOOL bMore = true;
    stSocketMessage result;
    oINT32 nType = pollResult(result, bMore);
    printf("OsnSocketManager::poll ===================> nType = %d\n", nType);
    switch (nType)
    {
        case eSockStatus_Exit:
            return 0;
        case eSockStatus_Data:
            forwardMessage(eOST_Data, false, result);
            break;
        case eSockStatus_Close:
            forwardMessage(eOST_Close, false, result);
            break;
        case eSockStatus_Open:
            forwardMessage(eOST_Connect, true, result);
            break;
        case eSockStatus_Error:
            forwardMessage(eOST_Error, true, result);
            break;
        case eSockStatus_Accept:
            forwardMessage(eOST_Accept, true, result);
            break;
        default:
            printf("OsnSocketManager::poll Error! Unknown socket message! type is %d!\n", nType);
            return -1;
    }
    if (bMore)
    {
        return -1;
    }
    return 1;
}

oBOOL OsnSocketManager::create()
{
    if (NULL == m_pPoll) {
        return false;
    }
    
    oINT32 nfd[2];
    m_nEventFD = m_pPoll->create();
    if (m_pPoll->invalid(m_nEventFD))
    {
        printf("OsnSocket::create Error! create event pool failed!\n");
        return false;
    }
    
    if (pipe(nfd))
    {
        printf("OsnSocket::create Error! create socket pair failed!\n");
        m_pPoll->release(m_nEventFD);
        return false;
    }
    
    if (!m_pPoll->add(m_nEventFD, nfd[0], NULL))
    {
        printf("OsnSocket::create Error! can not add server fd to event poll\n");
        ::close(nfd[0]);
        ::close(nfd[1]);
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
        oINT32 nSize = (oINT32)write(m_nSendFD, &request.header[6], len + 2);
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

oINT32 OsnSocketManager::pollResult(stSocketMessage &result, oBOOL &bMore)
{
    for (;;)
    {
        if (m_bCheckCtrl)
        {
            if (hasCmd())
            {
                oINT32 nType = ctrlCmd(result);
                if (eSockStatus_None != nType)
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
        
        if (m_nEventIndex == m_nEventN)
        {
            m_nEventN = m_pPoll->wait(m_nEventFD, m_Events, s_nMaxEvents);
            m_bCheckCtrl = true;
            bMore = false;
            m_nEventIndex = 0;
            if (m_nEventN <= 0)
            {
                m_nEventN = 0;
                return -1;
            }
        }
        
        stSocketEvent &event = m_Events[m_nEventIndex++];
        OsnSocket *pSocket = event.socket;
        if (NULL == pSocket)
        {
            continue;
        }
        
        switch (pSocket->getType())
        {
            case eSockType_Connecting:
                return reportConnect(*pSocket, result);
            case eSockType_Listen:
            {
                oINT32 ok = reportAccept(*pSocket, result);
                if (ok > 0)
                {
                    return eSockStatus_Accept;
                }
                else if (ok < 0)
                {
                    return eSockStatus_Error;
                }
            }
                break;
            case eSockType_Invalid:
                printf("OsnSocketManager::pollResult Error! invalid socket!\n");
                break;
            default:
                if (event.bRead)
                {
                    oINT32 nType;
                    if (eSProtocol_TCP == pSocket->getProtocol())
                    {
                        nType = forwardMessageTcp(*pSocket, result);
                    }
                    else
                    {
                        printf("OsnSocketManager::pollResult Error! no udp protocal!\n");
                        assert(false);
                    } //udp
                    
                    if (event.bWrite && eSockStatus_Close != nType && eSockStatus_Error != nType)
                    {
                        event.bRead = false;
                        --m_nEventIndex;
                    }
                    
                    if (eSockStatus_None == nType)
                    {
                        break;
                    }
                    return nType;
                }
                
				if (event.bWrite)
                {
                    oINT32 nType = sendBuff(*pSocket, result);
                    if (eSockStatus_None == nType)
                    {
                        break;
                    }
                    return nType;
                }
                break;
        }
    }
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
            return eSockStatus_Exit;
        case 'L':
            return listenSocket(*((stRequestListen*)buffer), result);
        case 'S':
            return startSocket(*((stRequestStart*)buffer), result);
		case 'K':
			return closeSocket(*((stRequestClose*)buffer), result);
        case 'D':
            return sendSocket(*((stRequestSend*)buffer), result, eSPriority_High);
        case 'P':
            return sendSocket(*((stRequestSend*)buffer), result, eSPriority_Low);
        case 'O':
            return openSocket(*((stRequestOpen*)buffer), result);
        default:
            printf("OsnSocketManager::ctrlCmd : unknown ctrl %c!\n", type);
            return eSockStatus_None;
    }
    return eSockStatus_None;
}

void OsnSocketManager::blockReadPipe(void *pBuffer, oINT32 sz)
{
    for (;;)
    {
        oINT32 nSize = (oINT32)read(m_nRecvFD, pBuffer, sz);
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
    if (eSockStatus_Close == nType || eSockStatus_Error == nType)
    {
        oINT32 nId = result.id;
        for (oINT32 i = m_nEventIndex; i < m_nEventN; ++i)
        {
            stSocketEvent &event = m_Events[i];
            OsnSocket *pSocket = static_cast<OsnSocket*>(event.socket);
            if (NULL!= pSocket)
            {
                if (eSockType_Invalid == pSocket->getType() && pSocket->getId() == nId)
                {
                    event.socket = NULL;
                    break;
                }
            }
        }
    }
}

oINT32 OsnSocketManager::reportConnect(OsnSocket &socket, stSocketMessage &result)
{
    oINT32 nError;
    socklen_t nErrlen = sizeof(nError);
    oINT32 nCode = getsockopt(socket.getFd(), SOL_SOCKET, SO_ERROR, &nError, &nErrlen);
    if (nCode < 0 || nError)
    {
        forceClose(socket, result);
        if (nCode >= 0)
        {
            result.data = (oINT8*)strerror(nError);
        }
        else
        {
            result.data = (oINT8*)strerror(errno);
        }
        return eSockStatus_Error;
    }
    else
    {
        socket.setType(eSockType_Connected);
        result.opaque = socket.getOpaque();
        result.id = socket.getId();
        result.ud = 0;
        if (socket.isSendBufferEmpty())
        {
            m_pPoll->write(m_nEventFD, socket.getFd(), &socket, false);
        }
        
        sockaddr_all u;
        socklen_t slen = sizeof(u);
        if (getpeername(socket.getFd(), &u.s, &slen))
        {
            void * sin_addr = (u.s.sa_family == AF_INET) ? (void*)&u.v4.sin_addr : (void *)&u.v6.sin6_addr;
            if (inet_ntop(u.s.sa_family, sin_addr, m_Buff, sizeof(m_Buff)))
            {
                result.data = m_Buff;
                return eSockStatus_Open;
            }
        }
        result.data = NULL;
        return eSockStatus_Open;
    }
}

oINT32 OsnSocketManager::reportAccept(OsnSocket &socket, stSocketMessage &result)
{
    sockaddr_all u;
    socklen_t len = sizeof(u);
    oINT32 nClientFd = accept(socket.getFd(), &u.s, &len);
    if (nClientFd < 0)
    {
        if (errno == EMFILE || errno == ENFILE) {
            result.opaque = socket.getOpaque();
            result.id = socket.getId();
            result.ud = 0;
            result.data = strerror(errno);
            return eSockStatus_None;
        }
        else
        {
            return eSockStatus_Data;
        }
    }
    
    oINT32 nId = reserveId();
    if (nId < 0)
    {
        ::close(nClientFd);
        return 0;
    }
    socketKeepAlive(nClientFd);
    m_pPoll->nonBlocking(nClientFd);
    OsnSocket *pSock = newFd(nId, nClientFd, eSProtocol_TCP, socket.getOpaque(), false);
    if (NULL == pSock)
    {
        ::close(nClientFd);
    }
    pSock->setType(eSockType_PAccept);
    result.opaque = socket.getOpaque();
    result.id = socket.getId();
    result.ud = nId;
    result.data = NULL;
    
    void * sin_addr = (u.s.sa_family == AF_INET) ? (void*)&u.v4.sin_addr : (void *)&u.v6.sin6_addr;
    int sin_port = ntohs((u.s.sa_family == AF_INET) ? u.v4.sin_port : u.v6.sin6_port);
    char tmp[INET6_ADDRSTRLEN];
    if (inet_ntop(u.s.sa_family, sin_addr, tmp, sizeof(tmp)))
    {
        snprintf(m_Buff, sizeof(m_Buff), "%s:%d", tmp, sin_port);
        result.data = m_Buff;
    }
    
    return eSockStatus_Close;
}

oINT32 OsnSocketManager::reserveId()
{
    for (oUINT64 i = 0; i < s_u64MaxSocket; ++i)
    {
        oINT32 nId = ATOM_INC(&m_nAllocId);
        if (nId < 0)
        {
            nId = ATOM_AND(&m_nAllocId, 0x7fffffff);
        }
        
        OsnSocket &socket = m_Socket[hashId(nId)];
        if (eSockType_Invalid == socket.getType())
        {
            if (socket.isInvalidAndReserve())
            {
                socket.setId(nId);
                socket.setFd(-1);
                return nId;
            }
            else
            {
                --i;
            }
        }
    }
    return -1;
}

OsnSocket* OsnSocketManager::newFd(oINT32 nId, oINT32 nFd, oINT32 nProtocol, oUINT32 unOpaque, oBOOL bAdd)
{
    OsnSocket &socket = m_Socket[hashId(nId)];
    
    assert(eSockType_Reserve == socket.getType());
    
    if (bAdd)
    {
        if (!m_pPoll->add(m_nEventFD, nFd, &socket))
        {
            socket.setType(eSockType_Invalid);
            return NULL;
        }
    }
    
    socket.setId(nId);
    socket.setFd(nFd);
    socket.setProtocol(nProtocol);
    socket.setTcpSize(s_nMinReadBuffer);
    socket.setOpaque(unOpaque);
    socket.setWBSize(0);
    socket.checkWriteBuff();
    
    return &socket;
}

void OsnSocketManager::socketKeepAlive(oINT32 nFd)
{
    oINT32 nKeepAlive = 1;
    setsockopt(nFd, SOL_SOCKET, SO_KEEPALIVE, &nKeepAlive, sizeof(nKeepAlive));
}

oINT32 OsnSocketManager::hashId(oINT32 nId)
{
    return (((oUINT32)nId) % s_u64MaxSocket);
}

void OsnSocketManager::forceClose(OsnSocket &socket, stSocketMessage &result)
{
    result.id = socket.getId();
    result.ud = 0;
    result.data = NULL;
    result.opaque = socket.getOpaque();
    if (eSockType_Invalid == socket.getType())
    {
        return;
    }
    assert(eSockType_Reserve != socket.getType());
    socket.freeWriteBuff();
    if (eSockType_PAccept != socket.getType() && eSockType_PListen != socket.getType())
    {
        m_pPoll->del(m_nEventFD, socket.getFd());
    }
    if (eSockType_Bind != socket.getType())
    {
        if (::close(socket.getFd()) < 0)
        {
            perror("OsnSocketManager::forceClose Error! close socket : ");
        }
    }
    socket.setType(eSockType_Invalid);
}

oINT32 OsnSocketManager::sendBuff(OsnSocket &socket, stSocketMessage &result)
{
    assert(!listUncomplete(socket.m_queLow));
    if (eSockStatus_Close == sendList(socket, socket.m_queHigh, result))
    {
        return eSockStatus_Close;
    }
    if (socket.m_queHigh.empty())
    {
        if (!socket.m_queLow.empty())
        {
            if (eSockStatus_Close == sendList(socket, socket.m_queLow, result))
            {
                return eSockStatus_Close;
            }
            
            if (listUncomplete(socket.m_queLow))
            {
                socket.raiseUncomplete();
            }
            else
            {
                m_pPoll->write(m_nEventFD, socket.getFd(), &socket, false);
                if (eSockType_Halfclose == socket.getType())
                {
                    forceClose(socket, result);
                    return eSockStatus_Close;
                }
            }
        }
    }
    
    return eSockStatus_None;
}

oBOOL OsnSocketManager::listUncomplete(QUE_WRITE_BUFF_PTR &queWBuff)
{
    if (queWBuff.size() <= 0)
    {
        return false;
    }
    stWriteBuff *pBuff = queWBuff.front();
    if (NULL == pBuff)
    {
        return false;
    }
    
    return pBuff->ptr != pBuff->pBuff;
}

oINT32 OsnSocketManager::sendList(OsnSocket &socket, QUE_WRITE_BUFF_PTR &queWBuff, stSocketMessage &result)
{
    if (eSProtocol_TCP == socket.getProtocol())
    {
        return sendListTcp(socket, queWBuff, result);
    }
    else
    {
        printf("OsnSocketManager::sendList Error! no udp protocal!\n");
        assert(false);
    }
    
    return eSockStatus_None;
}

oINT32 OsnSocketManager::sendListTcp(OsnSocket &socket, QUE_WRITE_BUFF_PTR &queWBuff, stSocketMessage &result)
{

    while (!queWBuff.empty())
    {
        stWriteBuff *pWBuff = queWBuff.front();
        queWBuff.pop();
        
        for (;;)
        {
            oINT32 sz = (oINT32)write(socket.getFd(), pWBuff->ptr, pWBuff->nSz);
            if (sz < 0)
            {
                switch (errno)
                {
                    case EINTR:
                        continue;
                    case AGAIN_WOULDBLOCK:
                        return eSockStatus_None;
                }
            }
            socket.setWBSize(socket.getWBSize() - sz);
            if (sz != pWBuff->nSz)
            {
                pWBuff->ptr += sz;
                pWBuff->nSz -= sz;
                return eSockStatus_None;
            }
        }
        
        SAFE_DELETE(pWBuff);
    }
    return eSockStatus_None;
}

void OsnSocketManager::forwardMessage(oINT32 nType, oBOOL bPadding, stSocketMessage &result)
{
    stOsnSocketMsg *pSM;
    oINT32 sz = sizeof(*pSM);
	oINT32 msg_sz = 0;
    if (bPadding)
    {
        if (NULL != result.data)
        {
            msg_sz = (oINT32)strlen(result.data);
            if (msg_sz > 128)
            {
                msg_sz = 128;
            }
        }
        else
        {
            result.data[0] = '\0';
        }
    }

    if (bPadding)
    {
		pSM = new stOsnSocketMsg(msg_sz);
        memcpy(pSM->pBuffer, result.data, msg_sz);
    }
    else
    {
		pSM = new stOsnSocketMsg(0);
		pSM->pBuffer = result.data;
		pSM->nSize = result.ud;
    }
	pSM->type = nType;
	pSM->id = result.id;
	pSM->ud = result.ud;

    OsnPreparedStatement stmt;
    stmt.setUInt64(0, (oUINT64)pSM);
    oUINT32 unSession = g_ServiceManager.send(result.opaque, ePType_Socket, stmt);
    if (0 == unSession)
    {
        SAFE_DELETE(pSM);
    }
}

oINT32 OsnSocketManager::forwardMessageTcp(OsnSocket &socket, stSocketMessage &result)
{
    oINT32 sz = socket.getTcpSize();
    oINT8 *pBuff = (oINT8*)malloc(sz);
    oINT32 nReadSize = (oINT32)read(socket.getFd(), pBuff, sz);
    if (nReadSize < 0)
    {
        SAFE_FREE(pBuff);
        switch(errno) {
            case EINTR:
                break;
            case AGAIN_WOULDBLOCK:
                fprintf(stderr, "OsnSocketManager::forwardMessageTcp Error! socket-server: EAGAIN capture.\n");
                break;
            default:
                // close when error
                forceClose(socket, result);
                result.data = strerror(errno);
                return eSockStatus_Error;
        }
        return eSockStatus_None;
    }
    
	if (0 == nReadSize)
    {
        SAFE_FREE(pBuff);
        forceClose(socket, result);
        return eSockStatus_Close;
    }
    else
    {
        if (eSockType_Halfclose == socket.getType())
        {
            SAFE_FREE(pBuff);
            return eSockStatus_None;
        }
        
        if (nReadSize == sz)
        {
            socket.setTcpSize(socket.getTcpSize() * 2);
        }
        else if (sz > s_nMinReadBuffer && nReadSize * 2 < sz)
        {
            socket.setTcpSize(socket.getTcpSize() / 2);
        }
        
        result.opaque = socket.getOpaque();
        result.id = socket.getId();
        result.ud = nReadSize;
        result.data = pBuff;
        return eSockStatus_Data;
    }
}

void OsnSocketManager::start(oUINT32 opaque, oINT32 sock)
{
    stRequestPackage request;
    request.u.start.id = sock;
    request.u.start.opaque = opaque;
    sendRequest(request, 'S', sizeof(request.u.start));
}

void OsnSocketManager::close(oUINT32 opaque, oINT32 sock)
{
	stRequestPackage request;
	request.u.close.id = sock;
	request.u.close.shutdown = 0;
	request.u.close.opaque = opaque;
	sendRequest(request, 'K', sizeof(request.u.close));
}

oINT32 OsnSocketManager::listen(oUINT32 opaque, std::string &&strAddr, oINT32 port, oINT32 nBackLog)
{
    oINT32 fd = doListen(strAddr, port, nBackLog);
    if (fd < 0)
    {
        return -1;
    }
    
    stRequestPackage request;
    oINT32 id = reserveId();
    if (id < 0)
    {
        ::close(id);
        return id;
    }
    
    request.u.listen.opaque = opaque;
    request.u.listen.id = id;
    request.u.listen.fd = fd;
    sendRequest(request, 'L', sizeof(request.u.listen));
    return id;
}

oINT32 OsnSocketManager::doListen(std::string &strAddr, oINT32 nPort, oINT32 nBackLog)
{
    oINT32 family = 0;
    oINT32 listenFd = doBind(strAddr, nPort, IPPROTO_TCP, family);
    if (listenFd < 0)
    {
        return -1;
    }
    if (-1 == ::listen(listenFd, nBackLog))
    {
        ::close(listenFd);
    }
    return listenFd;
}


oINT32 OsnSocketManager::doBind(std::string &strAddr, oINT32 nPort, oINT32 protocol, oINT32 &family)
{
    oINT32 fd;
    oINT32 status;
    oINT32 reuse = 1;
    addrinfo ai_hints;
    addrinfo *ai_list = NULL;
    oINT8 szPort[16];
    if ("" == strAddr)
    {
        strAddr = "0.0.0.0";
    }
    sprintf(szPort, "%d", nPort);
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_UNSPEC;
    if (IPPROTO_TCP == protocol)
    {
        ai_hints.ai_socktype = SOCK_STREAM;
    }
    else
    {
        printf("OsnSocketManager::doBind Error! protocal %d not found!\n", protocol);
        assert(false);
    }
    ai_hints.ai_protocol = protocol;
    status = getaddrinfo( strAddr.c_str(), szPort, &ai_hints, &ai_list );
    if ( status != 0 ) {
        const oINT8 *pSz = gai_strerror(status);
        printf("OsnSocketManager::doBind Error! get address info error! errinfo = %s!\n", pSz);
        return -1;
    }
    family = ai_list->ai_family;
    fd = socket(family, ai_list->ai_socktype, 0);
    if (fd < 0)
    {
        freeaddrinfo( ai_list );
        return -1;
    }
    if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)))
    {
        ::close(fd);
        freeaddrinfo( ai_list );
        return -1;
    }
    status = bind(fd, (sockaddr *)ai_list->ai_addr, ai_list->ai_addrlen);
    if (status != 0)
    {
        ::close(fd);
        freeaddrinfo( ai_list );
        return -1;
    }
    
    freeaddrinfo( ai_list );
    return fd;
}

oINT32 OsnSocketManager::listenSocket(stRequestListen &request, stSocketMessage &result)
{
    oINT32 id = request.id;
    oINT32 listenId = request.fd;
    OsnSocket *pSocket = newFd(id, listenId, eSProtocol_TCP, request.opaque, false);
    if (NULL == pSocket)
    {
        ::close(listenId);
        result.opaque = request.opaque;
        result.id = request.id;
        result.ud = 0;
        result.data = NULL;
        m_Socket[hashId(id)].setType(eSockType_Invalid);
        return eSockStatus_Error;
    }
    pSocket->setType(eSockType_PListen);
    return -1;
}

oINT32 OsnSocketManager::startSocket(stRequestStart &request, stSocketMessage &result)
{
    oINT32 id = request.id;
    result.id = id;
    result.opaque = request.opaque;
    result.ud = 0;
    result.data = NULL;
    OsnSocket &socket = m_Socket[hashId(id)];
    if (eSockType_Invalid == socket.getType() || socket.getId() != id)
    {
        result.data = "invalid socket";
        return eSockStatus_Error;
    }
    if (eSockType_PAccept == socket.getType() || eSockType_PListen == socket.getType())
    {
        if (!m_pPoll->add(m_nEventFD, socket.getFd(), &socket))
        {
            forceClose(socket, result);
            result.data = strerror(errno);
            return eSockStatus_Error;
        }
        socket.setType(eSockType_PAccept == socket.getType() ? eSockType_Connected : eSockType_Listen);
        socket.setOpaque(request.opaque);
        result.data = "start";
        return eSockStatus_Open;
    }
    if (eSockType_Connected == socket.getType())
    {
        socket.setOpaque(request.opaque);
        result.data = "transfer";
        return eSockStatus_Open;
    }
    return -1;
}

oINT32 OsnSocketManager::closeSocket(stRequestClose &request, stSocketMessage &result)
{
	oINT32 id = request.id;
	OsnSocket &sock = m_Socket[hashId(id)];
	if (eSockType_Invalid == sock.getType() || sock.getId() != id)
	{
		result.id = id;
		result.opaque = request.opaque;
		result.ud = 0;
		result.data = NULL;
		return eSockStatus_Close;
	}
	if (!sock.isSendBufferEmpty())
	{
		oINT32 type = sendBuff(sock, result);
		if (-1 != type)
		{
			return type;
		}
	}

	if (request.shutdown || sock.isSendBufferEmpty())
	{
		forceClose(sock, result);
		result.id = id;
		result.opaque = request.opaque;
		return eSockStatus_Close;
	}
	sock.setType(eSockType_Halfclose);

	return -1;
}

oINT64 OsnSocketManager::send(oINT32 sock, const void *pBuff, oINT32 sz)
{
    OsnSocket &socket = m_Socket[hashId(sock)];
    
    if (socket.getId() != sock || eSockType_Invalid == socket.getType())
    {
        return -1;
    }
    
    void *pSendBuff = malloc(sz);
    memcpy(pSendBuff, pBuff, sz);
    
    stRequestPackage request;
    request.u.send.id = sock;
    request.u.send.buffer = (oINT8*)pSendBuff;
    request.u.send.sz = sz;
    sendRequest(request, 'D', sizeof(request.u.send));
    return socket.getWBSize();
}

oINT32 OsnSocketManager::connect(oUINT32 opaque, const char *szAddr, oINT32 port)
{
    stRequestPackage request;
    oINT32 len = (oINT32)strlen(szAddr);
    if (len + sizeof(request.u.open) >= 256)
    {
        printf("OsnSocketManager::connect Error! socket-server : Invalid addr %s.\n", szAddr);
        return -1;
    }
    oINT32 id = reserveId();
    if (id < 0)
    {
        return -1;
    }
    request.u.open.opaque = opaque;
    request.u.open.id = id;
    request.u.open.port = port;
    memcpy(request.u.open.host, szAddr, len);
    request.u.open.host[len] = '\0';
    if (len < 0)
    {
        return -1;
    }
    sendRequest(request, 'O', sizeof(request.u.open) + len);
    return request.u.open.id;
}

oINT32 OsnSocketManager::openSocket(stRequestOpen &request, stSocketMessage &result)
{
    oINT32 id = request.id;
    result.opaque = request.opaque;
    result.id = id;
    result.ud = 0;
    result.data = NULL;
    OsnSocket *pSock = NULL;
    addrinfo ai_hints;
    addrinfo *ai_list = NULL;
    addrinfo *ai_ptr = NULL;
    oINT8 port[16];
    sprintf(port, "%d", request.port);
    memset(&ai_hints, 0, sizeof( ai_hints ) );
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;
    
    oINT32 status = getaddrinfo(request.host, port, &ai_hints, &ai_list);
    if (0 != status)
    {
        result.data = (oINT8*)gai_strerror(status);
        freeaddrinfo( ai_list );
        m_Socket[hashId(id)].setType(eSockType_Invalid);
        return eSockStatus_Error;
    }
    oINT32 nSock = -1;
    for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
    {
        nSock = ::socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
        if (nSock < 0)
        {
            continue;
        }
        socketKeepAlive(nSock);
        m_pPoll->nonBlocking(nSock);
        status = ::connect(nSock, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
        if (0 != status && errno != EINPROGRESS)
        {
            ::close(nSock);
            nSock = -1;
            continue;
        }
        break;
    }
    
    if (nSock < 0)
    {
        result.data = strerror(errno);
        freeaddrinfo( ai_list );
        m_Socket[hashId(id)].setType(eSockType_Invalid);
        return eSockStatus_Error;
    }
    
    pSock = newFd(id, nSock, eSProtocol_TCP, request.opaque, true);
    if (NULL == pSock)
    {
        ::close(nSock);
        result.data = "reach skynet socket number limit";
        freeaddrinfo( ai_list );
        m_Socket[hashId(id)].setType(eSockType_Invalid);
        return eSockStatus_Error;
    }
    
    if (0 == status)
    {
        pSock->setType(eSockType_Connected);
        sockaddr *addr = ai_ptr->ai_addr;
        void *sin_addr = (AF_INET == ai_ptr->ai_family) ? (void*)&((sockaddr_in *)addr)->sin_addr : (void*)&((sockaddr_in6 *)addr)->sin6_addr;
        if (inet_ntop(ai_ptr->ai_family, sin_addr, m_Buff, sizeof(m_Buff)))
        {
            result.data = m_Buff;
        }
        freeaddrinfo( ai_list );
        return eSockStatus_Open;
    }
    else
    {
        pSock->setType(eSockType_Connecting);
        m_pPoll->write(m_nEventFD, pSock->getFd(), pSock, true);
    }
    
    freeaddrinfo(ai_list);
    return -1;
}

oINT32 OsnSocketManager::sendSocket(stRequestSend &request, stSocketMessage &result, oINT32 priority)
{
    oINT32 id = request.id;
    OsnSocket &socket = m_Socket[hashId(id)];
    if (socket.getId() != id
        || socket.getType() == eSockType_Invalid
        || socket.getType() == eSockType_Halfclose
        || socket.getType() == eSockType_PAccept)
    {
        SAFE_FREE(request.buffer);
    }
    
    if (socket.getType() == eSockType_PListen || socket.getType() == eSockType_Listen)
    {
        printf("OsnSocketManager::sendSocket Error! write to listen fd %d\n", socket.getId());
        SAFE_FREE(request.buffer);
    }
    
    if (socket.isSendBufferEmpty() && socket.getType() == eSockType_Connected)
    {
        if (eSProtocol_TCP == socket.getProtocol())
        {
            oINT32 n = ::write(socket.getFd(), request.buffer, request.sz);
            if (n < 0)
            {
                switch (errno)
                {
                    case EINTR:
                    case AGAIN_WOULDBLOCK:
                        n = 0;
                        break;
                    default:
                        printf("OsnSocketManager::sendSocket Error! write to %d (fd = %d) error : %s!\n", id, socket.getFd(), strerror(errno));
                        forceClose(socket, result);
                        SAFE_FREE(request.buffer);
                        return eSockStatus_Close;
                }
            }
            if (n == request.sz)
            {
                SAFE_FREE(request.buffer);
                return -1;
            }
            socket.appendSendBuffer(request, n);
        }
        else
        {
            printf("OsnSocketManager::sendSocket Error! no udp protocal!\n");
            assert(false);
        }
    }
    else
    {
        if (eSProtocol_TCP == socket.getProtocol())
        {
            if (eSPriority_Low == priority)
            {
                socket.appendSendBufferLow(request, 0);
            }
            else
            {
                socket.appendSendBuffer(request, 0);
            }
        }
        else
        {
            printf("OsnSocketManager::sendSocket Error! no udp protocal!\n");
            assert(false);
        }
    }
    return -1;
}



