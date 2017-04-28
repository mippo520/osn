//
//  osn_socket.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_socket.h"
#include "osn_socket_head.h"

OsnSocket::OsnSocket()
    : m_Type(eSStatus_Invalid)
    , m_Id(0)
    , m_Fd(0)
    , m_Opaque(0)
    , m_Protocol(eSProtocol_TCP)
    , m_WBSize(0)
{
    
}

OsnSocket::~OsnSocket()
{
    
}

void OsnSocket::freeWriteBuff()
{
    freeWriteBuff(m_queHigh);
    freeWriteBuff(m_queLow);
}

void OsnSocket::freeWriteBuff(QUE_WRITE_BUFF_PTR &queBuff)
{
    while (!queBuff.empty())
    {
        stWriteBuff *pWB = queBuff.front();
        
        if (!pWB->bUserObject)
        {
            SAFE_FREE(pWB->pBuff);
        }
        SAFE_DELETE(pWB);
        
        queBuff.pop();
    }
}

oBOOL OsnSocket::isSendBuffEmpty()
{
    return m_queLow.empty() && m_queHigh.empty();
}

void OsnSocket::raiseUncomplete()
{
    assert(!m_queLow.empty());
    assert(m_queHigh.empty());
    stWriteBuff *pWBuff = m_queLow.front();
    m_queLow.pop();
    m_queHigh.push(pWBuff);
}

oBOOL OsnSocket::isInvalidAndReserve()
{
    if (ATOM_CAS(&m_Type, eSStatus_Invalid, eSStatus_Reserve))
    {
        return true;
    }
    return  false;
}

void OsnSocket::setTcpSize(oINT32 nSize)
{
    m_Size.nSize = nSize;
}

oINT32 OsnSocket::getTcpSize()
{
    return m_Size.nSize;
}

void OsnSocket::checkWriteBuff()
{
    assert(m_queLow.empty());
    assert(m_queHigh.empty());
}


