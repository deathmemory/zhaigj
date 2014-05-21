// DlgStartSet.cpp : implementation file
//

#include "stdafx.h"

#include "DlgMenu.h"


CMenuBtnUI::CMenuBtnUI()
{
	m_nIconID = 0;
	ZeroMemory(&m_rcIcon,sizeof(m_rcIcon));
}

LPCTSTR CMenuBtnUI::GetClass() const
{
	return _T("CMenuBtnUI");
}

LPVOID CMenuBtnUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CMenuBtnUI")) == 0 ) return static_cast<CMenuBtnUI*>(this);
	return CButtonUI::GetInterface(pstrName);
}

void CMenuBtnUI::PaintStatusImage(HDC hDC)
{
	CButtonUI::PaintStatusImage(hDC);

	UINT uTop = 0;
	UINT uBottom = 100;
	CString strDest;

	strDest.Format(_T("dest='%d,%d,%d,%d'"),m_rcIcon.left,m_rcIcon.top,m_rcIcon.right,m_rcIcon.bottom);
	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		CString strModify;strModify.Format(_T("%s %s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sIconModify,(LPCTSTR)strDest,75,uTop,100,uBottom);
		if (m_nIconID!=0)
		{
			if( !DrawImage(hDC, RES(m_nIconID),(LPCTSTR)strModify) ) m_nIconID=0;
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_PUSHED) != 0 || ((m_uButtonType == BUTTON_TYPE_CHECK)&&IsChecked() )){
		CString strModify;strModify.Format(_T("%s %s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sIconModify,(LPCTSTR)strDest,50,uTop,75,uBottom);
		if (m_nIconID!=0)
		{

			if( !DrawImage(hDC, RES(m_nIconID),(LPCTSTR)strModify) ) m_nIconID=0;
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		CString strModify;strModify.Format(_T("%s %s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sIconModify,(LPCTSTR)strDest,25,uTop,50,uBottom);
		if (m_nIconID!=0)
		{

			if( !DrawImage(hDC, RES(m_nIconID),(LPCTSTR)strModify) ) m_nIconID=0;
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		CString strModify;strModify.Format(_T("%s %s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sIconModify,(LPCTSTR)strDest,0,uTop,25,uBottom);
		if (m_nIconID!=0)
		{
			if( !DrawImage(hDC, RES(m_nIconID),(LPCTSTR)strModify) ) m_nIconID=0;
			else return;
		}
	}
	{
		CString strModify;strModify.Format(_T("%s %s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sIconModify,(LPCTSTR)strDest,0,uTop,25,uBottom);
		if (m_nIconID!=0)
		{	
			if( !DrawImage(hDC, RES(m_nIconID),(LPCTSTR)strModify) ) m_nIconID=0;
			else return;
		}
	}
}

void CMenuBtnUI::SetIconImage(UINT nID,RECT rcIcon,LPCTSTR strModify)
{
	m_sIconModify	= strModify;
	m_nIconID		= nID;
	m_rcIcon		= rcIcon;
}
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
CDlgMenu::CDlgMenu()
{
	m_nItemBKID  = 0;
	m_nItemWidth = 100;
	m_nSkinBK	 = 0;
	m_nID			= 0;
	ZeroMemory(&m_rcCorner,sizeof(m_rcCorner));
}

CDlgMenu::~CDlgMenu()
{
	vector<MenuItem*>::iterator it = m_ItemList.begin();
	for (;it!= m_ItemList.end();it++)
	{
		delete *it;
	}
}

void CDlgMenu::InitControls()
{
	CButtonUI *lpBtn=NULL;
	CLabelUI *lpLab=NULL;
}

void CDlgMenu::Notify( TNotifyUI& msg )
{
	if( msg.sType == _T("click"))
	{
		Close();
		UINT luSenderId=msg.pSender->GetID();;
		HWND hParent = GetParent(GetHWND());
		if (hParent)
		{
			::PostMessage(hParent,WM_MENU_CLICK,luSenderId,GetID());
		}
	}
}

void CDlgMenu::SetID(UINT nID)
{
	m_nID = nID;
}
UINT CDlgMenu::GetID()
{
	return m_nID;
}

void CDlgMenu::SetMenuIconRect(RECT rc)
{
	m_rcIcon = rc;
}
// CDlgStartSet message handlers

BOOL CDlgMenu::Initialization()
{
	SetSkinImage(m_nSkinBK);
	InitControls();
	InitItemList();
	SetSkinCorner(m_rcCorner);
	int nCount = m_ItemList.size();
	SetWindowPos(GetHWND(),NULL, 0, 0, m_nItemWidth + 4, nCount*30+4, SWP_NOZORDER|SWP_NOMOVE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMenu::AddItem(int nID,CString ItemText,int ImageheaderID)
{
	MenuItem* items = new MenuItem;
	items->nID = nID;
	items->sText = ItemText;
	items->HeadimageID = ImageheaderID;
	m_ItemList.push_back(items);
}

void CDlgMenu::SetSkin(UINT nBK)
{
	m_nSkinBK = nBK;
}

void CDlgMenu::SetCorner(RECT rc)
{
	m_rcCorner = rc;
}

void CDlgMenu::SetItemBkImage(UINT nID)
{
	m_nItemBKID = nID;
}

void CDlgMenu::SetItemWidth(UINT nWidth)
{
	 m_nItemWidth = nWidth;
}
void CDlgMenu::InitItemList()
{
	for (int i = 0;i < m_ItemList.size();i++)
	{
		MenuItem* items = m_ItemList[i];
		CMenuBtnUI *lpBtn = (CMenuBtnUI*)RegiserControl(CONTROL(CMenuBtnUI),items->nID,CRect(2,2+i*30,m_nItemWidth+2,32+i*30));
		if (lpBtn)
		{
			lpBtn->SetCorner(CRect(34,0,40,0));
			lpBtn->SetButtonImage(RES(m_nItemBKID));
			lpBtn->SetText(items->sText);
			lpBtn->SetTextStyle(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			lpBtn->SetTextPadding(CRect(35,0,0,0));
			lpBtn->SetTextColor(ARGB(255,255,255,255),ARGB(255,255,255,255),ARGB(255,255,255,255),ARGB(255,255,255,255));
			lpBtn->SetIconImage(items->HeadimageID,m_rcIcon);//);
		}
	}
}

void CDlgMenu::MenuPopup(HWND hWndParent,int nX,int nY)
{
	Create(hWndParent,_T("Menu"),WS_POPUP,WS_EX_TOPMOST,nX,nY);
	ASSERT(::IsWindow(m_hWnd));
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::SetFocus(m_hWnd);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
			//::SetFocus(hWndParent);
			//::ReleaseCapture();
		}
		if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_CLOSE ) break;
	}
	return ;
}      

LRESULT CDlgMenu::OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (uMsg == WM_KILLFOCUS)
	{
		Close();
	}
	return 0;
}