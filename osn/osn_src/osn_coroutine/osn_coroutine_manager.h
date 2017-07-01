//
//  osn_coroutine_manager.hpp
//  osn
//
//  Created by zenghui on 17/4/10.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_coroutine_manager_hpp
#define osn_coroutine_manager_hpp
#include "osn_coroutine_head.h"
#include "osn_common.h"
#include "osn_cache_arr_manager.h"
#include "osn_singleton.h"
#include "I_osn_coroutine.h"
#include "osn_spin_lock.h"

class OsnCoroutine;

class OsnCoroutineManager : public IOsnCoroutine
{
    static void mainFunc(OsnCoroutineManager *pManager);
    friend class OsnSingleton<OsnCoroutineManager>;
    friend class OsnCoroutine;
    static __thread stCoThreadInfo *s_pThreadInfo;
    static oUINT64 s_u64CoroutineCount;
    static OsnSpinLock s_CoCountLock;
    
    OsnCoroutineManager();
public:
    ~OsnCoroutineManager();
    
    virtual ID_COROUTINE create(const OSN_COROUTINE_FUNC &func) const;
    virtual oBOOL destroy(ID_COROUTINE co) const;
    virtual const OSN_CO_ARG& yield(const OSN_CO_ARG &arg = OSN_CO_ARG()) const;
    virtual const OSN_CO_ARG& resume(ID_COROUTINE unId, const OSN_CO_ARG &arg = OSN_CO_ARG()) const;
    virtual ID_COROUTINE running() const;
    void addThread();
private:
    stCoThreadInfo* getThreadInfo() const;
private:
    mutable OsnCacheArrManager<OsnCoroutine, eThread_Saved> m_arrCoroutine;
//    MAP_CO_THREAD_INFO m_mapInfo;
//    OsnSpinLock m_SpinLock;
};

#define g_CorotineManager OsnSingleton<OsnCoroutineManager>::instance()

#endif /* osn_coroutine_manager_hpp */
