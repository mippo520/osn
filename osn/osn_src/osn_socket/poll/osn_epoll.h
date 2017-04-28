#pragma once
#include "osn_poll.h"

class OsnEpoll : public OsnPoll
{
public:
	OsnEpoll();
	~OsnEpoll();
	virtual oINT32 create() override;
	virtual oBOOL invalid(oINT32 nfd) override;
	virtual void release(oINT32 nfd) override;
	virtual oBOOL add(oINT32 poll_fd, oINT32 sock, void *ud) override;
	virtual void del(oINT32 poll_fd, oINT32 sock) override;
	virtual oINT32 wait(oINT32 poll_fd, stSocketEvent *pEvent, oINT32 nMax) override;
	virtual void write(oINT32 poll_fd, oINT32 sock, void *ud, oBOOL enable) override;
	virtual void nonBlocking(oINT32 nSock) override;
};

