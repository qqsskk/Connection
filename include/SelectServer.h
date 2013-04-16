#ifndef _SELECT_SERVER_H
#define _SELECT_SERVER_H
#include <ConcurrentServer.h>

class SelectServer: public ConcurrentServer{
public:
	SelectServer();
	virtual ~SelectServer();
	
	virtual void StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&);
	virtual void EndService() throw(ConnectionException&);
	virtual void AddNewClient(ConnectionIPV4 &clientConn);
	virtual bool WaitForClient(ClientVector &) throw(ConnectionException&);
private:
	int mMaxSockfd;
	fd_set mInitSet, mCurSet;
};

#endif
