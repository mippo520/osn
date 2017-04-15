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
#include "osn_arr_manager.h"
#include "osn_coroutine_head.h"
#include "osn_service_head.h"

class OsnService {
    enum eYieldType
    {
        eYT_None = 0,
        eYT_Call,
        eYT_Return,
        eYT_Exit,
        eYT_Response,
    };
public:
    virtual ~OsnService();
private:
    friend class OsnServiceManager;
    friend class OsnArrManager<OsnService, eThread_Saved>;
    OsnService();
    virtual void init();
    virtual void exit();
    virtual oBOOL dispatch();
    oINT32 pushMsg(const OSN_SERVICE_MSG &msg);
    oUINT32 getMsgSize();
private:
    oUINT32 createCO(OSN_COROUTINE_FUNC func);
    void suspend(oINT32 co, const OSN_CO_ARG &arg);
private:
    std::queue<OSN_SERVICE_MSG> m_queMsg;
    MEMBER_VALUE(oBOOL, IsInGlobal)
    MEMBER_VALUE(oUINT32, Id)
    std::queue<oUINT32> m_queCO;
    oINT32 m_nSessionCount;
};

#endif /* osn_service_hpp */
