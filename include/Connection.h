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

class ConnectionIPV4{
public:
	ConnectionIPV4();
	~ConnectionIPV4();
	//server&client use
	void InitialSocket(void) throw(ConnectionException&);
	void Close(void) throw(ConnectionException&);
	const char *GetIP(void);
	int GetPort(void);
	int GetSockfd(void);
    void SendData(void *data, int n) throw(ConnectionException&);
    void RecvData(void *buffer, int n) throw(ConnectionException&);
	//server use
	void BindIpPort(const char *ipStr, int port) throw(ConnectionException&);
	void ListenForClient(int backlog) throw(ConnectionException&);
	bool AcceptClient(ConnectionIPV4& clientConn) throw(ConnectionException&);
	//client use
	bool ConnectToServer(const char *serverIpStr, int serverPort) throw(ConnectionException&);

private:
	struct sockaddr_in mSockAddr;
	bool mClosed;
	int mSockfd;
};
#endif
