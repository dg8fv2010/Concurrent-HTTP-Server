#pragma once

#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <map>

#include "util.h"
#include "threadpool.hpp"
#include <boost/thread/mutex.hpp>

#pragma comment(lib,"ws2_32.lib")

using namespace std;
using namespace boost::threadpool;

#define MAX_CLIENT			100
#define PORT				9999
#define IP_ADDRESS			"127.0.0.1"
#define SERVER_NAME			"HTTPServer"
#define BUFFER_SIZE			8192
#define REQUEST_MAX_SIZE	10240


struct RequestInfo 
{
	string method;
	string pathinfo;
	string query;
	string protocal;
	string path;
	string file;
	string physical_path;
	string ifModifiedSince;
};

class CServer
{
public:
	CServer(void);
	~CServer(void);

	void	InitAndRun();
	void	HandleRequest(int client_sock, sockaddr_in client_addr);
	int		ParseRequest(int client_sock, const sockaddr_in& client_addr, char *req);
	int		ProcessRequest(int client_sock, sockaddr_in client_addr, RequestInfo request_info);
	
	void	PrintError(const sockaddr_in& client_addr, const string& msg);

	int		SendHeaders(int client_sock, int status, const string& title, const string& extra_header, const string& mime_type, long length, time_t mod);
	int		SendFile(int client_sock, const sockaddr_in& client_addr, const string& filename, const string& pathinfo, const string& lastModifiedTime);
	int		SendIndexHtml(int client_sock, const sockaddr_in& client_addr, const string& path, const string& pathinfo, const string& lastModifiedTime);
	
	string	MimeContentType(const string& name);
};

