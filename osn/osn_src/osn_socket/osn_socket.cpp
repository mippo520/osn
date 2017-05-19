#include "osn_socket.h"
#include <string.h>
#include "osn_service_head.h"
#include "I_osn_service.h"
#include "I_osn_coroutine.h"
#include "I_osn_socket.h"

OsnSocket::OsnSocket()
{
}


OsnSocket::~OsnSocket()
{
    UnregistDispatchFunc(ePType_Socket);
    ID_SERVICE svrId = g_Service->getCurService();
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.begin();
    for (; itr != m_mapSocketInfo.end(); ++itr)
    {
        stSocketInfo &info = itr->second;
        clearBuffer(info.buffer);
        g_Socket->close(svrId, info.id);
    }
    
    while (m_queBufferPool.size() > 0)
    {
        stBufferNode *pNode = m_queBufferPool.front();
        SAFE_DELETE(pNode);
        m_queBufferPool.pop();
    }
}

void OsnSocket::init()
{
	RegistDispatchFunc(ePType_Socket, &OsnSocket::dispatchSocket, this);
    m_vecDispatchFunc.resize(eOST_Warning + 1);
	m_vecDispatchFunc[eOST_Data] = std::bind(&OsnSocket::funcSocketData, this, std::placeholders::_1);
	m_vecDispatchFunc[eOST_Connect] = std::bind(&OsnSocket::funcSocketConnect, this, std::placeholders::_1);
	m_vecDispatchFunc[eOST_Accept] = std::bind(&OsnSocket::funcSocketAccept, this, std::placeholders::_1);
	m_vecDispatchFunc[eOST_Error] = std::bind(&OsnSocket::funcSocketError, this, std::placeholders::_1);
	m_vecDispatchFunc[eOST_Close] = std::bind(&OsnSocket::funcSocketClose, this, std::placeholders::_1);
	m_vecDispatchFunc[eOST_Warning] = std::bind(&OsnSocket::funcSocketWarning, this, std::placeholders::_1);
}

oINT32 OsnSocket::open(const oINT8 *addr, oINT32 port, std::string &err)
{
	ID_SERVICE svrId = g_Service->getCurService();
	oINT32 sock = g_Socket->connect(svrId, addr, port);
	return connect(sock, err);
}

oINT64 OsnSocket::write(oINT32 sock, const void *pBuff, oINT32 sz)
{
	return g_Socket->send(sock, pBuff, sz);
}

oINT32 OsnSocket::listen(const oINT8 *addr, oINT32 nPort, oINT32 nBackLog)
{
    ID_SERVICE svrId = g_Service->getCurService();
    return g_Socket->listen(svrId, addr, nPort);
}

oINT32 OsnSocket::start(oINT32 sock, std::string &err, const ACCPET_FUNC &func)
{
    ID_SERVICE svrId = g_Service->getCurService();
    g_Socket->start(svrId, sock);
    return connect(sock, err, func);
}

void OsnSocket::close(oINT32 sock)
{
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        ID_SERVICE svrId = g_Service->getCurService();
        g_Socket->close(svrId, sock);
        return;
    }
    stSocketInfo &info = itr->second;
    if (info.bConnected)
    {
        ID_SERVICE svrId = g_Service->getCurService();
        g_Socket->close(svrId, sock);
        if (info.co > 0)
        {
            assert(0 != info.closing);
            info.closing = g_Coroutine->running();
            g_Osn->wait(info.closing);
        }
        else
        {
            suspend(info);
        }
        info.bConnected = false;
    }
    close_fd(sock);
    m_mapSocketInfo.erase(itr);
}

void OsnSocket::suspend(stSocketInfo &info)
{
	info.co = g_Coroutine->running();
	g_Osn->wait(info.co);
	if (info.closing > 0)
	{
		g_Osn->wakeup(info.closing);
	}
}

void OsnSocket::wakeup(stSocketInfo &info)
{
	ID_COROUTINE co = info.co;
	if (co > 0)
	{
		info.co = 0;
		g_Osn->wakeup(co);
	}
}

oINT32 OsnSocket::connect(oINT32 id, std::string &err, const ACCPET_FUNC &func)
{
	stSocketInfo &info = m_mapSocketInfo[id];
	info.strProtocal = "TCP";
	info.connecting.setBool(true);
    info.func = func;
	suspend(info);
    err = info.connecting.getCharPtr();
    info.connecting.setNull();
    if (info.bConnected)
    {
        return id;
    }
    else
    {
        MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(id);
        if (itr != m_mapSocketInfo.end())
        {
            m_mapSocketInfo.erase(itr);
        }
        return 0;
    }
}

oINT8* OsnSocket::read(oINT32 sock, oINT32 &nSize)
{
    oINT8 *pBuffer;
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::readAll read! socket not found!\n");
        assert(false);
        nSize = 0;
        return NULL;
    }
    stSocketInfo &info = itr->second;
    if (nSize <= 0)
    {
        pBuffer = __readAll(info.buffer, nSize);
        if (NULL != pBuffer)
        {
            return pBuffer;
        }
        
        if (!info.bConnected)
        {
            return pBuffer;
        }
        
        assert(TYPE_NULL == info.readRequired.getType() || !info.readRequired.getBool());
        info.readRequired.setInt32(0);
        suspend(info);
        pBuffer = __readAll(info.buffer, nSize);
        return pBuffer;
    }

    pBuffer = popBuffer(info.buffer, nSize);
    if (NULL != pBuffer)
    {
        return pBuffer;
    }
    
    if (!info.bConnected)
    {
        clearBuffer(info.buffer);
        return NULL;
    }
    
    assert(TYPE_NULL == info.readRequired.getType() || !info.readRequired.getBool());
    info.readRequired.setInt32(nSize);
    suspend(info);
    pBuffer = popBuffer(info.buffer, nSize);
    if (NULL != pBuffer)
    {
        return pBuffer;
    }
    else
    {
        if (info.bConnected)
        {
            printf("OsnSocket::read Error! socket connected but not enough data to read! read size = %d, data size = %d!", nSize, info.buffer.size);
            assert(false);
        }
        clearBuffer(info.buffer);
        return NULL;
    }
}

oINT8* OsnSocket::readAll(oINT32 sock, oINT32 &nSize)
{
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::readAll Error! socket not found!\n");
        assert(false);
        nSize = 0;
        return NULL;
    }
    stSocketInfo &info = itr->second;
    if (!info.bConnected)
    {
        return __readAll(info.buffer, nSize);
    }
    
    assert(TYPE_NULL == info.readRequired.getType() || !info.readRequired.getBool());
    info.readRequired.setBool(true);
    suspend(info);
    assert(!info.bConnected);
    return __readAll(info.buffer, nSize);
}

oINT8* OsnSocket::readLine(oINT32 sock, oINT32 &nSize, const std::string &strSep)
{
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::readLine Error! socket not found!\n");
        assert(false);
        nSize = 0;
        return NULL;
    }
    stSocketInfo &info = itr->second;
    oINT8 *pBuff = __readLine(info.buffer, nSize, strSep);
    if (NULL != pBuff)
    {
        return pBuff;
    }
    
    if (!info.bConnected)
    {
        clearBuffer(info.buffer);
        return NULL;
    }
    
    assert(TYPE_NULL == info.readRequired.getType() || !info.readRequired.getBool());
    info.readRequired.setString(strSep.c_str());
    suspend(info);
    if (info.bConnected)
    {
        return __readLine(info.buffer, nSize, strSep);
    }
    else
    {
        clearBuffer(info.buffer);
        return NULL;
    }
}

void OsnSocket::limit(oINT32 sock, oINT32 limit)
{
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::limit Error! socket not found!\n");
        assert(false);
        return;
    }
    stSocketInfo &info = itr->second;
    info.nBuffLimit = limit;
}

OsnSocket::stBufferNode* OsnSocket::getBufferNode()
{
    stBufferNode *pNode = NULL;
    if (m_queBufferPool.size() > 0)
    {
        pNode = m_queBufferPool.front();
        m_queBufferPool.pop();
    }
    else
    {
        pNode = new stBufferNode();
    }
    return pNode;
}

void OsnSocket::retrieveBufferNode(stBufferNode *pNode)
{
    m_queBufferPool.push(pNode);
}

void OsnSocket::dispatchSocket(const OsnPreparedStatement &stmt)
{
	const stOsnSocketMsg *pSM = (stOsnSocketMsg*)stmt.getUInt64(0);
	if (NULL != pSM)
	{
        if (pSM->type > eOST_None && pSM->type <= eOST_Warning)
        {
            m_vecDispatchFunc[pSM->type](pSM);
        }
	}
    SAFE_DELETE(pSM);
}

oINT32 OsnSocket::pushBuffer(stSocketBuffer &buffer, oINT8 *pBuff, oINT32 sz)
{
    if (NULL == pBuff)
    {
        printf("OsnSocket::push Error! message buffer ptr is NULL!\n");
        return 0;
    }
    
    if (sz <= 0)
    {
        printf("OsnSocket::push Error! message size is %d!\n", sz);
        return 0;
    }
    
    stBufferNode *pNode = getBufferNode();
    if (NULL == pNode)
    {
        printf("OsnSocket::push Error! create buffer node failed!\n");
        return 0;
    }
    pNode->msg = pBuff;
    pNode->sz = sz;
    buffer.listNode.push_back(pNode);
    buffer.size += sz;
    return buffer.size;
}

oINT8* OsnSocket::popBuffer(stSocketBuffer &buffer, oINT32 nSize)
{
    if (buffer.size < nSize)
    {
        return NULL;
    }
    else
    {
        oINT8 *pBuffer = __pop(buffer, nSize, 0);
        buffer.size -= nSize;
        return pBuffer;
    }
}

void OsnSocket::clearBuffer(stSocketBuffer &buffer)
{
    while (buffer.listNode.size() > 0)
    {
        returnFreeNode(buffer);
    }
    buffer.size = 0;
}

void OsnSocket::returnFreeNode(stSocketBuffer &buffer)
{
    stBufferNode *pNode = buffer.listNode.front();
    buffer.offset = 0;
    pNode->clear();
    m_queBufferPool.push(pNode);
    buffer.listNode.pop_front();
}

void OsnSocket::close_fd(oINT32 sock)
{
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(sock);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::close_fd Error! socket not found!\n");
        assert(false);
        return;
    }
    stSocketInfo &info = itr->second;
    clearBuffer(info.buffer);
}

void OsnSocket::shutdown(oINT32 sock)
{
    ID_SERVICE cur = g_Service->getCurService();
    g_Socket->shutdown(cur, sock);
}

oINT8* OsnSocket::__readAll(stSocketBuffer &buffer, oINT32 &nSize)
{
    oINT8 *pBuff = NULL;
    nSize = buffer.size;
    if (nSize > 0)
    {
        pBuff = (oINT8*)malloc(nSize);
        oINT32 nIdx = 0;
        while (buffer.listNode.size() > 0)
        {
            stBufferNode *pNode = buffer.listNode.front();
            oINT32 cpSize = pNode->sz - buffer.offset;
            memcpy(pBuff + nIdx, pNode->msg + buffer.offset, cpSize);
            returnFreeNode(buffer);
            nIdx += cpSize;
        }
        buffer.size = 0;
    }

    return pBuff;
}

oINT8* OsnSocket::__pop(stSocketBuffer &buffer, oINT32 sz, oINT32 skip)
{
    oINT8 *pBuff = (oINT8*)malloc(sz);
    
    do
    {
        stBufferNode *pFront = buffer.listNode.front();
        if (sz < pFront->sz - buffer.offset)
        {
            memcpy(pBuff, pFront->msg + buffer.offset, sz - skip);
            buffer.offset += sz;
            break;
        }
        
        if (pFront->sz - buffer.offset == sz)
        {
            memcpy(pBuff, pFront->msg + buffer.offset, sz - skip);
            returnFreeNode(buffer);
            break;
        }
        
        oINT32 nIdx = 0;
        for (;;)
        {
            oINT32 bytes = pFront->sz - buffer.offset;
            if (sz <= bytes)
            {
                if (sz > skip)
                {
                    memcpy(pBuff + nIdx, pFront->msg + buffer.offset, sz - skip);
                }
                buffer.offset += sz;
                if (bytes == sz)
                {
                    returnFreeNode(buffer);
                }
                break;
            }
            oINT32 realSize = sz - skip;
            if (realSize > 0)
            {
                realSize = (bytes > realSize) ? realSize : bytes;
                memcpy(pBuff + nIdx, pFront->msg + buffer.offset, realSize);
                nIdx += realSize;
            }
            returnFreeNode(buffer);
            sz -= bytes;
            if (0 == sz)
            {
                break;
            }
            assert(buffer.listNode.size() > 0);
            pFront = buffer.listNode.front();
        }
    } while (false);

    
    return pBuff;
}

oINT8* OsnSocket::__readLine(stSocketBuffer &buffer, oINT32 &nSize, const std::string &strSep)
{
    oINT8 *pBuffer = NULL;
    
    oINT32 nSepLen = static_cast<oINT32>(strSep.length());
    LST_BUFFER_NODE_ITR itr = buffer.listNode.begin();
    LST_BUFFER_NODE_ITR itrEnd = buffer.listNode.end();
    if (itr == itrEnd)
    {
        return pBuffer;
    }
    stBufferNode *pNode = *itr;
    oINT32 from = buffer.offset;
    oINT32 bytes = pNode->sz - from;
    for (oINT32 i = 0; i < buffer.size - nSepLen; ++i)
    {
        if (checkSep(itr, buffer.listNode.end(), from, strSep.c_str(), nSepLen))
        {
            if (-1 == nSize)
            {
                return buffer.listNode.front()->msg;
            }
            else
            {
                nSize = i + nSepLen;
                pBuffer = __pop(buffer, nSize, nSepLen);
                buffer.size -= i + nSepLen;
                return pBuffer;
            }
        }
        ++from;
        --bytes;
        if (0 == bytes)
        {
            ++itr;
            if (itr == itrEnd)
            {
                break;
            }
            pNode = *itr;
            from = 0;
            bytes = pNode->sz;
        }
    }
    
    return pBuffer;
}

oBOOL OsnSocket::checkSep(LST_BUFFER_NODE_ITR itrBegin, const LST_BUFFER_NODE_ITR &itrEnd, oINT32 from, const char *szSep, oINT32 sepLen)
{
    stBufferNode *pNode = *itrBegin;
    for (;;)
    {
        oINT32 sz = pNode->sz - from;
        if (sz >= sepLen)
        {
            return memcmp(pNode->msg + from, szSep, sepLen) == 0;
        }
        if (sz > 0)
        {
            if (0 != memcmp(pNode->msg + from, szSep, sz))
            {
                return false;
            }
        }
        
        ++itrBegin;
        if (itrBegin == itrEnd)
        {
            return false;
        }
        pNode = *itrBegin;
        szSep += sz;
        sepLen -= sz;
        from = 0;
    }
}

void OsnSocket::funcSocketData(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        return;
    }
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(msg->id);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::funcSocketData Error! socket: drop package from %d!\n", msg->id);
        msg->clear();
        return;
    }
    printf("OsnSocket::funcSocketData id = %d, ud = %d, size = %d\n", msg->id, msg->ud, msg->nSize);
    stSocketInfo &info = itr->second;
    oINT32 sz = pushBuffer(info.buffer, msg->pBuffer, msg->nSize);
    if (0 == sz)
    {
        printf("OsnSocket::funcSocketData Error! push buffer error!\n");
        msg->clear();
        return;
    }
    oINT32 rrType = info.readRequired.getType();
    if (TYPE_INT32 == rrType)
    {
        if (sz >= info.readRequired.getInt32())
        {
            info.readRequired.setNull();
            wakeup(info);
        }
    }
    else
    {
        if (info.nBuffLimit > 0 && sz > info.nBuffLimit)
        {
            printf("OsnSocket::funcSocketData Error! socket buffer overflow: fd=%d size=%d", msg->id, sz);
            clearBuffer(info.buffer);
            ID_SERVICE svrId = g_Service->getCurService();
            g_Socket->close(svrId, msg->id);
        }
        
        if (TYPE_STRING == rrType)
        {
            oINT32 nSize = -1;
            if (NULL != __readLine(info.buffer, nSize, info.readRequired.getCharPtr()))
            {
                info.readRequired.setNull();
                wakeup(info);
            }
        }
    }
}

void OsnSocket::funcSocketConnect(const stOsnSocketMsg *msg)
{
	oINT32 id = msg->id;
	MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(id);
	if (itr != m_mapSocketInfo.end())
	{
		stSocketInfo &info = itr->second;
		info.bConnected = true;
		wakeup(info);
	}
}

void OsnSocket::funcSocketClose(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        printf("OsnSocket::funcSocketClose Error! Socket Message is NULL!\n");
        return;
    }
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(msg->id);
    if (itr == m_mapSocketInfo.end())
    {
        return;
    }
    
    stSocketInfo &info = itr->second;
    info.bConnected = false;
    wakeup(info);
}

void OsnSocket::funcSocketAccept(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        printf("OsnSocket::funcSocketAccept Error! Socket Message is NULL!\n");
        return;
    }
    
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(msg->id);
    if (itr == m_mapSocketInfo.end())
    {
        ID_SERVICE svrId = g_Service->getCurService();
        g_Socket->close(svrId, msg->ud);
    }
    stSocketInfo &info = itr->second;
    info.func(msg->ud, msg->pBuffer, msg->nSize);
}

void OsnSocket::funcSocketError(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        printf("OsnSocket::funcSocketError Error! Socket Message is NULL!\n");
        return;
    }
    MAP_SOCKET_INFO_ITR itr = m_mapSocketInfo.find(msg->id);
    if (itr == m_mapSocketInfo.end())
    {
        printf("OsnSocket::funcSocketError Error! Socket unknown id = %d, err = %s\n", msg->id, msg->pBuffer);
        return;
    }
    stSocketInfo &info = itr->second;
    if (info.bConnected)
    {
        printf("OsnSocket::funcSocketError Error! error on id = %d, err = %s\n", msg->id, msg->pBuffer);
    }
    else if(info.connecting.getBool())
    {
        info.connecting.setString(msg->pBuffer);
    }
    info.bConnected = false;
    shutdown(msg->id);
    wakeup(info);
}

void OsnSocket::funcSocketWarning(const stOsnSocketMsg *msg)
{
    if (NULL == msg)
    {
        printf("OsnSocket::funcSocketWarning Error! Socket Message is NULL!\n");
        return;
    }
}

void OsnSocket::socketNullFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz)
{
    printf("OsnSocket::socketNullFunc called! fd = %d\n", fd);
}
