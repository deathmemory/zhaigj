#pragma once
#include "SkinDialog.h"
#define ID_TIMER_SHOW_BF_TIP  0x11
#define ID_TIMER_HIDE_BF_TIP  0x12

#define ID_TIMER_START_SHOW   0x13
#define ID_TIMER_START_HIDE   0x14

#define ID_TIMER_BTN_TEXT	  0x15

#define ID_NELAPSE_TIMER      25
#define WM_BALLOON_MSG		  WM_USER+730
class CBalloonTip :public CSkinDialog
{
public:

	typedef enum
	{
		ID_BTN_COSE = 1000,
		ID_LAB_TITLE,
		ID_LAB_INFO,
		ID_LAB_ICON,
		ID_BTN_YES,
		ID_BTN_NO
	}ControlID;
	CBalloonTip();   // standard constructor
	virtual ~CBalloonTip();

	virtual void InitControls();
	virtual void Notify(TNotifyUI& msg);

	void SetImage(int nbk,int nClose,int nIcon);

	virtual LPCTSTR GetWindowClassName() const { return _T("CBalloonTip"); };
public:
	BOOL Initialization();

	DWORD BalloonPopup(HWND hParent,LPCTSTR pText,UINT uTitle,int nX,int nY,UINT uMsg = 0);

	void InitIconImage();

	void UpdateWindow();

	void OnTipDisappered();

	LRESULT OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void GetDrawTextSize(LPCTSTR lpStr, SIZE &retSize);

	void SetTitleID(UINT nID);

	LRESULT		OnTimer(UINT nevent);

	void SetMessageStyle(DWORD dwStyle);

	int			m_nFade;
	int			m_nBK;
	int			m_nClose;
	int			m_nIcon;
	CString		m_strTitle;
	CString		m_strInfo;

	UINT		m_uLinkMsg;

	static HWND m_hOnlyHwnd;

	HWND		m_hWndMsg;

	BOOL		m_bLinkSendOnce;

	UINT		m_nTitleID;

	DWORD		m_dwMsgStyle;

	int			m_btnTextCount;
	
	DWORD		m_dwRet;
};

