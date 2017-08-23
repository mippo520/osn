//
//  osn_message.hpp
//  osn
//
//  Created by zenghui on 17/4/7.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_service_head
#define osn_service_head
#include "osn_common.h"
#include "osn_prepared_statement.h"

enum eProtoType
{
	ePType_None = 0,
	ePType_Response,
	ePType_User,
	ePType_Client,
    ePType_Start,
    ePType_Socket,
};

struct stServiceMessage
{
	ID_SERVICE unSource;
	ID_SESSION unSession;
	oINT32	nType;
    SHARED_PTR_STMT stmt;

	stServiceMessage(const OsnPreparedStatement &stmt)
		: unSource(0)
		, unSession(0)
		, nType(ePType_None)
        , stmt(new OsnPreparedStatement(stmt))
	{}
    
    ~stServiceMessage() {}
};

#define RegistDispatchFunc(type, func, ptr) g_Osn->registDispatchFunc(type, std::bind(func, ptr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
#define UnregistDispatchFunc(type) g_Osn->unregistDispatchFunc(type)

#endif /* osn_service_head */
