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
#include "osn_message.h"


class OsnService;

class OsnServiceManager {
    const static oINT32 s_nServiceCountBegin = 750;
    static oINT32 s_nServiceIdx;
    OsnServiceManager();
public:
    friend class Singleton<OsnServiceManager>;
    ~OsnServiceManager();
public:
    void send(oINT32 nTargetId, oINT32 nValue);
public:
    void init();
    
    template<class T>
    oINT32 startService()
    {
        oINT32 nId = createId();
        if (nId > 0) {
            T *pService = new T();
            addService(nId, pService);
            pService->setId(nId);
            pService->init();
        }
        return nId;
    }
    
    void removeService(oINT32 nId);
    OsnService* dispatchMessage(OsnService* pService, oINT32 nWeight);
private:
    friend class OsnService;
    friend class OsnStart;
    oINT32 createId();
    
    void addService(oINT32 nId, OsnService *pService);
    OsnService* getService(oINT32 nId);
    
    void pushMsg(oINT32 nTargetId, const OsnMessage &msg);
    OsnService* popWorkingService();
    void pushWarkingService(oINT32 nId);
    
    void lock();
    void unlock();
private:
    std::vector<OsnService*> m_vecServices;
    std::queue<oINT32> m_queIds;
    std::queue<oINT32> m_queHadMsgIds;
    std::mutex m_Mutex;
};

#define g_ServiceManager Singleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
