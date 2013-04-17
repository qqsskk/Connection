#include "EPollServer.h"

//----------------------------------------------------------------
EPollServer::EPollServer(int maxfds, int maxEvents):
	mMaxfds(maxfds),
	mMaxEvents(maxEvents),
	mpWaitingEvents(NULL)
{
	struct epoll_event *pEvent = new struct epoll_event[mMaxEvents];
	if(pEvent) mpWaitingEvents = pEvent;
	//else throw /*异常类需要加入另一个成员，表示内存异常*/
}
//----------------------------------------------------------------
EPollServer::~EPollServer()
{
	EndService();
}
//----------------------------------------------------------------
void EPollServer::StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&)
{
	int ret;
	
	ConcurrentServer::StartService(ipStr,port,queueMax);
	//epoll操作
	ret = mEpollfd = epoll_create(mMaxfds);
	if(ret < 0) throw ConnectionException("Error Create epoll fd");
	struct epoll_event ev;
	ev.data.fd = mListenConn.GetSockfd();
	ev.events = EPOLLIN;
	ret = epoll_ctl(mEpollfd, EPOLL_CTL_ADD, mListenConn.GetSockfd(), &ev);
	if(ret < 0) throw ConnectionException("Error Add fd to epoll");
}
//----------------------------------------------------------------
void EPollServer::EndService() throw(ConnectionException&)
{
	if(mInService){
		for(ClientMapIter iter = mClientMap.begin(); iter != mClientMap.end(); ++iter){
			iter->second.Close();
		}
	}
	close(mEpollfd);
	ConcurrentServer::EndService();
}
//----------------------------------------------------------------
void EPollServer::AddNewClient(ConnectionIPV4 &clientConn)
{
	mClientMap.insert(ClientMap::value_type(clientConn.GetSockfd(), clientConn));
}
//----------------------------------------------------------------
void EPollServer::RemoveClient(ConnectionIPV4 &clientConn)
{
	ClientMapIter iter = mClientMap.find(clientConn.GetSockfd());
	if(iter!= mClientMap.end()) mClientMap.erase(iter);
	//epoll操作
	struct epoll_event ev;
	ev.data.fd = clientConn.GetSockfd();
	if(epoll_ctl(mEpollfd, EPOLL_CTL_DEL, clientConn.GetSockfd(), &ev) <0)
		throw ConnectionException("Error delete fd from epoll");
}
//----------------------------------------------------------------
bool EPollServer::WaitForClient(ClientVector &waitClientVec) throw(ConnectionException&)
{
	int nReady;
	bool ret = false;
	struct epoll_event ev;
	
	waitClientVec.clear();
	nReady = epoll_wait(mEpollfd, mpWaitingEvents, mMaxEvents, -1);
	if(nReady == -1) throw ConnectionException("Epoll wait error");

	for(int i =0; i < nReady; ++i){
		if(mListenConn.GetSockfd() == mpWaitingEvents[i].data.fd){
			ConnectionIPV4 cliConn;
			if(mListenConn.AcceptClient(cliConn) == true) {
				ev.data.fd = cliConn.GetSockfd();
				ev.events = EPOLLIN;
				epoll_ctl(mEpollfd, EPOLL_CTL_ADD, cliConn.GetSockfd(), &ev);
				AddNewClient(cliConn);
				ret = true;
				waitClientVec.push_back(cliConn);
				if(waitClientVec.size() > 1)
					std::swap(waitClientVec[0], waitClientVec[waitClientVec.size() - 1]);
			}
		}else if (mpWaitingEvents[i].events &(EPOLLIN | EPOLLERR)){
			ClientMapIter iter = mClientMap.find(mpWaitingEvents[i].data.fd);
			if(iter != mClientMap.end()){
				waitClientVec.push_back(iter->second);
			}
		}
	}
	return ret;
}
