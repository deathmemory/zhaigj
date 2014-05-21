#pragma once
namespace DuiLib {

#define CHILD_WND(T) (new T)
#define CONTROL(T) (new T)

#define ACTIVECONTAINER(x) this->SetActiveContainer(x);
#define UNACTIVECONTAINER  this->SetActiveContainer(NULL);

#define ID_PENDINGMAKS_TIMER 60000

class CUIWndLayer : public CWindowWnd
{
public:
	CUIWndLayer();

	void Init(HWND hwndParent, const RECT rc, int nAlpha = 80);

	LPCTSTR GetWindowClassName() const;

	void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	int m_nAlphaValue;
};


class CSkinDialog : public CWindowWnd, public INotifyUI,public IMessageFilterUI
{
public:
	CSkinDialog(void);

	virtual ~CSkinDialog();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT MessageHandler(LPMSG lpMsg, BOOL& bHandled);
	
	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam, BOOL &bHandled); 

	virtual LRESULT OnDestroy(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnNCLButtonDown(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnSystemCmd(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnUserMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual LRESULT OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual BOOL	Initialization() = 0;

	virtual DWORD	DefaultAction(DWORD dParam = 0);

	virtual void	BeforeCreate(DWORD dParam = 0);

	virtual void Notify(TNotifyUI& msg) = 0;

	LRESULT OnShowLayer( WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void SetSkinImage(LPCTSTR);

	void SetSkinImage(UINT);

	void SetSkinColor(COLORREF);

	void SetSkinCorner(RECT rc);

	CControlUI * FindControl(UINT nID);

	void SetStretchMode(CControlUI* pControl, UINT uMode);

	void MakeMask();

	void SetTitleIcon(UINT nID);

	void OnFinalMessage(HWND hWnd)
	{
		//delete this;
	}

	//UINT GetClassStyle() const;

	virtual UINT ShowModal(HWND hParentWindow  = ::GetActiveWindow());

	CButtonUI*			RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CLabelUI*			RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CEditUI*			RegiserEdit(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CRichEditUI*		RegiserRichEdit(UINT nId, RECT rect, LPCTSTR lpszText = NULL,DWORD dStyle = 0);
	CProgressUI*		RegiserProgress(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CSliderUI*			RegiserSliderCtrl(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CListUI*			RegiserList(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CScrollBarUI*		RegiserVScrollBar(UINT nId, RECT rect, LPCTSTR lpszText = NULL);

	CControlUI*			RegiserControl(CControlUI*pControl, UINT nId, RECT rect = CRect(0,0,0,0), LPCTSTR lpszText = NULL);

	void				SetActiveContainer(CContainerUI *pContainer);

	CPaintManagerUI	*	m_pPM;

	CContainerUI	 *	m_pRoot;

	CUIWndLayer*		m_pLayer;

	CLabelUI *			m_pLableTitle;

	BOOL				m_bPendingMask;

	CContainerUI *		m_pActiveContainer;

	BOOL				m_bIsPopup;
};

}