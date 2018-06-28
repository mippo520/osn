//
//  osn_netpack.cpp
//  Gate
//
//  Created by zenghui on 17/6/22.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_netpack.h"
#include <string.h>

oINT32 OsnNetpack::hashFD(oINT32 fd)
{
    oINT32 a = fd >> 24;
    oINT32 b = fd >> 12;
    oINT32 c = fd;
    return (oINT32)((oUINT32)(a + b + c) % OSN_NETPACK_HASHSIZE);
}


stUncomlete* OsnNetpack::findUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd)
{
    stUncomlete *pUncomplete = NULL;
    do
    {
        if (lstUncomplete.size() <= 0)
        {
            break;
        }
        
        LST_SOCK_UNCOMPLETE::iterator itr = lstUncomplete.begin();
        for (; itr != lstUncomplete.end(); ++itr)
        {
            stUncomlete *pUC = *itr;
            if (pUC->pack.id == fd)
            {
                pUncomplete = pUC;
                lstUncomplete.erase(itr);
                break;
            }
        }
    }
    while (false);
    
    return pUncomplete;
}

stUncomlete* OsnNetpack::saveUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd)
{
    stUncomlete *pUC = new stUncomlete();
    pUC->pack.id = fd;
    lstUncomplete.push_back(pUC);
    return pUC;
}

void OsnNetpack::closeUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd)
{
    stUncomlete *pUC = findUncomplete(lstUncomplete, fd);
    if (NULL != pUC)
    {
        SAFE_FREE(pUC->pack.pBuffer);
        SAFE_FREE(pUC);
    }
}

void OsnNetpack::pushData(QUE_NETPACK &queNetpack, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize, oBOOL bClone)
{
    if (bClone)
    {
        oUINT8 *pTmp = (oUINT8*)malloc(nSize);
        memcpy(pTmp, pBuffer, nSize);
        pBuffer = pTmp;
    }
    
    stNetPack netpack;
    netpack.id = fd;
    netpack.pBuffer = pBuffer;
    netpack.size = nSize;
    queNetpack.push(netpack);
}

void OsnNetpack::pushMore(QUE_NETPACK &queNetpack, LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize)
{
    if (1 == nSize)
    {
        stUncomlete *pUC = saveUncomplete(lstUncomplete, fd);
        pUC->read = -1;
        pUC->header = *pBuffer;
        return;
    }
    
    oINT32 nPackSize = readSize(pBuffer);
    pBuffer += 2;
    nSize -= 2;
    
    if (nSize < nPackSize)
    {
        stUncomlete *pUC = saveUncomplete(lstUncomplete, fd);
        pUC->read = nSize;
        pUC->pack.size = nPackSize;
        pUC->pack.pBuffer = (oUINT8*)malloc(nPackSize);
        memcpy(pUC->pack.pBuffer, pBuffer, nSize);
        return;
    }
    pushData(queNetpack, fd, pBuffer, nPackSize, true);
    
    pBuffer += nPackSize;
    nSize -= nPackSize;
    if (nSize > 0)
    {
        pushMore(queNetpack, lstUncomplete, fd, pBuffer, nSize);
    }
}

oINT32 OsnNetpack::readSize(oUINT8 *pBuffer)
{
    oINT32 sz = (oINT32)pBuffer[0] << 8 | (oINT32)pBuffer[1];
    return sz;
}

void OsnNetpack::filterData_(QUE_NETPACK &queNetpack, LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize)
{
    stUncomlete *pUC = findUncomplete(lstUncomplete, fd);
    if (NULL != pUC)
    {
        if (pUC->read < 0)
        {
            assert(-1 == pUC->read);
            oINT32 nPackSize = *pBuffer;
            nPackSize |= pUC->header << 8;
            ++pBuffer;
            --nSize;
            pUC->pack.size = nPackSize;
            pUC->pack.pBuffer = (oUINT8*)malloc(nPackSize);
            pUC->read = 0;
        }
        oINT32 nNeed = pUC->pack.size - pUC->read;
        if (nSize < nNeed)
        {
            memcpy(pUC->pack.pBuffer + pUC->read, pBuffer, nSize);
            pUC->read += nSize;
            lstUncomplete.push_front(pUC);
            return;
        }
        memcpy(pUC->pack.pBuffer + pUC->read, pBuffer, nNeed);
        pBuffer += nNeed;
        nSize -= nNeed;
        pushData(queNetpack, fd, pUC->pack.pBuffer, pUC->pack.size, false);
        SAFE_DELETE(pUC);
        if (0 == nSize)
        {
            return;
        }
        
        //还有多余的
        pushMore(queNetpack, lstUncomplete, fd, pBuffer, nSize);
    }
    else
    {
        if (1 == nSize)
        {
            stUncomlete *pUC = saveUncomplete(lstUncomplete, fd);
            pUC->read = -1;
            pUC->header = *pBuffer;
            return;
        }
        
        oINT32 nPackSzie = readSize(pBuffer);
        pBuffer += 2;
        nSize -= 2;
        if (nSize < nPackSzie)
        {
            stUncomlete *pUC = saveUncomplete(lstUncomplete, fd);
            pUC->read = nSize;
            pUC->pack.size = nPackSzie;
            pUC->pack.pBuffer = (oUINT8*)malloc(nPackSzie);
            memcpy(pUC->pack.pBuffer, pBuffer, nSize);
            return;
        }
        
        pushData(queNetpack, fd, pBuffer, nPackSzie, true);
        if(nSize == nPackSzie)
        {
            return;
        }
        
        pBuffer += nPackSzie;
        nSize -= nPackSzie;
        pushMore(queNetpack, lstUncomplete, fd, pBuffer, nSize);
        return;
    }
}

void OsnNetpack::filterData(QUE_NETPACK &queNetpack, LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize)
{
    filterData_(queNetpack, lstUncomplete, fd, pBuffer, nSize);
    SAFE_FREE(pBuffer);
}


