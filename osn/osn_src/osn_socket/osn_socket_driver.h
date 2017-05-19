//
//  osn_socket_driver.hpp
//  osn
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_driver_hpp
#define osn_socket_driver_hpp
#include <string>
#include "osn_common.h"
#include "I_osn_socket.h"
#include "osn_singleton.h"

class OsnSocketDriver : public IOsnSocket
{
    friend class OsnSingleton<OsnSocketDriver>;
    OsnSocketDriver();
public:
    ~OsnSocketDriver();
    
    virtual oINT32 listen(std::string &&strAddr, oINT32 nPort, oINT32 nBackLog = s_nBacklog) const;
    virtual void start(oINT32 sock) const;
    virtual void close(oINT32 sock) const;
    virtual void shutdown(oINT32 sock) const;
    virtual oINT64 send(oINT32 sock, const void *pBuff, oINT32 sz) const;
    virtual oINT32 connect(const char *szAddr, oINT32 port) const;
};

#endif /* osn_socket_driver_hpp */
