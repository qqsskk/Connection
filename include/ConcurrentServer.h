#ifndef _CONCURRENT_SERVER_H
#define _CONCURRENT_SERVER_H
#include <Connection.h>
#include <vector>

class ConcurrentServer{
public:
	typedef std::vector<ConnectionIPV4> ClientVector;
	typedef ClientVector::iterator ClientVectorIter;
	ConcurrentServer();
	virtual ~ConcurrentServer();
	
	virtual void StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&);
	virtual void EndService() throw(ConnectionException&);
	virtual void AddNewClient(ConnectionIPV4 &clientConn);
	virtual void RemoveClient(ConnectionIPV4 &clientConn);
	/*返回值:是否有新客户连接过来s
	   true有新客户，并将其信息存放在数组的第一个位置
	   false表示没有
	*/
	virtual bool WaitForClient(ClientVector &) throw(ConnectionException&) = 0;	
	
protected:
	ConnectionIPV4 mListenConn;
	bool mInService;
	ClientVector mClientConnVec;
};

#endif
