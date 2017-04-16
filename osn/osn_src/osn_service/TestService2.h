#pragma once
#include "osn_service.h"

class TestService2 : public OsnService
{
public:
	TestService2();
	~TestService2();
public:
	virtual void init() override;
	virtual void exit() override;
	void dispatchLua(const OsnPreparedStatement &stmt);
};

