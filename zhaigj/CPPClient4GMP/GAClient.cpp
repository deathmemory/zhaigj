#include "StdAfx.h"
#include "GAClient.h"
#pragma comment(lib,"Winhttp.lib")
#define REQ_HEADER_CONTENT_TYPE 

GAClient::GAClient( wchar_t* trackingId,wchar_t* clientId)
	:m_trackingId(trackingId),
	m_clientId(clientId){

	srand((unsigned)time(NULL));
}


GAClient* GAClient::GetGAClient( wchar_t* clientId, 
	wchar_t* trackingId ){

	GAClient* singleInstance = new GAClient(clientId,trackingId);
	return singleInstance;
}

GAClient::~GAClient(void){

}


int GAClient::Exception(wchar_t* desc, 
	bool fatal){

	map<wstring,wstring> reqMap;

	reqMap[	L"v"	] = L"1";
	reqMap[	L"tid"	] = m_trackingId;
	reqMap[	L"cid"	] = m_clientId;
	reqMap[	L"t"	] = L"exception";

	reqMap[	L"exd"	] = desc;
	if (fatal){
		reqMap[	L"exf"	] = L'1';
	}else{
		reqMap[	L"exf"	] = L'0';
	}

	return SendWebRequest(reqMap);
}

int GAClient::Event(wchar_t* category, 
	wchar_t* action,
	wchar_t* label,
	wchar_t* _value){

	map<wstring,wstring> reqMap;

	reqMap[	L"v"	] = L"1";
	reqMap[	L"tid"	] = m_trackingId;
	reqMap[	L"cid"	] = m_clientId;
	reqMap[	L"t"	] = L"event";

	reqMap[	L"ec"	] = category;
	reqMap[	L"ea"	] = action;
	if (label){
		reqMap[ L"el" ] = label;
	}
	if (_value){
		reqMap[ L"ev" ] = _value;
	}

	return SendWebRequest(reqMap);
}

int GAClient::PageView(wchar_t* path,
	wchar_t* url,
	wchar_t* title){

	map<wstring,wstring> reqMap;

	reqMap[	L"v"	]	= L"1";
	reqMap[	L"tid"	]	= m_trackingId;
	reqMap[	L"cid"	]	= m_clientId;
	reqMap[	L"t"	]	= L"pageview";

	reqMap[	L"dp"	]	= path;
	if (url){
		reqMap[	L"dh"	]	= url;
	}

	if (title){
		reqMap[	L"dt"	]	= title;
	}

	return SendWebRequest(reqMap);
}

int GAClient::SendWebRequest(map< wstring, wstring >& reqMap){

	HINTERNET session = nullptr;
	HINTERNET connect = nullptr;
	HINTERNET request = nullptr;

	// Create the WinHTTP session.
	session = ::WinHttpOpen( 
		USER_AGENT, 
		WINHTTP_ACCESS_TYPE_NO_PROXY, 
		WINHTTP_NO_PROXY_NAME, 
		WINHTTP_NO_PROXY_BYPASS, 
		0);

	if (session){

		connect = ::WinHttpConnect(session, 
			GA_HOST, 
			INTERNET_DEFAULT_HTTP_PORT, 
			0);

		if (connect){
			request = ::WinHttpOpenRequest(
				connect, 
				L"POST", 
				COLLECT_PATH, 
				L"HTTP/1.1", 
				WINHTTP_NO_REFERER, 
				WINHTTP_DEFAULT_ACCEPT_TYPES, 
				0);
		}
	}

	wstring headers[] = {
		HEADER_CONTENT, 
		HEADER_CONNECTION, 
		HEADER_ACCEPT, 
		HEADER_ACCEPT_ENCODE, 
		HEADER_ACCEPT_LANG, 
		HEADER_ACCEPT_CHAR};

	BOOL result = false;
	
	if (request){
		for(int i = 0; i < 6 ;i ++ ){
			result = ::WinHttpAddRequestHeaders(request, 
				headers[i].c_str(), 
				(unsigned long)-1, 
				WINHTTP_ADDREQ_FLAG_ADD);

			if (!result)	{
				DWORD err = GetLastError();
				printf("%d",err);
				break;
			}
		}

		if (result){
			std::wostringstream sb;
			std::map<wstring,wstring>::iterator it;
			for(it = reqMap.begin(); it != reqMap.end(); ++it ){
				sb << it -> first.c_str();
				sb << L"=";
				sb << it -> second.c_str();
				sb << L"&";
			}

			//	convert wstring to string
			std::wstring widestring = sb.str();

			int stringsize = WideCharToMultiByte(CP_UTF8, 
				0, 
				widestring.c_str(), 
				-1, 
				nullptr, 
				0, 
				nullptr, 
				nullptr);

			char* temp = new char[stringsize];

			WideCharToMultiByte(CP_UTF8, 
				0, 
				widestring.c_str(), 
				-1, 
				temp, 
				stringsize, 
				nullptr, 
				nullptr);

			std::string str = temp;
			delete [] temp;

			//	and add a random z
			str.append("z=");
			char num[5];
			_itoa_s(rand()%1000,num,10);
			str.append(num);

			//send request
			result = WinHttpSendRequest(request, 
				WINHTTP_NO_ADDITIONAL_HEADERS,  
				0, 
				(void*)str.c_str(),
				static_cast<unsigned long>(str.length()), 
				static_cast<unsigned long>(str.length()), 
				0);

			if (!result)	{
				DWORD err = GetLastError();
				printf("%d",err);
			}
		}
	}

	if(request){
		WinHttpCloseHandle(request);
	}

	if(connect){
		WinHttpCloseHandle(connect);
	}

	if (session){
		WinHttpCloseHandle(session);
	}

	return result;
}