#pragma once
template <BOOL t_bManaged>
class CThreadT
{
public:
	HANDLE m_hThread;
	DWORD m_dwThreadId;

public:
	CThreadT(HANDLE hThread = NULL, DWORD dwThreadId = 0)
		: m_hThread(hThread), m_dwThreadId(dwThreadId)
	{
	}

	CThreadT(const CThreadT& otherThread)
	{
		m_dwThreadId = otherThread.m_dwThreadId;
		m_bCancel = FALSE;
		if ( t_bManaged && otherThread.m_hThread != NULL )
		{
			HANDLE hProcess = GetCurrentProcess();
			DuplicateHandle( hProcess, otherThread.m_hThread, hProcess, &m_hThread,
				0, FALSE, DUPLICATE_SAME_ACCESS );
			ASSERT( m_hThread != NULL );
		}
		else
			m_hThread = otherThread.m_hThread;
	}

	~CThreadT()
	{
		if ( t_bManaged && m_hThread != NULL )
			CloseHandle(m_hThread);
	}

	static CThreadT Create(LPTHREAD_START_ROUTINE pThreadProc, LPVOID pParam = NULL,
		DWORD dwCreationFlags = 0, LPSECURITY_ATTRIBUTES pSecurityAttr = NULL,
		DWORD dwStackSize = 0)
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = (HANDLE) _beginthreadex(pSecurityAttr, dwStackSize,
			(unsigned (__stdcall*)(void*)) pThreadProc,
			pParam, dwCreationFlags, (unsigned*) &dwThreadId);

		return CThreadT(hThread, dwThreadId);
	}

	static CThreadT Open(DWORD dwThreadId, DWORD dwDesiredAccess = THREAD_ALL_ACCESS,
		BOOL bInheritHandle = FALSE)
	{
		HANDLE hThread = OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
		return CThreadT(hThread, dwThreadId);
	}



	DWORD GetId() const
	{
		return m_dwThreadId;
	}


	virtual HANDLE GetHandle() const
	{
		return m_hThread;
	}


	int GetPriority() const
	{
		ASSERT( m_hThread != NULL );
		return GetThreadPriority(m_hThread);
	}


	BOOL SetPriority(int nPriority)
	{
		ASSERT( m_hThread != NULL );
		return SetThreadPriority(m_hThread, nPriority);
	}


	DWORD GetExitCode() const
	{
		ASSERT( m_hThread != NULL );
		DWORD dwExitCode = 0;
		if ( GetExitCodeThread(m_hThread, &dwExitCode) )
			return dwExitCode;
		else
			return (DWORD) -1;
	}


	BOOL GetThreadTimes(LPFILETIME pCreationTime, LPFILETIME pExitTime, LPFILETIME pKernelTime,
		LPFILETIME pUserTime) const
	{
		//ASSERT( m_hThread != NULL );
		return ::GetThreadTimes(m_hThread, pCreationTime, pExitTime, pKernelTime, pUserTime);
	}
	DWORD Resume()
	{
		//ASSERT( m_hThread != NULL );
		return ResumeThread(m_hThread);
	}


	DWORD Suspend()
	{
		//ASSERT( m_hThread != NULL );
		return SuspendThread(m_hThread);
	}


	BOOL Terminate(DWORD dwExitCode = 0)
	{
		//ASSERT( m_hThread != NULL );
		return TerminateThread(m_hThread, dwExitCode);
	}


	void Exit(DWORD dwExitCode = 0)
	{
		// Make sure this is only called from the thread that this object represents
		//ASSERT( m_dwThreadId == ::GetCurrentThreadId() );
		_endthreadex(dwExitCode);
	}


	DWORD Join(DWORD dwWaitMilliseconds = INFINITE) const
	{
		//ASSERT( m_hThread != NULL );
		return WaitForSingleObject(m_hThread, dwWaitMilliseconds);
	}

public:
	BOOL m_bCancel;
	void CancelThread()
	{
		m_bCancel = TRUE;
	}
	BOOL IsThreadCanceled()
	{
		return m_bCancel;
	}
};


typedef CThreadT<true> CThread;
typedef CThreadT<false> CThreadHandle;





template <class T>
__declspec(naked) DWORD WINAPI _ThreadProcThunk(void*)
{
	__asm
	{
		pop		eax
			pop		ecx
			push	eax
			xor		eax, eax
			jmp T::Run
	}
}



template <class T, class TBase = CThread>
class CThreadImpl : public TBase
{
	typedef CThreadImpl<T, TBase> thisClass;

public:
	CThreadImpl(DWORD dwCreationFlags = 0)
		: TBase(NULL, NULL)
	{
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk<T>,
			this, dwCreationFlags, &m_dwThreadId);
	}
};