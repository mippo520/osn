#include "TestService.h"



TestService::TestService()
{
}


TestService::~TestService()
{
}

void TestService::dispatchLua(const OsnPreparedStatement &stmt)
{
	OsnPreparedStatement msg;
	msg.setUInt32(0, 100);
	msg.setString(1, "abc");
	msg.setInt8(2, 20);
	msg = this->call(2, ePType_Lua, msg);
	printf("TestService::dispatch ==> float32 = %f, int32 = %d, string = %s\n", msg.getFloat32(0), msg.getInt32(1), msg.getString(2).c_str());

	msg.clear();
	msg.setUInt32(0, 101);
	msg = this->call(2, ePType_Lua, msg);
	msg.printContext();
}

void TestService::init()
{
	registDispatchFunc(ePType_Lua, static_cast<CO_MEMBER_FUNC>(&TestService::dispatchLua));
}

void TestService::exit()
{
}
