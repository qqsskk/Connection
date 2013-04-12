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
		while(connListen.AcceptClient(connCli)){
			cout <<"Client " <<  connCli.GetIP() << " on port " << connCli.GetPort() << " is connected to fd " << connCli.GetSockfd() << endl;
	        break;
		}

		while(1){
			connCli.RecvData(&head,sizeof(head));
			connCli.RecvData(msgStr, head.dataLen);
			msgStr[head.dataLen]=0;
			connCli.SendData(msgStr, head.dataLen);
			cout << "Received "<< msgStr << "\nEcho back ...\n";
		}
	}catch(const ConnectionException &e){
		cout << e.what();
	}
}
