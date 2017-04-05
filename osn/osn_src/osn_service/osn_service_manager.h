//
//  osn_service_manager.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_service_manager_hpp
#define osn_service_manager_hpp

#include "osn.h"
#include "osn_singleton.h"
#include <vector>

class OsnService;

class OsnServiceManager {
    const static oINT32 s_nServiceCountBegin = 750;
    static oINT32 s_nServiceIdx;
    OsnServiceManager();
public:
    friend class Singleton<OsnServiceManager>;
    ~OsnServiceManager();
public:
    void init();
    
    template<class T>
    oINT32 startService()
    {
        oINT32 nId = getId();
        if (nId > 0) {
            T *pService = new T();
            addService(nId, pService);
            pService->setId(nId);
        }
        return nId;
    }
    
    void removeService(oINT32 nId);
private:
    oINT32 getId();
    void addService(oINT32 nId, OsnService *pService);
private:
    std::vector<OsnService*> m_vecServices;
};

#define g_ServiceManager Singleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
