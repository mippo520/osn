#pragma once
#include "osn_service.h"

class TestService : public OsnService
{
public:
	TestService();
	~TestService();
public:
    virtual void start(const OsnPreparedStatement &stmt) override;
	void dispatchLua(const OsnPreparedStatement &stmt);
	void dispatchSocket(const OsnPreparedStatement &stmt);
};

