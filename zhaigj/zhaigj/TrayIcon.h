#pragma once

#define  WM_TRAYICON (WM_USER+245)
#define  WM_TRAYMENU (WM_USER+246)
#include <atlstr.h>
#include <Shellapi.h>

class CTrayIcon
{
public:
	CTrayIcon(void);
	~CTrayIcon(void);

	BOOL CreateTrayIcon(HINSTANCE hInstance,HWND hWnd, DWORD resId);
	LRESULT IconMessage(WPARAM wParam,LPARAM lParam);
	void PopUpMenu();

	void SetIconInfo(CString strInfo);

	UINT m_msgTaskbarRestart;
private:
	NOTIFYICONDATA m_data;
	HINSTANCE m_hMenuInstance;

};
