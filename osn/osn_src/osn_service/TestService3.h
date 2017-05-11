//
//  TestService3.hpp
//  osn
//
//  Created by zenghui on 17/5/3.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef TestService3_hpp
#define TestService3_hpp

#include "osn_service.h"
#include "osn_socket.h"

class TestService3 : public OsnService
{
public:
    TestService3();
    ~TestService3();
public:
    virtual void start(const OsnPreparedStatement &stmt) override;
    virtual void exit() override;
    void dispatchLua(const OsnPreparedStatement &stmt);
private:
    void acceptFunc(oINT32 fd, const oINT8 *pBuffer, oINT32 sz);
private:
	OsnSocket m_Socket;
    oINT32 m_SockId;
    oINT32 m_fd;
    oUINT32 m_curCO;
};

#endif /* TestService3_hpp */
