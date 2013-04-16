/*************************************************************************
	> File Name: server.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 01时00分06秒
 ************************************************************************/
#include <Connection.h>
#include <Protocal.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#define LISTEN_PORT 8000
#define LISTEN_QUEUE 5
using namespace std;

int main()
{
	ConnectionIPV4 connListen, connCli;
	char msgStr[100] = "hello, client";
	struct ProtocalHead head;
	try{
		connListen.InitialSocket();
		connListen.BindIpPort(NULL, LISTEN_PORT);
		connListen.ListenForClient(LISTEN_QUEUE);
		cout << "Listen ...\n";
		while(connListen.AcceptClient(connCli) ==false){
			cout << connListen.GetLastError() << endl;
		}
		cout <<"Client " <<  connCli.GetIP() << " on port " << connCli.GetPort() 
			<< " is connected to fd " << connCli.GetSockfd() << endl;

		while(1){
			if(connCli.RecvData(&head,sizeof(head)) == false){
				cout << connCli.GetLastError() << endl;
				return -1;
			}
			if(connCli.RecvData(msgStr, head.dataLen)==false){
				cout << connCli.GetLastError() << endl;
				return -1;
			}
			msgStr[head.dataLen]=0;
			if(connCli.SendData(msgStr, head.dataLen) == false){
				cout << connCli.GetLastError() << endl;
				return -1;
			}
			cout << "Received "<< msgStr << "\nEcho back ...\n";
		}
	}catch(const ConnectionException &e){
		cout << e.what();
	}
	return 0;
}
