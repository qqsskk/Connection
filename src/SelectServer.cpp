#include "SelectServer.h"
#include <sys/select.h>
#include <stdio.h>

//----------------------------------------------------------------
SelectServer::SelectServer()
{
}
//----------------------------------------------------------------
SelectServer::~SelectServer()
{
	try{
		EndService();
	}catch(ConnectionException &e){
		fprintf(stderr, "Exception occurs when endding service, description %s\n", e.what());
	}
}
//----------------------------------------------------------------
void SelectServer::StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&)
{
	ConcurrentServer::StartService(ipStr,port,queueMax);
	mMaxSockfd = mListenConn.GetSockfd();
	FD_ZERO(&mInitSet);
	FD_SET(mMaxSockfd, &mInitSet);
}
//----------------------------------------------------------------
void SelectServer::EndService() throw(ConnectionException&)
{
	if(mInService){
		for(ClientVectorIter iter = mClientConnVec.begin(); iter != mClientConnVec.end(); ++iter){
			iter->Close();
		}
	}
	ConcurrentServer::EndService();
}
//----------------------------------------------------------------
void SelectServer::AddNewClient(ConnectionIPV4& clientConn)
{	
	ConcurrentServer::AddNewClient(clientConn);
	//sort
	for(ClientVectorIter iter = mClientConnVec.begin(); iter!=mClientConnVec.end(); ++iter){
		if(iter->GetSockfd() > clientConn.GetSockfd()){
			if(iter->GetSockfd() != clientConn.GetSockfd()){
				mClientConnVec[mClientConnVec.size() - 1] = *iter;
				*iter = clientConn;
			}
			return;
		}
	}
}
//----------------------------------------------------------------
bool SelectServer::WaitForClient(ClientVector& waitClientVec) throw(ConnectionException&)
{
	int nReady;
	bool ret = false;

	waitClientVec.clear();
	mCurSet = mInitSet;
	for(ClientVectorIter iter = mClientConnVec.begin(); iter!=mClientConnVec.end(); ++iter){		
		FD_SET(iter->GetSockfd(), &mCurSet);
	}
	/*only care read event*/
	nReady = select(mMaxSockfd+1, &mCurSet, NULL, NULL, NULL);
	if(nReady == 0) return false;/*no new client*/
	else if(nReady < 0){
		throw ConnectionException("Error : select error\n");
	}

	//check for listen socket
	if(FD_ISSET(mListenConn.GetSockfd(), &mCurSet)){/*new client in*/
		ConnectionIPV4 cliConn ;
		
		if(mListenConn.AcceptClient(cliConn) == true){/*ignore error in accept*/
			AddNewClient(cliConn);
			waitClientVec.push_back(cliConn);
			mMaxSockfd = mClientConnVec[mClientConnVec.size() - 1].GetSockfd();
			ret = true;
		}
		--nReady;
	}
	//check for other socket
	for(ClientVectorIter iter = mClientConnVec.begin(); iter!=mClientConnVec.end() && nReady > 0; ++iter){
		if(FD_ISSET(iter->GetSockfd(), &mCurSet)){
			waitClientVec.push_back(*iter);
			--nReady;
		}
	}
	return ret;
}
