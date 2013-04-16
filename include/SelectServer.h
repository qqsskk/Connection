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
	/*返回值:是否有新客户连接过来
	   true有新客户，并将其信息存放在数组的第一个位置
	   false表示没有
	*/
	virtual bool WaitForClient(ClientVector &) throw(ConnectionException&);
private:
	int mMaxSockfd;
	fd_set mInitSet, mCurSet;
};

#endif
