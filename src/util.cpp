#include "util.h"

void getdate(char *s)
{
	char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	sprintf(s, "%d-%d-%d %d:%d:%d",(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}  

void split(const string& s, std::string& delim,std::vector<string>& ret)  
{  
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		ret.push_back(s.substr(last,index-last));  
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
	if (index-last>0)  
	{  
		ret.push_back(s.substr(last,index-last));  
	}  
} 

string replaceAll(string& str, const string& old_value,const string& new_value)     
{     
	while(true)   
	{     
		string::size_type   pos(0);     
		if((pos=str.find(old_value)) != string::npos)     
		{
			str.replace(pos,old_value.length(),new_value);     
		}
		else
		{
			break;
		}
	}     
	return str;     
}

string replaceAllDistinct(string& str, const string& old_value, const string& new_value)     
{     
	for(string::size_type pos(0);pos!=string::npos;pos+=new_value.length())   
	{     
		if((pos=str.find(old_value,pos))!=string::npos)     
		{
			str.replace(pos,old_value.length(),new_value);     
		}
		else
		{
			break;
		}
	}     
	return str;     
}     

string toUpperString(string& src)  
{  
	string dst;
	transform(src.begin(), src.end(), dst.begin(), (int (*)(int))toupper);  
	return dst;
}  
string toLowerString(string& src)  
{  
	string dst=src;
	transform(src.begin(), src.end(), dst.begin(), (int (*)(int))tolower);
	return dst;
}  

long filesize(const string& file)
{
	FILE *fp;   
	fp=fopen(file.c_str(),"r");   
	if (fp==NULL)
	{
		cout<<"file lost."<<endl;
	}
	long fileLength=filelength(fileno(fp));   
	fclose(fp); 
	return fileLength;
}

bool isDir(const string& path) 
{
	bool isFolder = GetFileAttributes(path.c_str())&FILE_ATTRIBUTE_DIRECTORY;
	return isFolder;
}

void getAllDirFiles(string path, vector<string>& files) 
{
	if (path[path.size()-1] == '/')
	{
		path=path.substr(0, path.size()-1);
	}
	//文件句柄  
	long hFile = 0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(),&fileinfo)) != -1)  
	{  
		do  
		{  
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR)) 
			{  
				if (strcmp(fileinfo.name,".")!=0 && strcmp(fileinfo.name,"..")!=0)  
					getAllDirFiles(p.assign(path).append("/").append(fileinfo.name), files);  
			}  
			else  
			{  
				//files.push_back(p.assign(path).append("/").append(fileinfo.name));
				string fileName(p.assign(path).append("/").append(fileinfo.name));
				int type_pos = fileName.rfind(".");
				files.push_back(fileName);
			}  
		} while (_findnext(hFile,&fileinfo) == 0);  
		_findclose(hFile);  
	}
}

void getCurDirFiles(string path, vector<string>& files)
{
	if (path[path.size()-1] == '/')
	{
		path=path.substr(0, path.size()-1);
	}
	//文件句柄  
	long hFile = 0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(),&fileinfo)) != -1)  
	{  
		do  
		{  
			string fileName(p.assign(path).append("/").append(fileinfo.name));
			int type_pos = fileName.rfind(".");
			files.push_back(fileName);
		} while (_findnext(hFile,&fileinfo) == 0);  
		_findclose(hFile);  
	}
}

string replaceBackplace(const string& str)
{
	string s=str;
	for (int i=0;i<s.size();i++)
	{
		if (s[i]=='\\')
		{
			s[i]='/';
		}
	}
	return s;
}

string getFileName(const string& str)
{
	int npos=str.rfind("/");
	string name=str.substr(npos+1);
	return name;
}

time_t SystemTimeToTimet(SYSTEMTIME st)
{
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);

	LONGLONG nLL;
	ULARGE_INTEGER ui;

	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	nLL = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;
	time_t pt = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);

	return pt;
}

time_t getLastModifiedTime(const string& fileName)
{
	HANDLE hFile = CreateFile(fileName.c_str(),              
		GENERIC_READ,  //必须有GENERIC_READ属性才能得到时间     
		FILE_SHARE_READ,                      
		NULL,                   
		OPEN_EXISTING,         
		FILE_ATTRIBUTE_NORMAL,
		NULL);                 
	time_t mod = 0;
	if (hFile != INVALID_HANDLE_VALUE) 
	{ 
		SYSTEMTIME sysTime;
		FILETIME fCreateTime, fAccessTime, fWriteTime;

		GetFileTime(hFile, &fCreateTime, &fAccessTime, &fWriteTime);//获取文件时间
		FileTimeToSystemTime(&fWriteTime, &sysTime);//将文件时间转换为本地系统时间

		mod = SystemTimeToTimet(sysTime);
	}
	return mod;
}

string getTimeStr(time_t mod)
{
	char timebuf[128];
	//strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&mod));
	//sprintf(buf, "Last-Modified: %s\r\n", timebuf);
	return string(timebuf);
}