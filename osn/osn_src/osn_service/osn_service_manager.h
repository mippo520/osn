//
//  osn_service_manager.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_service_manager_hpp
#define osn_service_manager_hpp


#include <vector>
#include <queue>
#include <mutex>
#include "osn.h"
#include "osn_singleton.h"
#include "osn_arr_manager.h"
#include "osn_service_head.h"


class OsnService;

class OsnServiceManager : public OsnArrManager<OsnService, eThread_Saved> {
    OsnServiceManager();
public:
    friend class OsnSingleton<OsnServiceManager>;
    ~OsnServiceManager();
public:
    oUINT32 send(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement &msg);
public:
    virtual void init();
    
    template<class T>
    oUINT32 startService()
    {
        return makeObj<T>();
    }
    
    OsnService* dispatchMessage(OsnService* pService, oINT32 nWeight);
private:
    friend class OsnService;
    friend class OsnStart;
        
    oUINT32 pushMsg(oUINT32 unTargetId, stServiceMessage &msg);
    OsnService* popWorkingService();
    void pushWarkingService(oUINT32 nId);
private:
    std::queue<oUINT32> m_queHadMsgIds;
};

#define g_ServiceManager OsnSingleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
