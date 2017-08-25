//
//  osn_kqueue.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_kqueue.h"
#include <sys/event.h>
#include <unistd.h>
#include "osn_socket_head.h"
#include <fcntl.h>

OsnKqueue::OsnKqueue()
{
    
}

OsnKqueue::~OsnKqueue()
{
    
}

oINT32 OsnKqueue::create()
{
    return kqueue();
}

oBOOL OsnKqueue::invalid(oINT32 nfd)
{
    return -1 == nfd;
}

void OsnKqueue::release(oINT32 nfd)
{
    close(nfd);
}

oBOOL OsnKqueue::add(oINT32 kfd, oINT32 sock, void *ud)
{
    struct kevent ke;
    EV_SET(&ke, sock, EVFILT_READ, EV_ADD, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
    {
        return false;
    }
    EV_SET(&ke, sock, EVFILT_WRITE, EV_ADD, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
    {
        EV_SET(&ke, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(kfd, &ke, 1, NULL, 0, NULL);
        return false;
    }
    EV_SET(&ke, sock, EVFILT_WRITE, EV_DISABLE, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
    {
        del(kfd, sock);
        return false;
    }
    return true;
}

void OsnKqueue::del(oINT32 kfd, oINT32 sock)
{
    struct kevent ke;
    EV_SET(&ke, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kfd, &ke, 1, NULL, 0, NULL);
    EV_SET(&ke, sock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(kfd, &ke, 1, NULL, 0, NULL);
}

oINT32 OsnKqueue::wait(oINT32 kfd, stSocketEvent *pEvent, oINT32 nMax)
{
    struct kevent ev[nMax];
    oINT32 n = kevent(kfd, NULL, 0, ev, nMax, NULL);
    
    for (oINT32 i = 0; i < n; ++i)
    {
        pEvent[i].socket = static_cast<OsnSocketData*>(ev[i].udata);
        oUINT32 filter = ev[i].filter;
        pEvent[i].bWrite = (filter == EVFILT_WRITE);
        pEvent[i].bRead = (filter == EVFILT_READ);
        pEvent[i].bError = false;   // kevent has not error event
    }
    
    return n;
}

void OsnKqueue::write(oINT32 kfd, oINT32 sock, void *ud, oBOOL enable)
{
    struct kevent ke;
    EV_SET(&ke, sock, EVFILT_WRITE, enable ? EV_ENABLE : EV_DISABLE, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
    {
        // todo: check error
        printf("OsnKqueue::write Error!\n");
    }
}

void OsnKqueue::nonBlocking(oINT32 nSock)
{
    oINT32 flag = fcntl(nSock, F_GETFL, 0);
    if ( -1 == flag ) {
        return;
    }
    
    fcntl(nSock, F_SETFL, flag | O_NONBLOCK);
}

