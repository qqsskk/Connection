/*************************************************************************
	> File Name: client.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 01时00分06秒
 ************************************************************************/
#include <Connection.h>
#include "../Protocal.h"
#include <stdlib.h> //exit
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;
#define SERV_PORT 8000
#define BUFFER_SIZE 1024
int main(int argc, char **argv)
{
	ConnectionIPV4 conn;
	char msgStr[BUFFER_SIZE];
	struct ProtocalHead head;
	int n;

	if(argc<2){
		cout << "Usage: client <IpAddress>\n";
		exit(1);
	}
	try{
		conn.InitialSocket();
		while(conn.ConnectToServer(argv[1],SERV_PORT) == false) {
			cout << conn.GetLastError() << endl;
			sleep(3);
		}
        	cout << "Connect ok" <<endl;

		while(fgets(msgStr, BUFFER_SIZE, stdin)){
			n = strlen(msgStr);
			--n;
			if(n>1){
				msgStr[n] = 0;
				head.dataLen = n;
				
				//send head
				if(conn.SendData(&head,sizeof(head))== false){
					cout << conn.GetLastError() << endl;
					return -1;
				}
				//recv head
				if(conn.SendData(msgStr, n) == false){
					cout << conn.GetLastError() << endl;
					return -1;
				}
				//receive data
			        if(conn.RecvData(msgStr, n) == false){
					cout  << conn.GetLastError();
					return -1;
			        }
				msgStr[n] = 0;

			        cout << "Received msg: " << msgStr << endl;
			}
		}
	}catch(const ConnectionException &e){
		cout << e.what();
	}
	
	return 0;
}
