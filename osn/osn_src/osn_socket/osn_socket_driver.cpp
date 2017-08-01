//
//  osn_socket_driver.cpp
//  osn
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_socket_driver.h"
#include "I_osn_service.h"
#include "osn_socket_manager.h"

const IOsnSocket *g_SocketDriver = &OsnSingleton<OsnSocketDriver>::instance();

OsnSocketDriver::OsnSocketDriver()
{
    
}

OsnSocketDriver::~OsnSocketDriver()
{
    
}


oINT32 OsnSocketDriver::listen(std::string &&strAddr, oINT32 nPort, oINT32 nBackLog) const
{
    ID_SERVICE svrId = g_Service->getCurService();
    return g_SocketManager.listen(svrId, strAddr, nPort, nBackLog);
}

void OsnSocketDriver::start(oINT32 sock) const
{
    ID_SERVICE svrId = g_Service->getCurService();
    g_SocketManager.start(svrId, sock);
}

void OsnSocketDriver::close(oINT32 sock) const
{
    ID_SERVICE svrId = g_Service->getCurService();
    g_SocketManager.close(svrId, sock);
}

void OsnSocketDriver::shutdown(oINT32 sock) const
{
    ID_SERVICE svrId = g_Service->getCurService();
    g_SocketManager.shutdown(svrId, sock);
}

oINT64 OsnSocketDriver::send(oINT32 sock, const void *pBuff, oINT32 sz) const
{
    return g_SocketManager.send(sock, pBuff, sz);
}

oINT32 OsnSocketDriver::connect(const char *szAddr, oINT32 port) const
{
    ID_SERVICE svrId = g_Service->getCurService();
    return g_SocketManager.connect(svrId, szAddr, port);
}

void OsnSocketDriver::nodelay(oINT32 fd) const
{
    g_SocketManager.nodelay(fd);
}

