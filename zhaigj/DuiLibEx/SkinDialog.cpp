#include "StdAfx.h"
#include "SkinDialog.h"


//////////////////////////////////////////////////////////////////////////////////
CUIWndLayer::CUIWndLayer()
{
	m_nAlphaValue = 80;
}

void CUIWndLayer::Init(HWND hwndParent, const RECT rc, int nAlpha)
{	
	Create(hwndParent, NULL, WS_POPUP | WS_DISABLED , WS_EX_WINDOWEDGE | WS_EX_LAYERED, rc);
	m_nAlphaValue = nAlpha;
}

LPCTSTR CUIWndLayer::GetWindowClassName() const
{
	return _T("LayerWnd");
}


void CUIWndLayer::OnFinalMessage(HWND /*hWnd*/)
{
	//delete this;
}


LRESULT CUIWndLayer::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	if (uMsg == WM_CREATE)
	{
		::SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlphaValue, LWA_ALPHA);
	}
	else
	{
		bHandled = FALSE;
	}

	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}


#define WM_SHOW_LAYER  WM_USER + 567
/////////////////////////////////////////////////////////////////////////////
CSkinDialog::CSkinDialog(void)
{
	m_pRoot				= NULL;
	m_bPendingMask		= FALSE;
	m_pLayer			= NULL;
	m_pLableTitle		= NULL;
	m_pActiveContainer	= NULL;
	m_bIsPopup			= FALSE;
	m_pPM				= NULL;
}

CSkinDialog::~CSkinDialog(void)
{
	if (m_pLayer)
	{
		delete m_pLayer;
		m_pLayer = NULL;
	}
	if (m_pPM !=NULL)
	{
		delete m_pPM;
		m_pPM  =  NULL;
	}
}

LRESULT  CSkinDialog::OnCreate(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_pRoot = new CContainerUI ;
	m_pPM	= new CPaintManagerUI ;
	m_pPM->Init(m_hWnd);
	m_pPM->AttachDialog(m_pRoot);
	m_pPM->AddNotifier(this);
	CRect rect;
	GetClientRect(GetHWND(),&rect);
	m_pRoot->SetPos(rect);
	Initialization();

	CRect rectWindow;
	GetWindowRect(GetHWND(),&rectWindow);
	m_pLayer = new CUIWndLayer;
	if (m_pLayer)
	{
		m_pLayer->Init(m_hWnd, rectWindow, 200);
	}
	if (m_pLayer)
	{
		if (::IsWindow(m_pLayer->GetHWND()))
		{
			::SetWindowPos(m_pLayer->GetHWND(),GetHWND(), rectWindow.left,rectWindow.top,rect.GetWidth(),rect.GetHeight(), SWP_NOACTIVATE);
		}
	}

	bHandled = FALSE;
	return S_OK;
}

void CSkinDialog::SetTitleIcon(UINT nID)
{
	if (!m_pLableTitle)
	{
		m_pLableTitle = RegiserLabel(0,CRect(8,8,100,40));
	}
	m_pLableTitle->SetBkImage(RES(nID));
	const TImageInfo * pImage = m_pPM->GetImageEx(RES(nID),_T("PNG"));
	if (pImage)
	{
		m_pLableTitle->SetFixedWidth(pImage->nX);
		m_pLableTitle->SetFixedHeight(pImage->nY);
	}
}

LRESULT  CSkinDialog::OnDestroy(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	Close();
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnNcActivate(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_bIsPopup)
	{
		bHandled = TRUE;
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}
	return 0;
}

LRESULT  CSkinDialog::OnNcCalcSize(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_bIsPopup)
	{
		bHandled = TRUE;
		NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;

		int nBorder = GetSystemMetrics(SM_CXFRAME);
		int nHeight= GetSystemMetrics(SM_CYMAXIMIZED)-2*nBorder;

		if (IsZoomed(GetHWND()))
		{
			lpncsp->rgrc[0].top = 0;//GetSystemMetrics(SM_CXFIXEDFRAME)+ GetSystemMetrics(SM_CYBORDER);
			lpncsp->rgrc[0].bottom = nHeight;
		}
	}
	return 0;
}

LRESULT  CSkinDialog::OnNcPaint(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_bIsPopup)
	{
	//	bHandled = TRUE;
	}
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnSize(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_bIsPopup)
	{
		CRect rect;
		GetWindowRect(GetHWND(),&rect);

		HRGN hRgn = CreateRectRgn(0,0,rect.GetWidth(),rect.GetHeight());
		SetWindowRgn(GetHWND(),hRgn,TRUE);
		DeleteObject(hRgn);
	}
	
	return S_OK;
}

LRESULT  CSkinDialog::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnNCLButtonDown(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT CSkinDialog::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT CSkinDialog::OnSystemCmd(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnUserMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

LRESULT  CSkinDialog::OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return S_OK;
}

CControlUI * CSkinDialog::FindControl(UINT nID)
{
	return m_pPM->FindControl(nID);
}

void CSkinDialog::SetStretchMode(CControlUI* pControl, UINT uMode)
{
	if(m_pRoot)
	{
		//m_pRoot->SetStretchMode(pControl, uMode);
		//pControl->SetStretchPos(pControl->GetPos());
	}
}

LRESULT CSkinDialog::MessageHandler(LPMSG lpMsg, BOOL& bHandled)
{
	return FALSE;
}

LRESULT CSkinDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = FALSE;
	switch( uMsg ) {
					case WM_CREATE:        lRes = OnCreate(wParam, lParam, bHandled); break;
					case WM_DESTROY:       lRes = OnDestroy(wParam, lParam, bHandled); break;
					case WM_NCACTIVATE:    lRes = OnNcActivate(wParam, lParam, bHandled); break;
					case WM_NCCALCSIZE:    lRes = OnNcCalcSize(wParam, lParam, bHandled); break;
					case WM_NCPAINT:       lRes = OnNcPaint(wParam, lParam, bHandled); break;
					case WM_NCHITTEST:     lRes = OnNcHitTest(wParam, lParam, bHandled); break;
					case WM_SIZE:          lRes = OnSize(wParam, lParam, bHandled); break;
					case WM_MOUSEMOVE:     lRes = OnMouseMove(wParam, lParam, bHandled); break;
					case WM_LBUTTONDOWN:   lRes = OnLButtonDown(wParam, lParam, bHandled); break;
					case WM_LBUTTONUP:     lRes = OnLButtonUp(wParam, lParam, bHandled); break;
					case WM_NCLBUTTONDOWN: lRes = OnNCLButtonDown(wParam, lParam, bHandled); break;
					case WM_SETCURSOR:	   lRes = OnSetCursor(wParam, lParam, bHandled); break;
					case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(wParam, lParam, bHandled); break;
					case WM_SYSCOMMAND:	   lRes = OnSystemCmd(wParam, lParam, bHandled); break;
					case WM_SHOW_LAYER:	   lRes = OnShowLayer(wParam, lParam, bHandled); break;
	}

	if(uMsg == WM_ENABLE)
	{
		::SendMessage(m_hWnd, WM_SHOW_LAYER, NULL, NULL);
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == ID_PENDINGMAKS_TIMER)
		{
			if (m_bPendingMask)
			{
				MakeMask();
			}
			else
			{
				KillTimer(GetHWND(),ID_PENDINGMAKS_TIMER);
			}
		}
	}


	if (uMsg > WM_USER && !bHandled)
	{
		lRes =  OnUserMessage(uMsg, wParam,  lParam,  bHandled);
	}
	else if (!bHandled)
	{
		lRes =  OnWindowMessage(uMsg, wParam,  lParam,  bHandled);
	}
	if(bHandled||(m_pPM&& m_pPM->MessageHandler(uMsg, wParam, lParam, lRes))) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CSkinDialog::SetSkinImage(LPCTSTR sImage)
{
	m_pRoot->SetBkImage(sImage);
}

void CSkinDialog::SetSkinImage(UINT nImage)
{
	m_pRoot->SetBkImage(RES(nImage));
}

void CSkinDialog::SetSkinColor(COLORREF col)
{
	m_pRoot->SetBkColor(col);
}

void CSkinDialog::SetSkinCorner(RECT rc)
{
	CString strInfo;strInfo.Format(_T("corner='%d,%d,%d,%d'"),rc.left,rc.top,rc.right,rc.right);
	m_pRoot->SetBkImageModify(strInfo);
}

CButtonUI*		CSkinDialog::RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CButtonUI * pControl = new CButtonUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
			
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		CString sName;
		sName.Format(_T("Button:%d"),nId);
		pControl->SetName(sName);

		return pControl;
	}
	return NULL;
}

CLabelUI*	    CSkinDialog::RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CLabelUI * pControl = new CLabelUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CEditUI*	CSkinDialog::RegiserEdit(UINT nId, RECT rect, LPCTSTR lpszText )
{
	CEditUI * pControl = new CEditUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CProgressUI*	CSkinDialog::RegiserProgress(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CProgressUI * pControl = new CProgressUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CSliderUI*		CSkinDialog::RegiserSliderCtrl(UINT nId, RECT rect, LPCTSTR lpszText )
{
	CSliderUI * pControl = new CSliderUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);

		m_pPM->InitControls(pControl,m_pRoot);
		return pControl;
	}
	return NULL;
}

CListUI*		CSkinDialog::RegiserList(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CListUI * pControl = new CListUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CScrollBarUI*	CSkinDialog::RegiserVScrollBar(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CScrollBarUI * pControl = new CScrollBarUI;
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CRichEditUI*	CSkinDialog::RegiserRichEdit(UINT nId, RECT rect, LPCTSTR lpszText,DWORD dStyle)
{
	CRichEditUI * pControl = new CRichEditUI;
	if (pControl)
	{
		pControl->SetWinStyle(dStyle);
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CControlUI*	CSkinDialog::RegiserControl(CControlUI*pControl, UINT nId, RECT rect, LPCTSTR lpszText)
{
	if (pControl)
	{
		if (m_pActiveContainer)
		{
			m_pActiveContainer->Add(pControl);
		}
		else
		{
			m_pRoot->Add(pControl);
		}
		pControl->SetFloat();
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

void CSkinDialog::SetActiveContainer(CContainerUI *pContainer)
{
	m_pActiveContainer = pContainer;
}

DWORD CSkinDialog::DefaultAction(DWORD dParam)
{
	return 0;
}

void CSkinDialog::BeforeCreate(DWORD dParam )
{

}
void CSkinDialog::MakeMask()
{
	return;

	BOOL bEnabled = ::IsWindowEnabled(m_hWnd);

	// 如果是Disabled时，先要检查是否还是当前窗体，如果还是当前窗体，延后蒙版操作
	if ((!bEnabled) && (GetForegroundWindow() == m_hWnd))
	{
		m_bPendingMask = TRUE;

		SetTimer(GetHWND(),ID_PENDINGMAKS_TIMER, 10, NULL);

		return;
	}

	m_bPendingMask = FALSE;

	if ((!bEnabled) && (::IsWindowVisible(m_hWnd)) )
	{
		if (m_pLayer)
		{

			CRect rectClient;
			GetWindowRect(GetHWND(),&rectClient);
			if (::IsWindow(m_pLayer->GetHWND()))
			{
				HWND hTopWnd = GetForegroundWindow();
				::SetWindowPos(m_pLayer->GetHWND(),GetHWND(),rectClient.left, rectClient.top, rectClient.GetWidth(), rectClient.GetHeight(),0);
				//::MoveWindow(m_pLayer->GetHWND(), rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), FALSE);
				m_pLayer->ShowWindow(TRUE, FALSE);
				if ((hTopWnd != NULL) && (::IsWindow(hTopWnd)) && (hTopWnd != m_hWnd))
				{
					SetForegroundWindow(hTopWnd);
				}
			}
		}
	}
	else
	{
		if (m_pLayer)
		{
			m_pLayer->ShowWindow(FALSE);;
		}
	}
}

LRESULT CSkinDialog::OnShowLayer( WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bPendingMask = FALSE;
	KillTimer(GetHWND(),ID_PENDINGMAKS_TIMER);

	MakeMask();

	return 0;
}
UINT CSkinDialog::ShowModal(HWND hParentWindow /* = ::GetActiveWindow */)
{
	m_bIsPopup = TRUE;
	if (!m_hWnd)
	{
		Create(hParentWindow,GetWindowClassName(),UI_WNDSTYLE_SKINDLG_CAP,0,0,0,0,0,0);
	}
	ASSERT(::IsWindow(m_hWnd));
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	SetForegroundWindow(m_hWnd);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}
		if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
	return TRUE;
}