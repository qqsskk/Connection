#include <EPollServer.h>
#include <Connection.h>
#include "../protocal.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>//strcpy
using namespace std;

#define LISTEN_PORT 8000
#define LISTEN_QUEUE 5
#define MAX_BUFFER_SIZE 1024
#define MAX_FDS 1024
#define MAX_EVENTS 10
typedef map<string, string> UserPasswdMap;
typedef map<string, ConnectionIPV4> UserConnMap;
typedef map<ConnectionIPV4, string> ConnUserMap;

UserPasswdMap userPasswdMap;
UserConnMap onlineUserConnMap;
ConnUserMap onlineConnUserMap;
char buffer[MAX_BUFFER_SIZE];

int main()
{
	ConcurrentServer *pServer = new EPollServer(MAX_FDS, MAX_EVENTS);
	ConcurrentServer::ClientVector waitClientVec;
	ConcurrentServer::ClientVectorIter iter;
	ReqHead *head = NULL;
	ReqLogin *pReqLogin = NULL;
	ReqRegister *pReqRegister = NULL;
	ReqChat *pReqChat = NULL;
	ReplyLogin *pReplyLogin = NULL;
	ReplyRegister *pReplyRegister = NULL;
	ReplyShow *pReplyShow = NULL;
	ReplyChat *pReplyChat = NULL;
	UserInfo *pUserInfo = NULL;
	string name, passwd,chatStr;

	//Init
	userPasswdMap.insert(make_pair("rensq", "123456"));
	userPasswdMap.insert(make_pair("shibojun", "123456"));
	userPasswdMap.insert(make_pair("lijin", "123456"));
	userPasswdMap.insert(make_pair("wangzaibing", "123456"));

	try{	
		cout << "Server Starting Service...\n";
		pServer->StartService(NULL, LISTEN_PORT, LISTEN_QUEUE);
		while(true){
			pServer->WaitForClient(waitClientVec);

			head = (struct ReqHead*)buffer;
			for(iter = waitClientVec.begin(); iter != waitClientVec.end(); ++iter){
				if(iter->RecvData(head, sizeof(struct ReqHead))){
					cout << "Receive data: " << head->reqNo << " " << head->reqLength << endl;
					switch(head->reqNo){
						case REQ_LOGIN:
							pReqLogin = (struct ReqLogin*)head;
							iter->RecvData(&(pReqLogin->nameLength), head->reqLength);
							name = string(pReqLogin->str);
							passwd = string(pReqLogin->str+pReqLogin->nameLength);
							pReplyLogin = (struct ReplyLogin *)buffer;
							pReplyLogin->head.replyNo = REPLY_LOGIN;
							pReplyLogin->head.replyLength = sizeof(bool);
							if(userPasswdMap.find(name) != userPasswdMap.end() && userPasswdMap[name] == passwd){
								//send ok to user
								pReplyLogin->ok = true;
								//add to online user map
								onlineUserConnMap.insert(make_pair(name, *iter));
								onlineConnUserMap.insert(make_pair(*iter, name));
							}else{
								//send failure to user
								pReplyLogin->ok = false;
							}
							iter->SendData(pReplyLogin, sizeof(struct ReplyLogin));
							break;
						case REQ_REGISTER:
							pReqRegister = (struct ReqRegister *)head;
							iter->RecvData(&(pReqRegister->nameLength), head->reqLength);
							name = string(pReqRegister->str);
							passwd = string(pReqRegister->str + pReqRegister->nameLength);
							pReplyRegister = (struct ReplyRegister*)buffer;
							pReplyRegister->head.replyNo = REPLY_REGISTER;
							pReplyRegister->head.replyLength = sizeof(bool);
							if(userPasswdMap.count(name) == 0){
								pReplyRegister->ok = true;
								//add to user map and online user map
								userPasswdMap.insert(make_pair(name, passwd));
								onlineUserConnMap.insert(make_pair(name, *iter));
								onlineConnUserMap.insert(make_pair(*iter, name));
							}else{
								pReplyRegister->ok = false;
							}
							iter->SendData(pReplyRegister, sizeof(struct ReplyRegister));
							break;
						case REQ_SHOW:
							pReplyShow = (struct ReplyShow*)buffer;
							pReplyShow->head.replyNo = REPLY_SHOW;
							pReplyShow->head.replyLength = sizeof(int);//one int
							pReplyShow->n = onlineUserConnMap.size();

							pUserInfo = &(pReplyShow->user[0]);/*one question: how to initialise one object in a specified memoey place*/
							for(UserConnMap::iterator iter2 = onlineUserConnMap.begin(); iter2 != onlineUserConnMap.end(); ++iter2){
								pUserInfo->length = iter2->first.size() + 1;
								strcpy(pUserInfo->name, iter2->first.c_str());
								pUserInfo = (struct UserInfo *)((char *)pUserInfo + sizeof(int) + iter2->first.size() + 1);
								pReplyShow->head.replyLength += (sizeof(int) + iter2->first.size() + 1);
							}
							iter->SendData(buffer, sizeof(struct ReplyHead) + pReplyShow->head.replyLength);	
							break;
						case REQ_CHAT:
							pReqChat = (struct ReqChat*)head;
							iter->RecvData(&(pReqChat->nameLength),head->reqLength);
							name = string(pReqChat->str);
							chatStr = string(pReqChat->str + pReqChat->nameLength);
							if(onlineUserConnMap.count(name) > 0){
								ConnectionIPV4& peekConn = onlineUserConnMap.find(name)->second;
								pReplyChat = (struct ReplyChat*)buffer;
								pReplyChat->head.replyNo = REPLY_CHAT;

								name = onlineConnUserMap.find(*iter)->second;
								pReplyChat->head.replyLength = sizeof(int) + name.size() + chatStr.size() + 2;
								pReplyChat->nameLength = name.size() + 1;
								strcpy(pReplyChat->str, name.c_str());
								strcpy(pReplyChat->str+name.size() + 1, chatStr.c_str());

								peekConn.SendData(pReplyChat, sizeof(struct ReplyHead) + pReplyChat->head.replyLength);
							}
							break;
						default:
							cout << "Non Exist Request" << endl;
							break;
					}
				}else{
					ConnUserMap::iterator iter2 = onlineConnUserMap.find(*iter);
					if(iter2 != onlineConnUserMap.end()){
						name = iter2->second;
						onlineUserConnMap.erase(onlineUserConnMap.find(name));
					}
					onlineConnUserMap.erase(onlineConnUserMap.find(*iter));
					pServer->RemoveClient(*iter);
				}
			}
		}//while(true)
		pServer->EndService();
	}catch(ConnectionException &e){
		cout << e.what() << endl;
	}
	
	delete pServer;
	return 0;
}
