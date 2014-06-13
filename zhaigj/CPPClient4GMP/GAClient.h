#pragma once
#define GA_HOST 				L"www.google-analytics.com"
#define COLLECT_PATH 			L"/collect"
#define USER_AGENT 				L"GA/1.0"
#define HEADER_CONTENT		 	L"Content-Type:application/xml\r\n"
#define HEADER_CONNECTION	 	L"Connection:keep-alive\r\n"
#define HEADER_ACCEPT 			L"Accept:*/*\r\n"
#define HEADER_ACCEPT_ENCODE	L"Accept-Encoding:gzip,deflate,sdch\r\n"
#define	HEADER_ACCEPT_LANG 		L"Accept-Language:zh-CN,Zh;q=0.8\r\n"
#define HEADER_ACCEPT_CHAR		L"Accept-Charset:GBK,utf-8;q=0.7,*;q=0.3\r\n\r\n"


#include <string>
#include <map>
#include <sstream>
#define nullptr NULL

using namespace std;
class GAClient
{
public:
	GAClient(wchar_t* clientId, wchar_t* trackingId);
	~GAClient(void);
private:
	wstring m_trackingId;
	wstring m_clientId;

public:
	static GAClient* GetGAClient(wchar_t* clientId, wchar_t* trackingId);

public:
	int PageView(wchar_t* path,
		wchar_t* host = NULL,
		wchar_t* title = NULL);

	int Event(wchar_t* category,
	 wchar_t* action,
	 wchar_t* label = NULL,
	 wchar_t* value = NULL);

	int Exception(wchar_t* desc, 
		bool fatal = true);

protected:
	int SendWebRequest(map<wstring,wstring> & reqMap);

};
