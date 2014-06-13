#pragma once
#include "../public/Thread.h"

class CGAThread : public CThreadImpl<CGAThread>
{
public:
	CGAThread(void):CThreadImpl<CGAThread>(CREATE_SUSPENDED){}
	~CGAThread(void){}
	DWORD Run();
};

