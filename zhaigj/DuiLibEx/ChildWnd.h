#pragma once
namespace DuiLib {

class CChildWnd :public CDialogLayoutUI, public INotifyUI
{
public:
	CChildWnd(void);
	~CChildWnd(void);
	
	virtual BOOL		Initialization(HWND hParent,RECT rect);

	virtual void		InitChildWindow() = 0;

	void				SetSkinImage(LPCTSTR);
	void				SetSkinImage(UINT);

	CButtonUI*			RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CLabelUI*			RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CRichEditUI*		RegiserEdit(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CProgressUI*		RegiserProgress(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CSliderUI*			RegiserSliderCtrl(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CListUI*			RegiserList(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CScrollBarUI*		RegiserVScrollBar(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CControlUI*			RegiserControl(CControlUI*pControl, UINT nId, RECT rect, LPCTSTR lpszText = NULL);

	CChildWnd*			RegiserChild(CChildWnd *p,LPCTSTR strName ,RECT rect);

	virtual void		Notify(TNotifyUI& msg) = 0;
	
	LRESULT				HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	CPaintManagerUI m_pm;
private:
	
	HWND				m_hParent;
	CStdStringPtrMap	m_ChildWnd;
};
}