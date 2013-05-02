#include "Connection.h"
#include <stdio.h>
#include <strings.h> //bzero
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <errno.h>

ConnectionIPV4::ConnectionIPV4() :
	mClosed(true),
	mSockfd(-1),
	mLastErrorStr("")
{
	bzero(&mSockAddr, sizeof(mSockAddr));
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
const char *ConnectionIPV4::GetLastError(void)
{
	return mLastErrorStr.c_str();
}
//----------------------------------------------------------------
void ConnectionIPV4::SetAddrReuseable(void) throw(ConnectionException&)
{
	int on = 1;

	if(setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
		throw ConnectionException("Error setsockopt : set reuseaddr failure\n");
	}
}
//----------------------------------------------------------------
/*
	通过返回值判断是否执行成功，true成功，false失败
	错误信息存放在字符串中
*/
bool ConnectionIPV4::SendData(void *data, int n)
{
	int sendNum = 0, sendBytes;
	char *buffer = static_cast<char *>(data);

	while(n-sendNum > 0){
	    sendBytes = write(mSockfd, buffer+sendNum, n-sendNum);
	    if(sendBytes <= 0){
	        if(EINTR == errno || EAGAIN == errno) sendBytes = 0;/*continue send*/
	        else if(EPIPE == errno){
				mLastErrorStr = "Error send data : send data to client which has send RST to server";
				return false;
	        }else if(ECONNRESET == errno){
	        	mLastErrorStr = "Error send data : connnection has been reseted";
				return false;
	        }else if(ECONNREFUSED == errno){
	        	mLastErrorStr = "Errno send date: counterpart restarted";
				return false;
	        }else{
	        	mLastErrorStr = "Error send data: unknown reason";
				return false;
	        }
	    }
	    sendNum += sendBytes;
	}
	return true;
}
//----------------------------------------------------------------
bool ConnectionIPV4::RecvData(void *data, int n)
{
	int recvNum = 0, recvBytes;
	char *buffer = static_cast<char*>(data);

	while(n-recvNum > 0){
	    recvBytes = read(mSockfd, buffer+recvNum, n-recvNum);
	    if(0 == recvBytes){
			mLastErrorStr = "Error recv data: received FIN from counterpart";
			return false;
	    }else if(recvBytes < 0){
	        if(EINTR == errno || EAGAIN == errno) recvBytes = 0;/*continue receive data*/
		else if(ETIMEDOUT == errno){
			mLastErrorStr = "Error recv data: timeout, no ACK from counterpart";
			return false;
		}else if(ECONNRESET == errno){
			mLastErrorStr = "Error recv data: connection reset";
			return false;
		}else if(ECONNREFUSED == errno){
			mLastErrorStr = "Errno recv date: counterpart restarted";
			return false;
		}else if(EHOSTUNREACH == errno || ENETUNREACH == errno){
			mLastErrorStr = "Error recv data: net unreachable";
			return false;
		}else {
			mLastErrorStr = "Error recv data: unknown reason";
			return false;
		}
	    }
	    recvNum += recvBytes;
	}
	return true;
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
bool ConnectionIPV4::AcceptClient(ConnectionIPV4& clientConn)
{
	socklen_t sockLen = sizeof(clientConn.mSockAddr);

	clientConn.mSockfd = accept(mSockfd, (struct sockaddr*)&clientConn.mSockAddr, &sockLen);
	if(clientConn.mSockfd < 0){
		if(EINTR == errno){
			mLastErrorStr = "Error accept client: interupted by signal";
			return false;/*receive signal when blocking*/
		}else if(EPROTO == errno || ECONNABORTED == errno){
			mLastErrorStr = "Error accept client: client aborted";
			return false;/*client  aborted after three handle shake process*/
		}else{
			mLastErrorStr = "Error accept: unknown reason";
			return false;
		}
	}
	return true;
}
//----------------------------------------------------------------
bool ConnectionIPV4::ConnectToServer(const char *serverIpStr, int serverPort)
{
	socklen_t cliSockLen = sizeof(mSockAddr);
	mSockAddr.sin_family = AF_INET;
	mSockAddr.sin_port = htons(serverPort);
	if(inet_aton(serverIpStr, &mSockAddr.sin_addr)==0){
		mLastErrorStr = "Error connect : server ip is not valid";
		return false;
	}
	/*每次连接必须保证上次连接关闭*/
	if(!mClosed){
		Close();
		InitialSocket();
	}
	if(connect(mSockfd, (struct sockaddr *)&mSockAddr, cliSockLen)<0){
		mClosed = false;
		if(EINTR== errno){
			mLastErrorStr = "Error connect: interupted by signal, please try again";
			return false;
		}else if(ECONNREFUSED == errno){
			mLastErrorStr = "Error connect: server is not ready, please wait and try again";
			return false;/*return false to let user reconnect*/

		}else if(ETIMEDOUT == errno){
			mLastErrorStr = "Error connect: connection time out, server busy, please wait and try again";
			return false;
		}else if(EHOSTUNREACH == errno){
			mLastErrorStr = "Error connect: server unreachable";
			return false;
		}else {
			mLastErrorStr = "Error connect: unknown reason";
			return false;
		}
	}
	return true;
}
//----------------------------------------------------------------
bool ConnectionIPV4::operator<(const ConnectionIPV4& rhs) const
{
	return mSockfd<rhs.mSockfd;
}
