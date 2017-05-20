//
//  gate.cpp
//  gate
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "gate.h"

AddService_Instance(Gate)

Gate::Gate()
{
    
}

Gate::~Gate()
{
    
}

void Gate::start(const OsnPreparedStatement &stmt)
{
    printf("Gate::start ====>!\n");
    RegistDispatchFunc(ePType_User, &Gate::dispatchUser, this);
}

void Gate::dispatchUser(ID_SERVICE source, ID_SESSION session, const OsnPreparedStatement &stmt)
{
    printf("Gate::dispatchUser ===========> %llx, %llx, %s\n", stmt.getUInt64(0), stmt.getUInt64(1), stmt.getCharPtr(2));
    OsnPreparedStatement ret;
    ret.setString(0, "gate ret!");
    g_Osn->ret(ret);
    g_Osn->exit();
}

