//
//  I_osn_socket.h
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_socket_h
#define I_osn_socket_h
#include "osn_socket_head.h"

class IOsnSocket
{
public:
    virtual oINT32 listen(oUINT32 opaque, std::string &&strAddr, oINT32 nPort, oINT32 nBackLog = s_nBacklog) const = 0;
    virtual void start(oUINT32 opaque, oINT32 sock) const = 0;
    virtual void close(oUINT32 opaque, oINT32 sock) const = 0;
    virtual void shutdown(oUINT32 opaque, oINT32 sock) const = 0;
    virtual oINT64 send(oINT32 sock, const void *pBuff, oINT32 sz) const = 0;
    virtual oINT32 connect(oUINT32 opaque, const char *szAddr, oINT32 port) const = 0;
};

extern const IOsnSocket *g_Socket;

#endif /* I_osn_socket_h */
