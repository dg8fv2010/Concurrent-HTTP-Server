#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define MAX_CLIENT 10
#define PORT 9999
#define IP_ADDRESS "127.0.0.1"

void HandleRequest(int client_sock, sockaddr_in client_addr)
{
	char buf[10240];
	memset(buf, 0, sizeof(buf));

	if (recv(client_sock, buf, 1024, 0) < 0)
	{
		cout<<"recv failed"<<endl;
		exit(1);
	}
	else
	{
		cout<<buf<<endl;
	}
}

void InitServerListen()
{
	int serversock, clientsock;
	sockaddr_in server_addr, client_addr;
	char currtime[32];
	WSADATA Ws;

	if (WSAStartup(MAKEWORD(2,2), &Ws) != 0)
	{
		cout<<"WSAStartup failed"<<endl;
		exit(1);
	}

	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		cout<<"socket failed"<<endl;
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	if (bind(serversock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		cout<<"bind failed"<<endl;
		exit(1);
	}
	
	if (listen(serversock, MAX_CLIENT) < 0)
	{
		cout<<"listen failed"<<endl;
		exit(1);
	}

	char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	sprintf(currtime, "%d-%d-%d %d:%d:%d", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	fprintf(stdout, "[%s] Start server listening at port %d ...\n", currtime, PORT);
	fprintf(stdout, "[%s] Waiting client connection ...\n", currtime);

	while (1)
	{
		int clientlen = sizeof(client_addr);
		
		if ((clientsock = accept(serversock, (struct sockaddr *) &client_addr, &clientlen)) < 0)
		{
			cout<<"accept failed"<<endl;
			exit(1);
		}

		HandleRequest(clientsock, client_addr);

		closesocket(clientsock);
	}

	closesocket(serversock);  
	WSACleanup();  
}

int main()
{
	InitServerListen();

	return 0;
}