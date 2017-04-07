//
//  osn_service.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_service_h
#define osn_service_h

#include <queue>
#include "osn.h"
#include "osn_message.h"

class OsnService {
public:
    OsnService();
    virtual ~OsnService();
public:
    void init();
    void exit();
private:
    friend class OsnServiceManager;
    virtual oBOOL dispatch();
    void pushMsg(const OsnMessage &msg);
    oUINT32 getMsgSize();
private:
    std::queue<OsnMessage> m_queMsg;
    MEMBER_VALUE(oBOOL, IsInGlobal)
    MEMBER_VALUE(oINT32, Id)
    oINT32 m_nCount;
};

#endif /* osn_service_hpp */
