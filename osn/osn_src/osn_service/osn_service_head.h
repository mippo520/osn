//
//  osn_message.hpp
//  osn
//
//  Created by liqing on 17/4/7.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_service_head
#define osn_service_head
#include "osn.h"
#include "osn_prepared_statement.h"

enum eProtoType
{
	ePType_None = 0,
	ePType_Response,
	ePType_Lua,
	ePType_Client,
    ePType_Start,
};

struct stServiceMessage
{
	oUINT32 unSource;
	oUINT32 unSession;
	oINT32	nType;
	OsnPreparedStatement stmt;

	stServiceMessage(const OsnPreparedStatement &stmt)
		: unSource(0)
		, unSession(0)
		, nType(ePType_None)
        , stmt(stmt)
	{}
};

#endif /* osn_service_head */
