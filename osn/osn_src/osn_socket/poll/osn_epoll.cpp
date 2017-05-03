#include "osn_epoll.h"

#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "osn_socket_head.h"

OsnEpoll::OsnEpoll()
{

}

OsnEpoll::~OsnEpoll()
{

}

oINT32 OsnEpoll::create()
{
	return epoll_create(1024);
}

oBOOL OsnEpoll::invalid(oINT32 efd)
{
	return (efd == -1);
}

void OsnEpoll::release(oINT32 efd)
{
	close(efd);
}

oBOOL OsnEpoll::add(oINT32 efd, oINT32 sock, void *ud)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1) 
	{
		return false;
	}
	return true;
}

void OsnEpoll::del(oINT32 efd, oINT32 sock)
{
	epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
}

oINT32 OsnEpoll::wait(oINT32 efd, stSocketEvent *pEvent, oINT32 max)
{
	struct epoll_event ev[max];
	oINT32 n = epoll_wait(efd, ev, max, -1);
	for (oINT32 i = 0; i < n; ++i) 
	{
		pEvent[i].socket = static_cast<OsnSocketData*>(ev[i].data.ptr);
		oUINT32 flag = ev[i].events;
		pEvent[i].bWrite = (flag & EPOLLOUT) != 0;
		pEvent[i].bRead = (flag & EPOLLIN) != 0;
	}

	return n;
}

void OsnEpoll::write(oINT32 efd, oINT32 sock, void *ud, oBOOL enable)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = ud;
	epoll_ctl(efd, EPOLL_CTL_MOD, sock, &ev);
}

void OsnEpoll::nonBlocking(oINT32 fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (-1 == flag) {
		return;
	}

	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}
