//
//  osn_service.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_service_h
#define osn_service_h

#include <queue>
#include <mutex>
#include <map>
#include "osn.h"
#include "osn_arr_manager.h"
#include "osn_coroutine_head.h"
#include "osn_service_head.h"

class OsnService {
    enum eYieldType
    {
        eYT_None = 0,
        eYT_Call,
        eYT_Return,
        eYT_Exit,
        eYT_Response,
        eYT_Quit,
    };
	typedef std::function<void(const OsnPreparedStatement &)> DISPATCH_FUNC;
	typedef OsnPreparedStatement::STMT_FUNC OSN_SERVICE_CO_FUNC;
    static std::queue<oUINT32> s_queCoroutine;
    static OsnSpinLock s_CoQueSpinLock;
public:
    virtual ~OsnService();
protected:
	friend class OsnServiceManager;
	friend class OsnArrManager<OsnService, eThread_Saved>;
	typedef void (OsnService::*CO_MEMBER_FUNC)(const OsnPreparedStatement &);

    OsnService();
    void registDispatchFunc(oINT32 nPType, CO_MEMBER_FUNC funcPtr);
private:
    virtual void start(const OsnPreparedStatement &stmt) = 0;
    virtual OsnPreparedStatement dispatch(const OsnPreparedStatement &stmt);
    void init();
    virtual void exit();
    oBOOL getIsInGlobal();
    void setIsInGlobal(oBOOL value);
private:
    virtual oBOOL dispatchMessage(oINT32 &nType);
    oUINT32 pushMsg(stServiceMessage &msg);
    oUINT32 getMsgSize();
    oUINT32 createCO(OSN_SERVICE_CO_FUNC func);
    oINT32 suspend(oUINT32 co, const OSN_CO_ARG &arg);
    
    oBOOL popMessage(stServiceMessage &msg);
private:
    std::queue<stServiceMessage> m_queMsg;
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

	typedef std::map<oINT32, DISPATCH_FUNC> MAP_DISPATCH_FUNC;
	typedef MAP_DISPATCH_FUNC::iterator MAP_DISPATCH_FUNC_ITR;
	MAP_DISPATCH_FUNC m_mapDispatchFunc;
};

#endif /* osn_service_hpp */
