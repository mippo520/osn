//
//  osn_netpack.hpp
//  Gate
//
//  Created by zenghui on 17/6/22.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_netpack_hpp
#define osn_netpack_hpp

#include "osn_socket_head.h"
#include "osn_common.h"

class OsnNetpack
{
public:
    static oINT32 hashFD(oINT32 fd);
    static void pushData(QUE_NETPACK &queNetpack, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize, oBOOL bClone);
    static void pushMore(QUE_NETPACK &queNetpack, LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize);
    static stUncomlete* findUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd);
    static stUncomlete* saveUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd);
    static void closeUncomplete(LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd);
    static oINT32 readSize(oUINT8 *pBuffer);
    static void filterData(QUE_NETPACK &queNetpack, LST_SOCK_UNCOMPLETE &lstUncomplete, oINT32 fd, oUINT8 *pBuffer, oINT32 nSize);
};

#endif /* osn_netpack_hpp */
