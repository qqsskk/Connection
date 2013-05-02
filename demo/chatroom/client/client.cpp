/*************************************************************************
	> File Name: client.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 01时00分06秒
 ************************************************************************/
/*
	执行流程：
	1. connect to server
	2. input username and password
	3. send request
	4. get reply
**/
#include <Connection.h>
#include "../protocal.h"
#include <stdlib.h> //exit
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>

using namespace std;
#define SERV_PORT 8000
ConnectionIPV4 conn;
#define MAX_BUFFER_SIZE 1024

void *RecvFunc(void*)
{
	char buffer[MAX_BUFFER_SIZE];
	UserInfo *pUserInfo = NULL;
	ReplyHead *pReplyHead = (struct ReplyHead*)buffer;
	ReplyShow *pReplyShow = (struct ReplyShow*)buffer;
	ReplyChat *pReplyChat = (struct ReplyChat*)buffer;

	while(true){
		conn.RecvData(pReplyHead, sizeof(ReplyHead));
		if(pReplyHead->replyLength > 0){
			conn.RecvData(buffer + sizeof(struct ReplyHead), pReplyHead->replyLength);
		}
		if(pReplyHead->replyNo == REPLY_SHOW){
			int n = pReplyShow->n;
			pUserInfo  = (UserInfo *)(pReplyShow->user);
			cout << "There are " << n << " people online now" << endl;
			while(n > 0){
				cout << pUserInfo->name << endl;
				pUserInfo = (UserInfo *)((char *)pUserInfo + pUserInfo->length + sizeof(int));
				n--;
			}
		}else if(pReplyHead->replyNo == REPLY_CHAT){
			cout << pReplyChat->str << ": " ;
			cout << pReplyChat->str + pReplyChat->nameLength << endl;
		}
	}
}

int main(int argc, char **argv)
{
	string username, passwd;
	char buffer[MAX_BUFFER_SIZE];

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
       	cout << "Connect to server ok" << endl;
		cout << "User Name: " ;
		cin >> username;
		cout << "Password: ";
		cin >>  passwd;

		//login
		ReqLogin *req = (struct ReqLogin*)buffer;
		req->head.reqNo = REQ_LOGIN;
		req->head.reqLength = username.size() + passwd.size() + 2 + sizeof(int); //include two '\0'
		req->nameLength = username.size() + 1;
		strcpy(req->str, username.c_str());
		strcpy(req->str+ username.size() + 1, passwd.c_str());
		if(conn.SendData(req, sizeof(struct ReqLogin) + username.size() + passwd.size() +2) == false){
			cout << conn.GetLastError() << endl;
			return -1;
		}

		//read for reply
		ReplyLogin *login = (struct ReplyLogin*)buffer;
		conn.RecvData(login, sizeof(struct ReplyLogin));	

		if(login->ok == false){
			cout << "username not exist or passwd uncorrect " << endl;
			return -1;
		}
		cout << "Loging Success" << endl;

		//login ok, continue
		//split to two threads, main thread: send request, new thread: recv message
		pthread_t threadId;
		pthread_create(&threadId, NULL, RecvFunc, NULL);
		
		int choice;
		ReqShow *pReqShow = (struct ReqShow*)buffer;
		ReqChat *pReqChat = (struct ReqChat*)buffer;
		while(true){
			cout << "1: show the online persons " << endl;
			cout << "2: chat to someone " << endl;
			cout << "Your choice: " << endl;
			cin >> choice;
			
			if(choice == 1){
				pReqShow->head.reqNo = REQ_SHOW;
				pReqShow->head.reqLength = 0;
				conn.SendData(pReqShow, sizeof(struct ReqShow));		
			}else if(choice == 2){
				string nameStr,chatStr;
				cout << "Talk to whom(?) : ";
				cin >> nameStr;
				cout << "You: ";
				cin >> chatStr;

				pReqChat->head.reqNo = REQ_CHAT;
				pReqChat->head.reqLength = sizeof(int) + nameStr.size() + chatStr.size() + 2;
				req->nameLength = nameStr.size() + 1;
				strcpy(pReqChat->str, nameStr.c_str());
				strcpy(pReqChat->str + nameStr.size() + 1, chatStr.c_str());

				conn.SendData(pReqChat, sizeof(struct ReqHead) + sizeof(int) + nameStr.size() + chatStr.size() + 2);
			}else continue;
			sleep(1);
		}
	}catch(const ConnectionException &e){
		cout << e.what();
	}
	return 0;
}
