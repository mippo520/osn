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

class OsnServiceManager : public OsnArrManager<OsnService, eThread_Saved>, public IOsnService {
    static __thread oUINT32 s_unThreadCurService;
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
    virtual oUINT32 getCurService() const;
    virtual void pushWarkingService(oUINT32 unId) const;
    virtual oUINT32 sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement *pMsg) const;
    oUINT32 startService(const std::string &strServiceName);
    void registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC funcPtr);
    void unregistDispatchFunc(oINT32 nPType);
    oUINT32 pushMsg(oUINT32 unTargetId, stServiceMessage *pMsg) const;
    OsnService* popWorkingService();
    void setCurService(oUINT32 unId);
    oUINT32 genId();
    void addServiceFactory(const std::string &strName, IServiceFactory *pFactory);
    void pushDylibHandle(void *handle);
private:
    mutable std::queue<oUINT32> m_queHadMsgIds;
    mutable OsnSpinLock m_QueSpinLock;
    
//    std::map<std::thread::id, oUINT32> m_mapThreadCurService;
//    OsnSpinLock m_CurServiceLock;
    
    oUINT64 m_u64DistroyCount;
    OsnSpinLock m_CountLock;
    
    typedef std::map<std::string, IServiceFactory*> MAP_SERVICE_FACTORY;
    typedef MAP_SERVICE_FACTORY::iterator MAP_SERVICE_FACTORY_ITR;
    MAP_SERVICE_FACTORY m_mapServiceFactory;
    
    std::vector<void*> m_vecDylibHandles;
};

#define g_ServiceManager OsnSingleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
