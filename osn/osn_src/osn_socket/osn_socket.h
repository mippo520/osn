//
//  osn_socket.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_hpp
#define osn_socket_hpp
#include "osn.h"
#include <queue>
#include "osn_socket_head.h"

class OsnSocket
{
    typedef union{
        oINT32 nSize;
    } SIZE;
public:
    OsnSocket();
    ~OsnSocket();
    
    void freeWriteBuff();
    void checkWriteBuff();
    oBOOL isSendBufferEmpty();
    void raiseUncomplete();
    oBOOL isInvalidAndReserve();
    void setTcpSize(oINT32 nSize);
    oINT32 getTcpSize();
    void appendSendBuffer(const stRequestSend &request, oINT32 n);
    void appendSendBufferLow(const stRequestSend &request, oINT32 n);
private:
    void freeWriteBuff(QUE_WRITE_BUFF_PTR &listBuff);
    void appendSendBufferLogic(QUE_WRITE_BUFF_PTR &listBuff, const stRequestSend &request, oINT32 n);
public:
    QUE_WRITE_BUFF_PTR m_queHigh;
    QUE_WRITE_BUFF_PTR m_queLow;
private:
    MEMBER_VALUE(oINT32, Type);
    MEMBER_VALUE(oINT32, Id);
    MEMBER_VALUE(oINT32, Fd);
    MEMBER_VALUE(oUINT32, Opaque);
    MEMBER_VALUE(oINT32, Protocol);
    MEMBER_VALUE(oINT64, WBSize);
    SIZE m_Size;
};

#endif /* osn_socket_hpp */
