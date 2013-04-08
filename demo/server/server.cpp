/*************************************************************************
	> File Name: server.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 01时00分06秒
 ************************************************************************/
#include "Connection.h"
#include <iostream>
#include <unistd.h>
#define LISTEN_PORT 8000
#define LISTEN_QUEUE 5
using namespace std;

int main()
{
	ConnectionIPV4 connListen, connCli;
    char msgStr[100] = "hello, client";
	try{
		connListen.InitialSocket();
		connListen.BindIpPort(NULL, LISTEN_PORT);
		connListen.ListenForClient(LISTEN_QUEUE);
		cout << "Listen ...\n";
		while(connListen.AcceptClient(connCli)){
			cout <<"Client " <<  connCli.GetIP() << " on port " << connCli.GetPort() << " is connected to fd " << connCli.GetSockfd() << endl;
            break;
		}
        cout << "Send Msg: " << msgStr << endl;
        connCli.SendData(msgStr, 100);
        cout << "Send ok" << endl;
		while(1) sleep(10);
	}catch(const ConnectionException &e){
		cout << e.what();
	}
}
