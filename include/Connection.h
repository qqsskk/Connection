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
/*�쳣�����ԭ����:
1.֪��ԭ��Ĵ�������׳��쳣��ͨ������ֵ���û����д���
2.��֪��ԭ��Ĵ���Ż᷵���쳣���ᵼ��Ӧ�ó����˳�
�������У��ܶ�����ǿ���ͨ��errno����֪�ģ���˲���ǿ��Ӧ�ó��򲶻��쳣
��Щ�����紴���׽����Լ��󶨼����ȣ�����û�о����errno��Ӧ�������׳��쳣
�������ͺͽ������ݣ�accept�û��ȣ���Ӧ��ͨ������ֵ��֪Ӧ�ó����Ƿ���ȷִ��
Ϊ�˲�Ӱ�����Ļ�ȡ�������ݳ�Ա�м���һ���쳣�ַ��������û���ȡ
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
