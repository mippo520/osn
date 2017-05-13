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
	ePType_Lua,
	ePType_Client,
    ePType_Start,
    ePType_Socket,
	ePType_Text,
};

struct stServiceMessage
{
	oUINT32 unSource;
	oUINT32 unSession;
	oINT32	nType;
	OsnPreparedStatement stmt;

	stServiceMessage(const OsnPreparedStatement *pStmt)
		: unSource(0)
		, unSession(0)
		, nType(ePType_None)
        , stmt(pStmt)
	{}
    
    ~stServiceMessage() {}
};

#define RegistDispatchFunc(type, func, ptr) g_Osn->registDispatchFunc(type, std::bind(func, ptr, std::placeholders::_1));
#define UnregistDispatchFunc(type) g_Osn->unregistDispatchFunc(type)

#endif /* osn_service_head */
