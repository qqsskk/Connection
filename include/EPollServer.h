#ifndef _EPOLL_SERVER_H
#define _EPOLL_SERVER_H
#include <ConcurrentServer.h>
#include <sys/epoll.h>
#include <map>

class EPollServer: public ConcurrentServer{
public:
	typedef std::map<int, ConnectionIPV4> ClientMap;
	typedef ClientMap::iterator ClientMapIter;
	
	EPollServer(int maxfds, int maxEvents);
	virtual ~EPollServer();
	
	virtual void StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&);
	virtual void EndService() throw(ConnectionException&);
	virtual void AddNewClient(ConnectionIPV4 &clientConn);
	virtual void RemoveClient(ConnectionIPV4 &clientConn);
	virtual bool WaitForClient(ClientVector &) throw(ConnectionException&);
private:
	int mMaxfds, mMaxEvents;
	ClientMap mClientMap;
	int mEpollfd;
	struct epoll_event *mpWaitingEvents;
};

#endif

