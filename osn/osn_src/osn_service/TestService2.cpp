#include "TestService2.h"



TestService2::TestService2()
{
}


TestService2::~TestService2()
{
}

void TestService2::dispatchLua(const OsnPreparedStatement &stmt)
{
	stmt.printContext();
	OsnPreparedStatement msg;
	if (100 == stmt.getUInt32(0))
	{
		msg.setFloat32(0, 0.5f);
		msg.setInt32(1, 101);
		msg.setString(2, "def");
		this->ret(msg);
	}
	else
	{
		msg.setFloat32(0, 1.5f);
		msg.setInt32(1, 201);
		msg.setString(2, "ghi");
		this->ret(msg);
	}
}

void TestService2::init()
{
	registDispatchFunc(ePType_Lua, static_cast<CO_MEMBER_FUNC>(&TestService2::dispatchLua));
	send(1, ePType_Lua);
}

void TestService2::exit()
{
}
