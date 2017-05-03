//
//  TestService3.cpp
//  osn
//
//  Created by zenghui on 17/5/3.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "TestService3.h"
#include "osn_service_manager.h"

TestService3::TestService3()
{
    
}

TestService3::~TestService3()
{
    
}

void TestService3::start(const OsnPreparedStatement &stmt)
{
	RegistDispatchFunc(ePType_Lua, &TestService3::dispatchLua, this);
	m_Socket.init();
	oINT32 fd = m_Socket.open("127.0.0.1", 18888);
	m_Socket.write(fd, "aaaaa", 5);
}

void TestService3::exit()
{
    
}

void TestService3::dispatchLua(const OsnPreparedStatement &stmt)
{
    
}


