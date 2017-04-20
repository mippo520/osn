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
#include <map>
#include <thread>
#include "osn.h"
#include "osn_singleton.h"
#include "osn_arr_manager.h"
#include "osn_service_head.h"
#include "osn_spin_lock.h"


class OsnService;

class OsnServiceManager : public OsnArrManager<OsnService, eThread_Saved> {
    OsnServiceManager();
public:
    friend class OsnSingleton<OsnServiceManager>;
    ~OsnServiceManager();
public:
    virtual void init();
    
    template<class T>
    oUINT32 startService()
    {
        return makeObj<T>();
    }
    
    OsnService* dispatchMessage(OsnService* pService, oINT32 nWeight);
    oUINT32 send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement());
    const OsnPreparedStatement& call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement());
    const OsnPreparedStatement& ret(const OsnPreparedStatement &msg);
    const OsnPreparedStatement& exit();
    void addThread();
    void printThreadInfo();
    void pushWarkingService(oUINT32 unId);
private:
    friend class OsnService;
    friend class OsnStart;
        
    oUINT32 pushMsg(oUINT32 unTargetId, stServiceMessage &msg);
    OsnService* popWorkingService();
    oUINT32 sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement &msg);
    void setCurService(oUINT32 unId);
    oUINT32 getCurService();
private:
    std::queue<oUINT32> m_queHadMsgIds;
    OsnSpinLock m_QueSpinLock;
    
    std::map<std::thread::id, oUINT32> m_mapThreadCurService;
    OsnSpinLock m_CurServiceLock;
    
    oUINT64 m_u64DistroyCount;
    OsnSpinLock m_CountLock;
};

#define g_ServiceManager OsnSingleton<OsnServiceManager>::instance()

#endif /* osn_service_manager_hpp */
