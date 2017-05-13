//
//  osn_socket_data.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_data_hpp
#define osn_socket_data_hpp
#include "osn_common.h"
#include <queue>
#include "osn_socket_head.h"

class OsnSocketData
{
    typedef union{
        oINT32 nSize;
    } SIZE;
public:
    OsnSocketData();
    ~OsnSocketData();
    
    oINT32 getTcpSize();
    oBOOL isSendBufferEmpty();
    oBOOL isInvalidAndReserve();
    void freeWriteBuff();
    void checkWriteBuff();
    void raiseUncomplete();
    void setTcpSize(oINT32 nSize);
    void appendSendBuffer(const stRequestSend &request, oINT32 n);
    void appendSendBufferLow(const stRequestSend &request, oINT32 n);
private:
    void freeWriteBuff(QUE_WRITE_BUFF_PTR &listBuff);
    void appendSendBufferLogic(QUE_WRITE_BUFF_PTR &listBuff, const stRequestSend &request, oINT32 n);
public:
    QUE_WRITE_BUFF_PTR m_queHigh;
    QUE_WRITE_BUFF_PTR m_queLow;
private:
    SIZE m_Size;
    MEMBER_VALUE(oINT32, Type);
    MEMBER_VALUE(oINT32, Id);
    MEMBER_VALUE(oINT32, Fd);
    MEMBER_VALUE(oUINT32, Opaque);
    MEMBER_VALUE(oINT32, Protocol);
    MEMBER_VALUE(oINT64, WBSize);
};

#endif /* osn_socket_data_hpp */
