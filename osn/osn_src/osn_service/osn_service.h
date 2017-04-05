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

class OsnMessage;

class OsnService {
public:
    OsnService();
    ~OsnService();
public:
    void pushMsg(OsnMessage* msg);
    void exit();
private:
    std::queue<OsnMessage*> m_QueMsg;
    oBOOL m_isInGlobal;
};

#endif /* osn_service_hpp */
