//
//  osn_socket_data.cpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_socket_data.h"
#include "osn_socket_head.h"

OsnSocketData::OsnSocketData()
    : m_Type(eSockType_Invalid)
    , m_Id(0)
    , m_Fd(0)
    , m_Opaque(0)
    , m_Protocol(eSProtocol_TCP)
    , m_WBSize(0)
{
    
}

OsnSocketData::~OsnSocketData()
{
    
}

void OsnSocketData::freeWriteBuff()
{
    freeWriteBuff(m_queHigh);
    freeWriteBuff(m_queLow);
}

void OsnSocketData::freeWriteBuff(QUE_WRITE_BUFF_PTR &queBuff)
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

oBOOL OsnSocketData::isSendBufferEmpty()
{
    return m_queLow.empty() && m_queHigh.empty();
}

void OsnSocketData::raiseUncomplete()
{
    assert(!m_queLow.empty());
    assert(m_queHigh.empty());
    stWriteBuff *pWBuff = m_queLow.front();
    m_queLow.pop();
    m_queHigh.push(pWBuff);
}

oBOOL OsnSocketData::isInvalidAndReserve()
{
    if (ATOM_CAS(&m_Type, eSockType_Invalid, eSockType_Reserve))
    {
        return true;
    }
    return  false;
}

void OsnSocketData::setTcpSize(oINT32 nSize)
{
    m_Size.nSize = nSize;
}

oINT32 OsnSocketData::getTcpSize()
{
    return m_Size.nSize;
}

void OsnSocketData::checkWriteBuff()
{
    assert(m_queLow.empty());
    assert(m_queHigh.empty());
}

void OsnSocketData::appendSendBuffer(const stRequestSend &request, oINT32 n)
{
    appendSendBufferLogic(m_queHigh, request, n);
}

void OsnSocketData::appendSendBufferLow(const stRequestSend &request, oINT32 n)
{
    appendSendBufferLogic(m_queLow, request, n);
}

void OsnSocketData::appendSendBufferLogic(QUE_WRITE_BUFF_PTR &listBuff, const stRequestSend &request, oINT32 n)
{
    stWriteBuff *pWB = new stWriteBuff();
    pWB->ptr = request.buffer + n;
    pWB->nSz = request.sz - n;
    pWB->pBuff = request.buffer;
    listBuff.push(pWB);
}

