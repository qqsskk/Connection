#ifndef _ConnectionIPV4_H
#define _ConnectionIPV4_H
#include <exception>
#include <string>
#include <netinet/in.h>

class ConnectionException : public std::exception{
public:
	ConnectionException(const char *exceptStr);
	virtual ~ConnectionException() throw();
	virtual const char* what() const throw();
private:
	std::string mExceptionStr;
};
/*异常处理的原则是:
1.知道原因的错误绝不抛出异常，通过返回值供用户进行处理
2.不知道原因的错误才会返回异常，会导致应用程序退出
网络编程中，很多错误都是可以通过errno来获知的，因此不必强制应用程序捕获异常
有些错误，如创建套接字以及绑定监听等，由于没有具体的errno对应表，可以抛出异常
但是像发送和接收数据，accept用户等，就应该通过返回值告知应用程序是否正确执行
为了不影响错误的获取，在数据成员中加入一个异常字符串，供用户读取
*/
class ConnectionIPV4{
public:
	ConnectionIPV4();
	~ConnectionIPV4();

	const char*	GetIP(void);
	int 			GetPort(void);
	int 			GetSockfd(void);
	const char *	GetLastError(void);
	//server&client use
	void 			InitialSocket(void) throw(ConnectionException&);
	void 			Close(void) throw(ConnectionException&);
	//server use
	void 			BindIpPort(const char *ipStr, int port) throw(ConnectionException&);
	void 			ListenForClient(int backlog) throw(ConnectionException&);
	bool			AcceptClient(ConnectionIPV4& clientConn);
	//client use
	bool 			ConnectToServer(const char *serverIpStr, int serverPort);

	bool 			SendData(void *data, int n);
	bool 			RecvData(void *buffer, int n);
	
private:
	struct sockaddr_in mSockAddr;
	bool mClosed;
	int mSockfd;
	std::string mLastErrorStr;
};
#endif
