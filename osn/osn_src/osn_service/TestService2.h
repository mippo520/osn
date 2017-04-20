#pragma once
#include "osn_service.h"

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

