#pragma once
#include "osn_service.h"
#include "osn_common.h"
#include "I_osn_service_factory.h"

class TestService2 : public OsnService
{
public:
	TestService2();
	~TestService2();
public:
	virtual void start(const OsnPreparedStatement &stmt) override;
	virtual void exit() override;
	void dispatchLua(const OsnPreparedStatement &stmt);
};

AddService_Declare(TestService2)

