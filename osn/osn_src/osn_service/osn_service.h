//
//  osn_service.hpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_service_h
#define osn_service_h

#include <queue>
#include <mutex>
#include <map>
#include "osn_common.h"
#include "osn_service_head.h"
#include "osn_spin_lock.h"
#include "osn_arr_manager.h"

class OsnService {
    enum eYieldType
    {
        eYT_None = 0,
        eYT_Call,
        eYT_Return,
        eYT_Exit,
        eYT_Response,
        eYT_Quit,
        eYT_Sleep,
        eYT_Wakeup,
        eYT_CleanSleep,
    };
    typedef std::function<void (const OsnPreparedStatement &)> OSN_SERVICE_CO_FUNC;
    static std::queue<oUINT32> s_queCoroutine;
    static OsnSpinLock s_CoQueSpinLock;
    static oUINT64 s_u64CoroutineCount;
    static OsnSpinLock s_CoCountLock;
public:
    virtual ~OsnService();
protected:
	friend class OsnServiceManager;
	friend class OsnArrManager<OsnService, eThread_Saved>;
    friend class Osn;
	typedef void (OsnService::*CO_MEMBER_FUNC)(const OsnPreparedStatement &);

    OsnService();
    virtual void exit();
private:
    virtual void start(const OsnPreparedStatement &stmt) = 0;
    
    void onStart(const OsnPreparedStatement &stmt);
    void dispatch(const OsnPreparedStatement &stmt);
    void init(const OsnPreparedStatement &stmt);
    oBOOL getIsInGlobal();
    void setIsInGlobal(oBOOL value);
    oBOOL dispatchMessage(oINT32 &nType);
    oUINT32 pushMsg(stServiceMessage *pMsg);
    oUINT32 getMsgSize();
    oUINT32 createCO(OSN_SERVICE_CO_FUNC func);
    oINT32 suspend(oUINT32 co, const OSN_CO_ARG &arg);
    void pushToCoroutinePool(oUINT32 co);
    oUINT32 popFromCoroutinePool();
    stServiceMessage* popMessage();
    oINT32 dispatchWakeup();
	void registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC func);
    void unregistDispatchFunc(oINT32 nPType);
private:
    std::queue<stServiceMessage*> m_queMsg;
    OsnSpinLock m_QueMsgSpinLock;

	oBOOL m_IsInGlobal;
    
    MEMBER_VALUE(oUINT32, Id)
    oUINT32 m_unSessionCount;
	std::map<oUINT32, oUINT32> m_mapCoroutineSession;
	std::map<oUINT32, oUINT32> m_mapCoroutineSource;

	typedef std::map<oUINT32, oUINT32> MAP_SESSION_CO;
	typedef MAP_SESSION_CO::iterator MAP_SESSION_CO_ITR;
	MAP_SESSION_CO m_mapSessionCoroutine;
	OSN_COROUTINE_FUNC m_CoroutineFunction;
    std::map<oUINT32, oUINT32> m_mapSleepSession;
    std::queue<oUINT32> m_queWakeup;
	MAP_DISPATCH_FUNC m_mapDispatchFunc;
};

#endif /* osn_service_hpp */
