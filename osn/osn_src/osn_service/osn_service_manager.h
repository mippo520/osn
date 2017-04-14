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
    oINT32 send(oINT32 nTargetId, const OSN_SERVICE_MSG &msg);
public:
    virtual void init();
    
    template<class T>
    oINT32 startService()
    {
        return makeObj<T>();
    }
    
    OsnService* dispatchMessage(OsnService* pService, oINT32 nWeight);
private:
    friend class OsnService;
    friend class OsnStart;
        
    oINT32 pushMsg(oINT32 nTargetId, const OSN_SERVICE_MSG &msg);
    OsnService* popWorkingService();
    void pushWarkingService(oINT32 nId);
private:
    std::queue<oINT32> m_queHadMsgIds;
};

#define g_ServiceManager OsnSingleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
