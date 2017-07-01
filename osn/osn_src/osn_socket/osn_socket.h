#pragma once
#include <map>
#include <list>
#include "osn_common.h"
#include "osn_prepared_statement.h"
#include "osn_socket_head.h"

struct stOsnSocketMsg;
struct stSocketInfo;

class OsnSocket
{
public:
    typedef std::function<void (oINT32 fd, const oINT8 *pBuff, oINT32 sz)> ACCPET_FUNC;
private:
    struct stBufferNode
    {
        oINT8 *msg;
        oINT32 sz;
        
        stBufferNode()
        : msg(NULL)
        , sz(0)
        {}
        
        void clear()
        {
            if(NULL != msg)
            {
                free(msg);
                msg = NULL;
            }
//            SAFE_FREE(msg);
            sz = 0;
        }
        
        ~stBufferNode()
        {
            clear();
        }
    };
    
    typedef std::list<stBufferNode*> LST_BUFFER_NODE;
    typedef LST_BUFFER_NODE::iterator LST_BUFFER_NODE_ITR;
    typedef std::queue<stBufferNode*> QUE_BUFFER_NODE;

    struct stSocketBuffer
    {
        oINT32 size;
        oINT32 offset;
        LST_BUFFER_NODE listNode;
        
        stSocketBuffer()
        : size(0)
        , offset(0)
        {}
        
        ~stSocketBuffer()
        {
            while(listNode.size() > 0)
            {
                stBufferNode *pNode = listNode.front();
                SAFE_DELETE(pNode);
                listNode.pop_front();
            }
        }
    };
    
    struct stSocketInfo
    {
        ID_COROUTINE co;
        oINT32 id;
        oBOOL bConnected;
        PreparedStatementData connecting;
        std::string strProtocal;
        stSocketBuffer buffer;
        ID_COROUTINE closing;
        ACCPET_FUNC func;
        PreparedStatementData readRequired;
        oINT32 nBuffLimit;
        
        stSocketInfo()
        : co(0)
        , id(0)
        , bConnected(false)
        , closing(0)
        , nBuffLimit(-1)
        {}
    };
public:
    OsnSocket();
    ~OsnSocket();
public:
	void init();
	oINT32 open(const oINT8 *addr, oINT32 port, std::string &err);
	oINT64 write(oINT32 sock, const void *pBuff, oINT32 sz);
    oINT32 listen(const oINT8 *addr, oINT32 nPort = 0, oINT32 nBackLog = s_nBacklog);
    oINT32 start(oINT32 sock, std::string &err, const ACCPET_FUNC &func = socketNullFunc);
    void close(oINT32 sock);
    /*
     read函数:
     参数nSize<=0时,读取所有数据,调用结束后nSize赋值为读取的长度
     参数nSize>0时,读取指定长度
     返回值为NULL时,表示socket连接异常
     */
    oINT8* read(oINT32 sock, oINT32 &nSize);
    oINT8* readAll(oINT32 sock, oINT32 &nSize);
    oINT8* readLine(oINT32 sock, oINT32 &nSize, const std::string &strSep = "\n");
    void limit(oINT32 sock, oINT32 limit);
private:
	void suspend(stSocketInfo &info);
	void wakeup(stSocketInfo &info);
	oINT32 connect(oINT32 id, std::string &err, const ACCPET_FUNC &func = socketNullFunc);
    void close_fd(oINT32 sock);
    void shutdown(oINT32 sock);
    stBufferNode* getBufferNode();
    void retrieveBufferNode(stBufferNode *pNode);
    oINT32 pushBuffer(stSocketBuffer &buffer, oINT8 *pBuff, oINT32 sz);
    oINT8* popBuffer(stSocketBuffer &buffer, oINT32 nSize);
    void clearBuffer(stSocketBuffer &buffer);
    oINT8* __readAll(stSocketBuffer &buffer, oINT32 &nSize);
    oINT8* __pop(stSocketBuffer &buffer, oINT32 sz, oINT32 skip);
    oINT8* __readLine(stSocketBuffer &buffer, oINT32 &nSize, const std::string &strSep);
    void returnFreeNode(stSocketBuffer &buffer);
    oBOOL checkSep(LST_BUFFER_NODE_ITR itrBegin, const LST_BUFFER_NODE_ITR &itrEnd, oINT32 from, const char *szSep, oINT32 sepLen);
    
	void dispatchSocket(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt);
	void funcSocketData(const stOsnSocketMsg *msg);
	void funcSocketConnect(const stOsnSocketMsg *msg);
	void funcSocketClose(const stOsnSocketMsg *msg);
	void funcSocketAccept(const stOsnSocketMsg *msg);
	void funcSocketError(const stOsnSocketMsg *msg);
	void funcSocketWarning(const stOsnSocketMsg *msg);

    static void socketNullFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz);
private:
    std::vector<SOCKET_MSG_FUNC> m_vecDispatchFunc;
	typedef std::map<oINT32, stSocketInfo> MAP_SOCKET_INFO;
	typedef MAP_SOCKET_INFO::iterator MAP_SOCKET_INFO_ITR;
	MAP_SOCKET_INFO m_mapSocketInfo;
    QUE_BUFFER_NODE m_queBufferPool;
};

