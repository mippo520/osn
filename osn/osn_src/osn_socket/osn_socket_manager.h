//
//  osn_socket.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_manager_hpp
#define osn_socket_manager_hpp
#include <sys/types.h>
#include "osn.h"
#include "osn_singleton.h"
#include "osn_socket.h"
#include "osn_socket_head.h"

class OsnPoll;

class OsnSocketManager
{
    const static oINT32 s_nMaxSocketShift = 16;
    const static oUINT64 s_u64MaxSocket = 1 << s_nMaxSocketShift;
    const static oINT32 s_nMaxEvents = 64;
public:
    ~OsnSocketManager();
    
    void init();
    void exit();
    void release();
    oINT32 poll();
private:
    friend class OsnSingleton<OsnSocketManager>;
    OsnSocketManager();
    oBOOL create();
    void sendRequest(stRequestPackage &request, oINT8 type, oUINT8 len);
    void blockReadPipe(void *pBuffer, oINT32 sz);
    oBOOL hasCmd();
    oINT32 pollResult(stSocketMessage &result, oINT32 &nMore);
    oINT32 ctrlCmd(stSocketMessage &result);
    void clearClosedEvent(stSocketMessage &result, oINT32 nType);
private:
    OsnPoll *m_pPool;
    
    oINT32 m_nEventFD;
    oINT32 m_nRecvFD;
    oINT32 m_nSendFD;
    oBOOL m_bCheckCtrl;
    OsnSocket m_Socket[s_u64MaxSocket];
    fd_set m_fdSet;
    oINT32 m_nEventIndex;
    oINT32 m_nEventN;
    stSocketEvent m_Events[s_nMaxEvents];
};

#define g_SocketManager OsnSingleton<OsnSocketManager>::instance()

#endif /* osn_socket_manager_hpp */
