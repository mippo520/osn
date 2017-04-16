#pragma once
#include "osn_service.h"

class TestService : public OsnService
{
public:
	TestService();
	~TestService();
public:
	virtual void init() override;
	virtual void exit() override;
	void dispatchLua(const OsnPreparedStatement &stmt);
};

