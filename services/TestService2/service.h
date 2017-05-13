#pragma once
#include "osn_service.h"

class Service : public OsnService
{
public:
	Service();
	~Service();
public:
	virtual void start(const OsnPreparedStatement &stmt) override;
	virtual void exit() override;
	void dispatchLua(const OsnPreparedStatement &stmt);
};

