#include "Connection.h"
#include <stdio.h>
#include <strings.h> //bzero
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <errno.h>

ConnectionIPV4::ConnectionIPV4()
{
	bzero(&mSockAddr, sizeof(mSockAddr));
	mSockfd = -1;
	mClosed = true;
}
//----------------------------------------------------------------
ConnectionIPV4::~ConnectionIPV4()
{
	try{
		Close();
	}catch(ConnectionException &e){
		//deconstructor should not throw exception
	}
}
//----------------------------------------------------------------
void ConnectionIPV4::Close(void) throw(ConnectionException&)
{
	if(!mClosed){
		if(close(mSockfd)!=0) throw ConnectionException("Error: close socket error\n");
		mClosed = true;
	}
}
//----------------------------------------------------------------
const char *ConnectionIPV4::GetIP(void)
{
	return inet_ntoa(mSockAddr.sin_addr);
}
//----------------------------------------------------------------
int ConnectionIPV4::GetPort(void)
{
	return ntohs(mSockAddr.sin_port);
}
//----------------------------------------------------------------
int ConnectionIPV4::GetSockfd(void)
{
	return mSockfd;
}
//----------------------------------------------------------------
void ConnectionIPV4::InitialSocket(void) throw(ConnectionException&)
{
	mSockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(mSockfd < 0) throw ConnectionException("Error: create socket error\n");
	mClosed = false;
	return;
}
//----------------------------------------------------------------
void ConnectionIPV4::BindIpPort(const char *ipStr, int port) throw(ConnectionException&)
{
	int sockLen = sizeof(mSockAddr);
	mSockAddr.sin_port = htons(port);
	mSockAddr.sin_family = AF_INET;
	if(ipStr==NULL) mSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else{
		if(inet_aton(ipStr, &mSockAddr.sin_addr)==0) 
			throw ConnectionException("Error: bind error, not a valid ip address\n");
	}
	if(bind(mSockfd, (struct sockaddr *)&mSockAddr, sockLen) < 0){
		if(EADDRINUSE == errno){
			char errorStr[100];
			sprintf(errorStr, "Error: bind error, socket(%s:%d) is in use\n", (ipStr!=NULL)?ipStr:"ANY_ADDRESS", port);
			throw ConnectionException(errorStr);
		}
		else throw ConnectionException("Error: bind error, unknown reason\n");
	}
	return;
}
//----------------------------------------------------------------
void ConnectionIPV4::ListenForClient(int backlog) throw(ConnectionException&)
{
	if(listen(mSockfd, backlog)<0) throw ConnectionException("Error: listen error\n");
	return;
}
//----------------------------------------------------------------
bool ConnectionIPV4::AcceptClient(ConnectionIPV4& clientConn) throw(ConnectionException&)
{
	socklen_t sockLen = sizeof(clientConn.mSockAddr);

	clientConn.mSockfd = accept(mSockfd, (struct sockaddr*)&clientConn.mSockAddr, &sockLen);
	if(clientConn.mSockfd < 0){
		if(EINTR == errno) return false;/*receive signal when blocking*/
		else if(EPROTO == errno || ECONNABORTED == errno) return false;/*client  aborted after three handle shake process*/
		else throw ConnectionException("Error: accept error\n");
	}
	return true;
}
//----------------------------------------------------------------
bool ConnectionIPV4::ConnectToServer(const char *serverIpStr, int serverPort) throw(ConnectionException&)
{
	socklen_t cliSockLen = sizeof(mSockAddr);
	mSockAddr.sin_family = AF_INET;
	mSockAddr.sin_port = htons(serverPort);
	if(inet_aton(serverIpStr, &mSockAddr.sin_addr)==0)
		throw ConnectionException("Error: connect error, server's ip is not valid\n'");
	if(!mClosed){
		Close();
		InitialSocket();
	}
	if(connect(mSockfd, (struct sockaddr *)&mSockAddr, cliSockLen)<0){
		mClosed = false;
		if(ECONNREFUSED == errno || EINTR == errno)
			return false;/*return false to let user reconnect*/
		else if(ETIMEDOUT == errno)
			throw ConnectionException("Error: connection time out, server busy\n");
		else if(EHOSTUNREACH == errno)
			throw ConnectionException("Error: server unreachable\n");
		else 
			throw ConnectionException("Error: connection failure, unknown reason\n");
		return false;
	}
	return true;
}
