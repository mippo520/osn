//
//  osn_service_manager.hpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_service_manager_hpp
#define osn_service_manager_hpp


#include <vector>
#include <queue>
#include <mutex>
#include <map>
#include <thread>
#include "osn_common.h"
#include "osn_singleton.h"
#include "osn_arr_manager.h"
#include "osn_service_head.h"
#include "osn_spin_lock.h"
#include "I_osn_service.h"

class OsnService;
class IServiceFactory;

class OsnServiceManager : public OsnArrManager<OsnService, eThread_Saved>, public IOsnService
{
    static __thread ID_SERVICE s_u64ThreadCurService;
    friend class OsnSingleton<OsnServiceManager>;
    friend class OsnService;
    friend class OsnStart;
    friend class Osn;
    OsnServiceManager();
public:
    ~OsnServiceManager();
public:
    virtual void init();
    
    OsnService* dispatchMessage(OsnService* pService, oINT32 nWeight);
    void addThread();
private:
    virtual ID_SERVICE getCurService() const;
    virtual void pushWarkingService(ID_SERVICE unId) const;
    virtual ID_SESSION sendMessage(ID_SERVICE unTargetId, ID_SERVICE unSource, oINT32 type, ID_SESSION unSession, const OsnPreparedStatement &msg = OsnPreparedStatement()) const;
    ID_SERVICE startService(const std::string &strServiceName, const OsnPreparedStatement &stmt);
    void registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC funcPtr);
    void unregistDispatchFunc(oINT32 nPType);
    OsnService* popWorkingService();
    void setCurService(ID_SERVICE unId);
    ID_SESSION genId();
    void addServiceFactory(const std::string &strName, IServiceFactory *pFactory);
    void pushDylibHandle(void *handle);
private:
    mutable std::queue<ID_SERVICE> m_queHadMsgIds;
    mutable OsnSpinLock m_QueSpinLock;
    
    oUINT64 m_u64DistroyCount;
    OsnSpinLock m_CountLock;
    
    typedef std::map<std::string, IServiceFactory*> MAP_SERVICE_FACTORY;
    typedef MAP_SERVICE_FACTORY::iterator MAP_SERVICE_FACTORY_ITR;
    MAP_SERVICE_FACTORY m_mapServiceFactory;
    
    std::vector<void*> m_vecDylibHandles;
};

#define g_ServiceManager OsnSingleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
