//
//  I_osn_socket.h
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_socket_h
#define I_osn_socket_h

const static oINT32 s_nBacklog = 32;

class IOsnSocket
{
public:
    virtual oINT32 listen(std::string &&strAddr, oINT32 nPort, oINT32 nBackLog = s_nBacklog) const = 0;
    virtual void start(oINT32 sock) const = 0;
    virtual void close(oINT32 sock) const = 0;
    virtual void shutdown(oINT32 sock) const = 0;
    virtual oINT64 send(oINT32 sock, const void *pBuff, oINT32 sz) const = 0;
    virtual oINT32 connect(const char *szAddr, oINT32 port) const = 0;
    virtual void nodelay(oINT32 fd) const = 0;
};

extern const IOsnSocket *g_SocketDriver;

#endif /* I_osn_socket_h */
