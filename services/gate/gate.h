//
//  gate.hpp
//  gate
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef gate_hpp
#define gate_hpp
#include "osn_service.h"
#include "I_osn_service_factory.h"
#include "osn_socket_head.h"
#include <map>

class Gate : public OsnService
{
    typedef std::function<void (ID_SERVICE, const OsnPreparedStatement &)> USER_DISPATCH_FUNC;
    struct stConnectInfo
    {
        oINT32 fd;
        oINT32 nClient;
        ID_SERVICE agent;
        std::string strIP;
        
        stConnectInfo()
            : fd(-1)
            , nClient(-1)
            , agent(0)
            , strIP("")
        {}
        
        void unforward()
        {
            nClient = -1;
            agent = 0;
        }
    };
public:
    Gate();
    ~Gate();
private:
    virtual void start(const OsnPreparedStatement &stmt) override;
    void dispatchUser(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt);
    void dispatchSocket(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt);
    // socket
    void funcSocketData(const stOsnSocketMsg *msg);
    void funcSocketConnect(const stOsnSocketMsg *msg);
    void funcSocketClose(const stOsnSocketMsg *msg);
    void funcSocketAccept(const stOsnSocketMsg *msg);
    void funcSocketError(const stOsnSocketMsg *msg);
    void funcSocketWarning(const stOsnSocketMsg *msg);
    // user
    void funcUserOpen(ID_SERVICE source, const OsnPreparedStatement &stmt);
    void funcUserClose(ID_SERVICE source, const OsnPreparedStatement &stmt);
    void funcUserKick(ID_SERVICE source, const OsnPreparedStatement &stmt);
    void funcUserForward(ID_SERVICE source, const OsnPreparedStatement &stmt);
    
    void unforward(stConnectInfo &info);
    void openClient(oINT32 fd);
    void closeClient(oINT32 fd);
    void closeFD(oINT32 fd);
    void closeAgent(oINT32 fd);
private:
    std::vector<SOCKET_MSG_FUNC> m_vecSockDispatchFunc;
    std::vector<USER_DISPATCH_FUNC> m_vecUserDispatchFunc;
    stOsnSockQueue m_sockQueue;
    oINT32 m_SocketId;
    std::map<oINT32, stConnectInfo> m_mapConnect;
    std::map<ID_SERVICE, stConnectInfo*> m_mapForward;
    std::map<oINT32, ID_SERVICE> m_mapFDService;
};

AddService_Declare(Gate)

#endif /* gate_hpp */
