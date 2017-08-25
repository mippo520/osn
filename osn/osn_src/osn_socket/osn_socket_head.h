//
//  osn_socket_head.h
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_head_h
#define osn_socket_head_h
#include "osn_common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include <list>
#include <string>
#include "osn_prepared_statement.h"
#include "I_osn_socket.h"

// EAGAIN and EWOULDBLOCK may be not the same value.
#if (EAGAIN != EWOULDBLOCK)
#define AGAIN_WOULDBLOCK EAGAIN : case EWOULDBLOCK
#else
#define AGAIN_WOULDBLOCK EAGAIN
#endif

enum eSocketType
{
    eSockType_Invalid = 0,
    eSockType_Reserve,
    eSockType_PListen,
    eSockType_Listen,
    eSockType_Connecting,
    eSockType_Connected,
    eSockType_Halfclose,
    eSockType_PAccept,
    eSockType_Bind,
};

enum eSocketStatus
{
    eSockStatus_None = -1,
    eSockStatus_Data = 0,
    eSockStatus_Close,
    eSockStatus_Open,
    eSockStatus_Accept,
    eSockStatus_Error,
    eSockStatus_Exit,
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

enum eSocketPriority
{
    eSPriority_High = 0,
    eSPriority_Low,
};

struct stRequestOpen {
    oINT32 id;
    oINT32 port;
    ID_SERVICE opaque;
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
    ID_SERVICE opaque;
};

struct stRequestListen {
    oINT32 id;
    oINT32 fd;
    ID_SERVICE opaque;
    oINT8 host[1];
};

struct stRequestBind {
    oINT32 id;
    oINT32 fd;
    ID_SERVICE opaque;
};

struct stRequestStart {
    oINT32 id;
    ID_SERVICE opaque;
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
    ID_SERVICE opaque;
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
    oINT32 ud;  // for accept, ud is new connection id
    mutable oINT8 *pBuffer;
	mutable oINT32 nSize;
    
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
	}
    
    void clear() const
    {
        SAFE_FREE(pBuffer);
        nSize = 0;
    }
};

typedef std::function<void(const stOsnSocketMsg *)> SOCKET_MSG_FUNC;

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

class OsnSocketData;

struct stSocketEvent
{
    OsnSocketData *socket;
    oBOOL bRead;
    oBOOL bWrite;
    oBOOL bError;
    
    stSocketEvent()
        : socket(NULL)
        , bRead(false)
        , bWrite(false)
        , bError(false)
    {}
};

union sockaddr_all
{
    sockaddr s;
    sockaddr_in v4;
    sockaddr_in6 v6;
};

#define OSN_NETPACK_QUEUESIZE 1024
#define OSN_NETPACK_HASHSIZE 4096
#define OSN_NETPACK_SMALLSTRING 2048

struct stNetPack
{
    oINT32 id;
    oINT32 size;
    oUINT8 *pBuffer;
    
    stNetPack()
        : id(-1)
        , size(0)
        , pBuffer(NULL)
    {}
    
};

struct stUncomlete
{
    stNetPack pack;
    oINT32 read;
    oINT32 header;
    
    stUncomlete()
        : read(0)
        , header(0)
    {}
};

typedef std::list<stUncomlete*> LST_SOCK_UNCOMPLETE;
typedef std::queue<stNetPack> QUE_NETPACK;

struct stOsnSockQueue
{
    LST_SOCK_UNCOMPLETE hash[OSN_NETPACK_HASHSIZE];
    QUE_NETPACK queNetpack;
};

#define SOCKET_START_FUNC_BIND() std::bind(&TestService3::acceptFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

struct stSocketAddr
{
    std::string strHost;
    oINT32 nPort;
};

class OsnSocketChannel;

struct stChannelDesc
{
    std::string strHost;
    oINT32 nPort;
    oBOOL bSocket;
    oBOOL bClosed;
    ID_COROUTINE dispatchThread;
    ID_COROUTINE connectingThread;
    ID_COROUTINE authCoroutine;
    oINT32  nSock;
    std::queue<ID_COROUTINE> queConnecting;
    oBOOL bNodelay;
    std::vector<stSocketAddr> vecBackup;
    VOID_STMT_FUNC response;
    std::function<oBOOL (OsnSocketChannel*, std::string &strErr)> auth;
    
    stChannelDesc()
    : strHost("")
    , nPort(0)
    , bSocket(false)
    , bClosed(false)
    , dispatchThread(0)
    , connectingThread(0)
    , authCoroutine(0)
    , nSock(0)
    , bNodelay(false)
    , response(nullptr)
    , auth(nullptr)
    {}
};

#endif /* osn_socket_head_h */
