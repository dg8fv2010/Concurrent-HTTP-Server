#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <Windows.h>
#include <io.h>
#include <direct.h>

using namespace std;

void getdate(char *s);

void split(const string& s, std::string& delim,std::vector<string>& ret);

string replaceAll(string& str, const string& old_value,const string& new_value);

string replaceAllDistinct(string& str, const string& old_value, const string& new_value);

string toUpperString(string& src);

string toLowerString(string& src);

long filesize(const string& file);

bool isDir(const string& path);

void getAllDirFiles(string path, vector<string>& files);

void getCurDirFiles(string path, vector<string>& files);

string replaceBackplace(const string& str);

string getFileName(const string& str);

time_t SystemTimeToTimet(SYSTEMTIME st);

time_t getLastModifiedTime(const string& fileName);

string getTimeStr(time_t mod);