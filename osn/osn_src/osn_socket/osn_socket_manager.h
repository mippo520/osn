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
#include <string>
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
    const static oINT32 s_nMaxInfo = 128;
    const static oINT32 s_nMinReadBuffer = 64;
    const static oINT32 s_nBacklog = 32;
public:
    ~OsnSocketManager();
    
    void init();
    void exit();
    void release();
    oINT32 poll();
    oINT32 listen(oUINT32 opaque, std::string &&strAddr, oINT32 nPort, oINT32 nBackLog = s_nBacklog);
    void start(oUINT32 opaque, oINT32 sock);
private:
    friend class OsnSingleton<OsnSocketManager>;
    OsnSocketManager();
    oBOOL create();
    void sendRequest(stRequestPackage &request, oINT8 type, oUINT8 len);
    void blockReadPipe(void *pBuffer, oINT32 sz);
    oBOOL hasCmd();
    oINT32 pollResult(stSocketMessage &result, oBOOL &bMore);
    oINT32 ctrlCmd(stSocketMessage &result);
    void clearClosedEvent(stSocketMessage &result, oINT32 nType);
    void forceClose(OsnSocket &socket, stSocketMessage &result);
    oINT32 reportConnect(OsnSocket &socket, stSocketMessage &result);
    oINT32 reportAccept(OsnSocket &socket, stSocketMessage &result);
    oINT32 sendBuff(OsnSocket &socket, stSocketMessage &result);
    oBOOL listUncomplete(QUE_WRITE_BUFF_PTR &queWBuff);
    oINT32 sendList(OsnSocket &socket, QUE_WRITE_BUFF_PTR &queWBuff, stSocketMessage &result);
    oINT32 sendListTcp(OsnSocket &socket, QUE_WRITE_BUFF_PTR &queWBuff, stSocketMessage &result);
    oINT32 reserveId();
    static oINT32 hashId(oINT32 nId);
    void socketKeepAlive(oINT32 nFd);
    OsnSocket* newFd(oINT32 nId, oINT32 nFd, oINT32 nProtocol, oUINT32 unOpaque, oBOOL bAdd);
    void forwardMessage(oINT32 nType, oBOOL bPadding, stSocketMessage &result);
    oINT32 forwardMessageTcp(OsnSocket &socket, stSocketMessage &result);
    oINT32 doListen(std::string &strAddr, oINT32 nPort, oINT32 nBackLog);
    oINT32 doBind(std::string &strAddr, oINT32 nPort, oINT32 protocol, oINT32 &family);
    oINT32 listenSocket(stRequestListen &request, stSocketMessage &result);
    oINT32 startSocket(stRequestStart &request, stSocketMessage &result);
private:
    OsnPoll *m_pPoll;
    
    oINT32 m_nEventFD;
    oINT32 m_nRecvFD;
    oINT32 m_nSendFD;
    oBOOL m_bCheckCtrl;
    OsnSocket m_Socket[s_u64MaxSocket];
    fd_set m_fdSet;
    oINT32 m_nEventIndex;
    oINT32 m_nEventN;
    stSocketEvent m_Events[s_nMaxEvents];
    oINT8 m_Buff[s_nMaxInfo];
    oINT32 m_nAllocId;
};

#define g_SocketManager OsnSingleton<OsnSocketManager>::instance()

#endif /* osn_socket_manager_hpp */
