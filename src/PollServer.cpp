#include "PollServer.h"

//----------------------------------------------------------------
PollServer::PollServer()
{
}
//----------------------------------------------------------------
PollServer::~PollServer()
{
	EndService();
}
//----------------------------------------------------------------
void PollServer::StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&)
{
	ConcurrentServer::StartService(ipStr,port,queueMax);

	struct pollfd listenPollfd;
	listenPollfd.fd = mListenConn.GetSockfd();
	listenPollfd.events = POLLRDNORM;
	mPollVec.push_back(listenPollfd);
}
//----------------------------------------------------------------
void PollServer::EndService() throw(ConnectionException&)
{
	if(mInService){
		for(PollClientVectorIter iter = mPollVec.begin() +1; iter != mPollVec.end(); ++iter){
			close(iter->fd);
		}
	}
	ConcurrentServer::EndService();
}
//----------------------------------------------------------------
void PollServer::AddNewClient(ConnectionIPV4 &clientConn)
{
	struct pollfd pollfd;
	pollfd.fd = clientConn.GetSockfd();
	pollfd.events = POLLRDNORM;
	mPollVec.push_back(pollfd);
	
	ConcurrentServer::AddNewClient(clientConn);
}
//----------------------------------------------------------------
void PollServer::RemoveClient(ConnectionIPV4 &clientConn)
{
	for(PollClientVectorIter iter = mPollVec.begin() + 1; iter !=mPollVec.end(); ++iter){
		if(iter->fd == clientConn.GetSockfd()){
			mPollVec.erase(iter);
			break;
		}
	}
	ConcurrentServer::RemoveClient(clientConn);
}
//----------------------------------------------------------------
bool PollServer::WaitForClient(ClientVector &waitClientVec) throw(ConnectionException&)
{
	int nReady,n;
	bool ret = false;
	ConnectionIPV4 cliConn ;

	waitClientVec.clear();
	nReady = poll(&mPollVec[0], mPollVec.size(), INFTIM);
	if(nReady == 0) return false;
	else if(nReady < 0) throw ConnectionException("poll error\n");

	//check for listen socket
	if(mPollVec[0].revents & POLLRDNORM){/*new client in*/
		
		if(mListenConn.AcceptClient(cliConn) == true){/*ignore error in accepting*/
			AddNewClient(cliConn);
			waitClientVec.push_back(cliConn);
			ret = true;
		}
		--nReady;
	}
	//check for other requests
	n = 0;
	for(PollClientVectorIter iter = mPollVec.begin() + 1; iter != mPollVec.end() && nReady > 0; ++iter,++n){
		if(iter->revents & (POLLRDNORM | POLLERR)){
			waitClientVec.push_back(mClientConnVec[n]);
			--nReady;
		}
	}
	return ret;
}
