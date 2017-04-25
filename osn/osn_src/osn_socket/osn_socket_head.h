//
//  osn_socket_head.h
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_head_h
#define osn_socket_head_h
#include "osn.h"

enum eSocketStatus
{
    eSStatus_Invalid = 0,
    eSStatus_Reserve,
};

enum eSocketType
{
    eSType_None = -1,
    eSType_Data = 0,
    eSType_Close,
    eSType_Open,
    eSType_Accept,
    eSType_Error,
    eSType_Exit,
};

struct stRequestPackage
{
    oUINT8 header[8];
    union {
        oINT8 buffer[256];
        
    } u;
    oUINT8 dummy[256];
};

struct stSocketMessage
{
    oINT32 id;
    oUINT32 opaque;
    oINT32 ud;	// for accept, ud is new connection id ; for data, ud is size of data
    oINT8 *data;
    
    stSocketMessage()
        : id(0)
        , opaque(0)
        , ud(0)
        , data(NULL)
    {}
    
    void clear()
    {
        opaque = 0;
        id = 0;
        ud = 0;
        data = NULL;
    }
};

struct stSocketEvent
{
    void *socket;
    oBOOL bRead;
    oBOOL bWrite;
    
    stSocketEvent()
        : socket(NULL)
        , bRead(false)
        , bWrite(false)
    {}
};

#endif /* osn_socket_head_h */
