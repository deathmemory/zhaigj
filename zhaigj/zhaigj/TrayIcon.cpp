#include "stdafx.h"
#include "TrayIcon.h"

CTrayIcon::CTrayIcon(void)
{
	m_msgTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
}

CTrayIcon::~CTrayIcon(void)
{
	Shell_NotifyIcon(NIM_DELETE, &m_data);
}

void CTrayIcon::SetIconInfo(CString strInfo)
{
	LPCTSTR pstr = strInfo.GetBuffer(0);
	//lstrcpy(m_data.szTip ,pstr);
	memcpy(m_data.szTip,pstr,min(128,strInfo.GetLength())*sizeof(TCHAR));
	strInfo.ReleaseBuffer();
	Shell_NotifyIcon(NIM_MODIFY, &m_data);
}

BOOL CTrayIcon::CreateTrayIcon(HINSTANCE hInstance,HWND hWnd, DWORD resId)
{
	SecureZeroMemory(&m_data, sizeof(m_data));
	m_hMenuInstance = hInstance;
	m_data.cbSize = sizeof(m_data);
	m_data.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(resId));
	m_data.hWnd = hWnd;
	m_data.uID = resId;
	m_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
	m_data.uCallbackMessage = WM_TRAYICON;
	m_data.dwInfoFlags = NIIF_USER;
	lstrcpy(m_data.szInfoTitle, _T("MK"));
	lstrcpy(m_data.szTip ,_T("MK"));
	return Shell_NotifyIcon(NIM_ADD, &m_data);
}

LRESULT CTrayIcon::IconMessage(WPARAM wParam,LPARAM lParam)
{
	if (wParam != this->m_data.uID) return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP:
		PopUpMenu();
		break;
	case WM_LBUTTONDOWN:	
		ShowWindow(m_data.hWnd,SW_SHOW);
		::PostMessage(m_data.hWnd,WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(m_data.hWnd);
		
		break;
	case WM_LBUTTONDBLCLK:
		break;
		//去掉下面的注释可以使鼠标悬停在图标上时出现气泡提示，个人不太喜欢
	}

	return 0;
}
void CTrayIcon::PopUpMenu()
{
	if (m_data.hWnd)
	{
		SetForegroundWindow(m_data.hWnd);
		::PostMessage(m_data.hWnd,WM_TRAYMENU,0,0);
	}
	return;
}