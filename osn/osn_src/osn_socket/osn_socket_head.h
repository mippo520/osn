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
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>

// EAGAIN and EWOULDBLOCK may be not the same value.
#if (EAGAIN != EWOULDBLOCK)
#define AGAIN_WOULDBLOCK EAGAIN : case EWOULDBLOCK
#else
#define AGAIN_WOULDBLOCK EAGAIN
#endif

enum eSocketStatus
{
    eSStatus_Invalid = 0,
    eSStatus_Reserve,
    eSStatus_PListen,
    eSStatus_Listen,
    eSStatus_Connecting,
    eSStatus_Connected,
    eSStatus_Halfclose,
    eSStatus_PAccept,
    eSStatus_Bind,
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

enum eOsnSocketType
{
    eOST_None = 0,
    eOST_Data,
    eOST_Connect,
    eOST_Close,
    eOST_Accept,
    eOST_Error,
    eOST_Udp,
    eOST_Warning
};

enum eSocketProtocol
{
    eSProtocol_TCP = 0,
};

struct stRequestOpen {
    oINT32 id;
    oINT32 port;
    oUINT32 opaque;
    oINT8 host[1];
};

struct stRequestSend {
    oINT32 id;
    oINT32 sz;
    oINT8 *buffer;
};

struct stRequestClose {
    oINT32 id;
    oINT32 shutdown;
    oUINT32 opaque;
};

struct stRequestListen {
    oINT32 id;
    oINT32 fd;
    oUINT32 opaque;
    oINT8 host[1];
};

struct stRequestBind {
    oINT32 id;
    oINT32 fd;
    oUINT32 opaque;
};

struct stRequestStart {
    oINT32 id;
    oUINT32 opaque;
};

struct stRequestSetopt {
    oINT32 id;
    oINT32 what;
    oINT32 value;
};

struct stRequestPackage
{
    oUINT8 header[8];
    union {
        oINT8 buffer[256];
        stRequestOpen open;
        stRequestSend send;
        stRequestClose close;
        stRequestListen listen;
        stRequestBind bind;
        stRequestStart start;
        stRequestSetopt setopt;
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

struct stOsnSocketMsg
{
    oINT32 type;
    oINT32 id;
    oINT32 ud;
    oINT8 *pBuffer;
	oINT32 nSize;
    
    stOsnSocketMsg(oINT32 sz)
        : type(0)
        , id(0)
        , ud(0)
        , pBuffer(NULL)
		, nSize(sz)
    {
		if (sz > 0)
		{
			pBuffer = (oINT8*)malloc(sz);
		}
		else
		{
			pBuffer = NULL;
		}
	}
    
    ~stOsnSocketMsg()
    {
		SAFE_FREE(pBuffer);
    }
};

struct stWriteBuff
{
    void *pBuff;
    oINT8 *ptr;
    oINT32 nSz;
    oBOOL bUserObject;
    
    stWriteBuff()
        : pBuff(NULL)
        , ptr(NULL)
        , nSz(0)
        , bUserObject(false)
    {}
};

typedef std::queue<stWriteBuff*> QUE_WRITE_BUFF_PTR;

class OsnSocket;

struct stSocketEvent
{
    OsnSocket *socket;
    oBOOL bRead;
    oBOOL bWrite;
    
    stSocketEvent()
        : socket(NULL)
        , bRead(false)
        , bWrite(false)
    {}
};

union sockaddr_all
{
    sockaddr s;
    sockaddr_in v4;
    sockaddr_in6 v6;
};

#endif /* osn_socket_head_h */
