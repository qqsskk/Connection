/*************************************************************************
	> File Name: client.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 01时00分06秒
 ************************************************************************/
#include <Connection.h>
#include <Protocal.h>
#include <iostream>
#include <stdlib.h> //exit
#include <unistd.h>

using namespace std;
#define SERV_PORT 8000

int main(int argc, char **argv)
{
	ConnectionIPV4 conn;
    char msgStr[100];
    int n;
	struct ProtocalHead head;

	if(argc<2){
		cout << "Usage: client <IpAddress>\n";
		exit(1);
	}
	try{
		conn.InitialSocket();
		while(conn.ConnectToServer(argv[1],SERV_PORT) == false) sleep(3);
        cout << "Connect ok" <<endl;

	//receive head
	conn.RecvData(&head, sizeof(head));
	//receive data
        conn.RecvData(msgStr, head.dataLen);
        msgStr[head.dataLen] = 0;

        cout << "Received msg: " << msgStr << endl;
		while(1) sleep(10);
	}catch(const ConnectionException &e){
		cout << e.what();
	}
}
