//
//  osn_coroutine.hpp
//  osn
//
//  Created by liqing on 17/4/10.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_coroutine_hpp
#define osn_coroutine_hpp
#include "osn_coroutine_head.h"
#include "osn.h"
#include "osn_cache_arr_manager.h"
#include "osn_arr_manager.h"

class OsnCoroutine {
    const static oINT32 s_nStackSize = 128 * 1024;
private:
    friend class OsnCoroutineManager;
    friend class OsnCacheArrManager<OsnCoroutine, eThread_Saved>;
    friend class OsnArrManager<OsnCoroutine, eThread_Saved>;
    OsnCoroutine();
    ~OsnCoroutine();
    
    void init();
    void exit();
    void createContext(ucontext_t *pNextCtx);
    void setLastContext(ucontext_t *pLastCtx);
    ucontext_t* getCtxPtr();
    OSN_CO_ARG run(const OSN_CO_ARG &arg);
private:
    MEMBER_VALUE(oUINT32, Id);
    MEMBER_VALUE(OSN_COROUTINE_FUNC, Func);
    MEMBER_VALUE(oINT32, State);
    MEMBER_VALUE(ucontext_t, Ctx);
    oINT8 m_szStack[s_nStackSize];
};
#endif /* osn_coroutine_hpp */