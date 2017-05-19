//
//  Service.hpp
//  osn
//
//  Created by zenghui on 17/5/3.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef Service_hpp
#define Service_hpp

#include "osn_service.h"
#include "osn_socket.h"
#include "I_osn_service_factory.h"

class TestService : public OsnService
{
public:
	TestService();
    ~TestService();
public:
    virtual void start(const OsnPreparedStatement &stmt) override;
    void dispatchLua(const OsnPreparedStatement &stmt);
private:
    void acceptFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz);
private:
	OsnSocket m_Socket;
    oINT32 m_SockId;
    oINT32 m_fd;
    ID_COROUTINE m_curCO;
};

AddService_Declare(TestService)

#endif /* Service_hpp */
