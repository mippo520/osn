#pragma once
#include "osn_service.h"
#include <set>

class TestService : public OsnService
{
public:
	TestService();
	~TestService();
public:
    virtual void start(const OsnPreparedStatement &stmt) override;
	void dispatchLua(const OsnPreparedStatement &stmt);
	void dispatchSocket(const OsnPreparedStatement &stmt);
	void dispatchText(const OsnPreparedStatement &stmt);
private:
	void report();
private:
	oINT32 m_nListenId;
	std::set<oINT32> m_setConnectedId;
};

