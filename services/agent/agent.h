//
//  agent.hpp
//  Agent
//
//  Created by zenghui on 17/6/28.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef agent_hpp
#define agent_hpp
#include "osn_service.h"
#include "I_osn_service_factory.h"
#include "osn_socket_head.h"

class Agent : public OsnService
{
public:
    Agent();
    ~Agent();
private:
    virtual void start(const OsnPreparedStatement &stmt) override;
    virtual void exit() override;
    void dispatchUser(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt);
    void dispatchClient(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt);
    void funcUserStart(const OsnPreparedStatement &stmt);
    void funcUserDisconnect(const OsnPreparedStatement &stmt);
private:
    std::vector<VOID_STMT_FUNC> m_vecUserDispatchFunc;
    oINT32 m_nClientFD;
    QUE_NETPACK m_queNetpack;
    LST_SOCK_UNCOMPLETE m_lstUncomplete;
};

AddService_Declare(Agent)

#endif /* agent_hpp */
