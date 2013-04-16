#include <EPollServer.h>
#include <Protocal.h>
#include <vector>
#include <iostream>
using namespace std;

#define LISTEN_PORT 8000
#define LISTEN_QUEUE 5
#define MAX_MSG_SIZE 1024
#define MAX_FDS 1024
#define MAX_EVENTS 10
int main()
{
	ConcurrentServer *pServer = new EPollServer(MAX_FDS, MAX_EVENTS);
	ConcurrentServer::ClientVector waitClientVec;
	char msgStr[MAX_MSG_SIZE];
	struct ProtocalHead head;
	bool bNewClient;
	ConcurrentServer::ClientVectorIter iter;
	
	try{	
		cout << "Server Starting Service...\n";
		pServer->StartService(NULL, LISTEN_PORT, LISTEN_QUEUE);
		while(true){
			bNewClient = pServer->WaitForClient(waitClientVec);

			//process new client
			iter = waitClientVec.begin();
			if(bNewClient){
				cout << "Client " << iter->GetIP() << ":" << iter->GetPort() << " connected in\n";
				++iter;
			}
			//process requests
			for(; iter != waitClientVec.end(); ++iter){
				if(iter->RecvData(&head,sizeof(head)) == false){
					cout << iter->GetLastError() << endl;
					cout << "Remove Client " << iter->GetIP() << ":" << iter->GetPort() << endl;
					pServer->RemoveClient(*iter);
					continue;
				}
				if(iter->RecvData(msgStr, head.dataLen) == false){
					cout << iter->GetLastError() << endl;
					cout << "Remove Client " << iter->GetIP() << ":" << iter->GetPort() << endl;
					pServer->RemoveClient(*iter);
					continue;
				}
				if(iter->SendData(msgStr, head.dataLen) == false){
					cout << iter->GetLastError() << endl;
					cout << "Remove Client " << iter->GetIP() << ":" << iter->GetPort() << endl;
					pServer->RemoveClient(*iter);
					continue;
				}
				
				msgStr[head.dataLen]=0;
				cout << "Received "<< msgStr << "\nEcho back ...\n";
			}
		}//while(true)
		pServer->EndService();
	}catch(ConnectionException &e){
		cout << e.what() << endl;
	}
	
	delete pServer;
	return 0;
}
