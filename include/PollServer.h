#ifndef _POLL_SERVER_H
#define _POLL_SERVER_H
#include <ConcurrentServer.h>
#include <poll.h>
#include <sys/stropts.h>//INFTIM

#ifndef INFTIM
#define INFTIM (-1)
#endif

class PollServer: public ConcurrentServer{
public:
	typedef std::vector<struct pollfd> PollClientVector;
	typedef PollClientVector::iterator PollClientVectorIter;
	
	PollServer();
	virtual ~PollServer();
	
	virtual void StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&);
	virtual void EndService() throw(ConnectionException&);
	virtual void AddNewClient(ConnectionIPV4 &clientConn);
	virtual void RemoveClient(ConnectionIPV4 &clientConn);
	/*返回值:是否有新客户连接过来
	   true有新客户，并将其信息存放在数组的第一个位置
	   false表示没有
	*/
	virtual bool WaitForClient(ClientVector &) throw(ConnectionException&);
private:
	PollClientVector mPollVec;/*include listen and r&w*/
};

#endif
