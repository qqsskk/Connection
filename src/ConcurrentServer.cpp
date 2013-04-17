#include "ConcurrentServer.h"
#include <stdio.h>

//----------------------------------------------------------------
ConcurrentServer::ConcurrentServer():
	mInService(false)
{
}
//----------------------------------------------------------------
ConcurrentServer::~ConcurrentServer()
{
}
//----------------------------------------------------------------
void ConcurrentServer::StartService(const char *ipStr, int port, int queueMax) throw(ConnectionException&)
{
	mListenConn.InitialSocket();
	mListenConn.SetAddrReuseable();//必须在socket和bind之间进行设置
	mListenConn.BindIpPort(ipStr,port);
	mListenConn.ListenForClient(queueMax);
	
	mInService = true;
}
//----------------------------------------------------------------
void ConcurrentServer::EndService() throw(ConnectionException&)
{
	if(mInService){
		mListenConn.Close();
		mInService = false;
	}
}
//----------------------------------------------------------------
void ConcurrentServer::AddNewClient(ConnectionIPV4 &clientConn)
{
	mClientConnVec.push_back(clientConn);
}
//----------------------------------------------------------------
void ConcurrentServer::RemoveClient(ConnectionIPV4& clientConn)
{
	for(ClientVectorIter iter = mClientConnVec.begin(); iter!=mClientConnVec.end(); ++iter){
		if(iter->GetSockfd() == clientConn.GetSockfd()){
			mClientConnVec.erase(iter);
			return;
		}
	}
}
