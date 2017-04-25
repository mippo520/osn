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
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1) {
        return false;
    }
    EV_SET(&ke, sock, EVFILT_WRITE, EV_ADD, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1) {
        EV_SET(&ke, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(kfd, &ke, 1, NULL, 0, NULL);
        return false;
    }
    EV_SET(&ke, sock, EVFILT_WRITE, EV_DISABLE, 0, 0, ud);
    if (kevent(kfd, &ke, 1, NULL, 0, NULL) == -1) {
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
