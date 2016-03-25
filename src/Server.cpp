#include "Server.h"
#include <direct.h>
#include <io.h>

//#include "depends/threadpool/boost/threadpool.hpp"
#include "threadpool.hpp"

#include <boost/thread/mutex.hpp>

using namespace boost::threadpool;

CServer::CServer(void)
{
}


CServer::~CServer(void)
{
}

void CServer::HandleRequest(int clientSocket, sockaddr_in clientAddr)
{
	char buf[10240];
	memset(buf, 0, sizeof(buf));

	int size = recv(clientSocket, buf, 1024, 0);
	if (size < 0)
	{
		cout<<"recv failed"<<endl;
		exit(1);
	}
	else if (size > 0)
	{
		//cout<<buf<<endl;
		ParseRequest(clientSocket, clientAddr, buf);
	}
}

int CServer::ParseRequest(int clientSock, const sockaddr_in& clientAddr, char *req)
{
	char currtime[32], log[10240];
	int line_total, method_total, query_total, i;
	RequestInfo requestInfo;

	getdate(currtime);
	vector<string> buf;
	split(string(req), string("\n"), buf);

	// 检查请求是否为空
	if (buf[0]=="\n" || buf[0]=="\r\n")
	{
		PrintError(clientAddr, string("Can't parse request."));
		return -1;
	}

	// 检查GET请求
	vector<string> method_buf;
	split(buf[0], string(" "), method_buf);
	if (toLowerString(method_buf[0])!="get")
	{
		PrintError(clientAddr, string("That method is not implemented."));
	}
	vector<string> query_buf;
	split(method_buf[1], string("?"), query_buf);

	char path[1024];
	getcwd(path, sizeof(path));
	string absPath(path);
	absPath += "/res";
	absPath = replaceBackplace(absPath);
	
	string relPath = query_buf[0].substr(0, query_buf[0].rfind("/"));
	string file = query_buf[0].substr(query_buf[0].rfind("/")+1);

	memset(&requestInfo, 0, sizeof(requestInfo));
	
	requestInfo.method = method_buf[0];
	requestInfo.pathinfo = query_buf[0];
	requestInfo.query = (query_buf.size() == 2 ? query_buf[1] : "");
	requestInfo.protocal = toLowerString(method_buf[2]);
	requestInfo.path = relPath;
	requestInfo.file = file;
	requestInfo.physical_path = absPath+query_buf[0];

	ProcessRequest(clientSock, clientAddr, requestInfo);

	return 0;
}

int CServer::ProcessRequest(int client_sock, sockaddr_in client_addr, RequestInfo request_info)
{
	if ( access(request_info.physical_path.c_str(), 0) != 0 )
	{
		char buf[128];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "File %s is not found.", request_info.pathinfo.c_str());
		PrintError(client_addr, string(buf));
		return -1;
	}

	if (isDir(request_info.physical_path) == false)
	{
		SendFile(client_sock, client_addr, request_info.physical_path, request_info.pathinfo);
	} 
	else
	{
		SendIndexHtml(client_sock, client_addr, request_info.physical_path, request_info.pathinfo);
	}

	return 0;
}

void CServer::InitAndRun()
{
	int serverSocket, clientSocket;
	sockaddr_in serverAddr, clientAddr;
	char currtime[32];
	WSADATA Ws;

	if (WSAStartup(MAKEWORD(2,2), &Ws) != 0)
	{
		cout<<"WSAStartup failed"<<endl;
		exit(1);
	}

	if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		cout<<"socket failed"<<endl;
		exit(1);
	}

	memset(&serverAddr, 0, sizeof(serverAddr)); 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	if (::bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		cout<<"bind failed"<<endl;
		exit(1);
	}

	if (listen(serverSocket, MAX_CLIENT) < 0)
	{
		cout<<"listen failed"<<endl;
		exit(1);
	}

	getdate(currtime);
	fprintf(stdout, "[%s] Start server listening at port %d ...\n", currtime, PORT);
	fprintf(stdout, "[%s] Waiting client connection ...\n", currtime);

	pool tp(100);

	while (1)
	{
		int clientLength = sizeof(clientAddr);

		if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientLength)) < 0)
		{
			cout<<"accept failed"<<endl;
			exit(1);
		}

		//HandleRequest(clientSocket, clientAddr);
		tp.schedule(boost::bind(&CServer::HandleRequest, this, clientSocket, clientAddr));

		closesocket(clientSocket);
	}

	closesocket(serverSocket);  
	WSACleanup();  
}

void CServer::PrintError(const sockaddr_in& clientAddr, const string& msg)
{
	char currtime[32],log[10240];
	getdate(currtime);

	memset(log, 0, sizeof(log));
	sprintf(log, "[%s] %s %s", currtime, inet_ntoa(clientAddr.sin_addr), msg.c_str());
	string l(log);
	cout<<l<<endl;
}

int CServer::SendFile(int clientSocket, const sockaddr_in& clientAddr, const string& fileName, const string& pathInfo)
{
	char buf[128], contents[8192];
	FILE* fp;
	size_t fileSize;

	string mimeType = MimeContentType(fileName);
	SendHeaders(clientSocket, 200, string("OK"), string(""), mimeType.c_str(), filesize(fileName), 0);

	fp = fopen(fileName.c_str(), "rb");
	if (fp==NULL)
	{
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "open file %s failed.", pathInfo);
		PrintError(clientAddr, string(buf));
		return -1;
	}
	
	int num = 0;  
	while(!feof(fp))  
	{  
		num = fread(contents, 1, BUFFER_SIZE, fp);  
		send(clientSocket, contents, num, 0);  
	}
	fclose(fp);

	return 0;
}

int CServer::SendIndexHtml(int client_sock, const sockaddr_in& client_addr, const string& path, const string& pathinfo)
{
	vector<string> fileList;
	getCurDirFiles(path, fileList);
	for (int i=0;i<fileList.size();i++)
	{
		if (fileList[i].find("index.html") != string::npos)
		{
			SendFile(client_sock, client_addr, fileList[i], pathinfo);
			return 0;
		}
	}
}

int CServer::SendHeaders(int client_sock, int status, const string& title, const string& extra_header, const string& mime_type, long length, time_t mod)
{
	time_t now;
	char timebuf[100], buf[BUFFER_SIZE], buf_all[REQUEST_MAX_SIZE], log[8];

	memset(buf_all, 0, REQUEST_MAX_SIZE);
	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "%s %d %s\r\n", "HTTP/1.0", status, title.c_str());
	strcpy(buf_all, buf);

	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "Server: %s\r\n", SERVER_NAME);
	strcat(buf_all, buf);

	now = time( (time_t*) 0 );
	strftime( timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime( &now ) );
	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "Date: %s\r\n", timebuf);
	strcat(buf_all, buf);

	if (extra_header.size() > 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "%s\r\n", extra_header.c_str());
		strcat(buf_all, buf);
	}
	if (mime_type.size() > 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "Content-Type: %s\r\n", mime_type.c_str());
		strcat(buf_all, buf);
	}
	if (length >= 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "Content-Length: %lld\r\n", (long long)length);        
		strcat(buf_all, buf);
	}
	if (mod != (time_t) -1 )
	{
		memset(buf, 0, BUFFER_SIZE);
		strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&mod));
		sprintf(buf, "Last-Modified: %s\r\n", timebuf);
		strcat(buf_all, buf);
	}
	memset(buf, 0, strlen(buf));
	sprintf(buf, "Connection: close\r\n\r\n");
	strcat(buf_all, buf);

	send(client_sock, buf_all, strlen(buf_all), 0);

	return 0;
} 

string CServer::MimeContentType(const string& name)
{
	string ret; 
	string fileType=name.substr(name.rfind("."));

	if (fileType.size()==0) 
	{
		ret = "application/octet-stream"; 
	} 
	else 
	{
		if (fileType == ".txt")
		{
			ret = "text/plain";
		} 
		else if (fileType == ".css")
		{
			ret = "text/css";
		} 
		else if (fileType == ".js")
		{
			ret = "text/javascript";
		} 
		else if (fileType == ".xml")
		{
			ret = "text/xml";
		} 
		else if (fileType == ".html" || fileType == ".htm")
		{
			ret = "text/html";
		}
		else if (fileType == ".png")
		{
			ret = "image/png";
		} 
		else if (fileType == ".bmp")
		{
			ret = "application/x-MS-bmp";
		} 
		else if (fileType == ".jpg")
		{
			ret = "image/jpeg";
		}
		else 
		{
			ret = "application/octet-stream";
		}
	}
	return ret;
}
