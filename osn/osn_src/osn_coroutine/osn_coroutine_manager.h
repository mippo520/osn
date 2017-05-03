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
#include "osn.h"
#include "osn_cache_arr_manager.h"
#include "osn_singleton.h"

class OsnCoroutine;

class OsnCoroutineManager {
    static void mainFunc(OsnCoroutineManager *pManager);
    friend class OsnSingleton<OsnCoroutineManager>;
    friend class OsnCoroutine;
    static __thread stCoThreadInfo *s_pThreadInfo;
    
    OsnCoroutineManager();
public:
    ~OsnCoroutineManager();
    
    oUINT32 create(const OSN_COROUTINE_FUNC &func);
    const OSN_CO_ARG& yield(const OSN_CO_ARG &arg = OSN_CO_ARG());
    const OSN_CO_ARG& resume(oUINT32 unId, const OSN_CO_ARG &arg = OSN_CO_ARG());
    oUINT32 running();
    void addThread();
private:
    stCoThreadInfo* getThreadInfo();
    void printThreadInfo();
private:
    OsnCacheArrManager<OsnCoroutine, eThread_Saved> m_arrCoroutine;
//    MAP_CO_THREAD_INFO m_mapInfo;
//    OsnSpinLock m_SpinLock;
};

#define g_CorotineManager OsnSingleton<OsnCoroutineManager>::instance()

#endif /* osn_coroutine_manager_hpp */
