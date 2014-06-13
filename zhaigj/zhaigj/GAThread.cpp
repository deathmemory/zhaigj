#include "StdAfx.h"
#include "GAThread.h"

#define CATEGORY_MAINPROGRAM				_T("mainProgram")

DWORD CGAThread::Run(){
	gaClient.Event(CATEGORY_MAINPROGRAM, _T("mainStart"));
	while ( ! IsThreadCanceled() ){
		Sleep(1000 * 60 * 10);	// 10 min
		gaClient.Event(CATEGORY_MAINPROGRAM, _T("HeartBeat"));
	}
	return 0;
}