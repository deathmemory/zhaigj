#include "StdAfx.h"
#include "ChildWnd.h"

CChildWnd::CChildWnd(void)
{
}

CChildWnd::~CChildWnd(void)
{
}
BOOL CChildWnd::Initialization(HWND hParent,RECT rect)
{
	m_pm.Init(hParent,TRUE);
	m_pm.AttachDialog(this);
	m_pm.AddNotifier(this);
	SetPos(rect);
	m_hParent = hParent;
	InitChildWindow();
	return TRUE;
}

CChildWnd*	CChildWnd::RegiserChild(CChildWnd *pChild,LPCTSTR strName ,RECT rect)
{
	if (pChild)
	{
		Add(pChild);
		pChild->Initialization(m_hParent,rect);
		m_ChildWnd.Insert(strName,(LPVOID)pChild);
	}
	return pChild;
}

LRESULT	CChildWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = S_OK;
	if (uMsg == WM_PAINT)
	{
		int i =0;
	}
	switch( uMsg ) {        
					case WM_LBUTTONDOWN:   
					case WM_LBUTTONUP:     
					case WM_MOUSEMOVE:  
					case WM_MOUSEHOVER:    
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
						{
							RECT rcPos = GetPos();
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							if (::PtInRect(&rcPos,pt))
							{
								m_pm.MessageHandler(uMsg,wParam,lParam,res);
							}
						}
						break;
					default:
						m_pm.MessageHandler(uMsg,wParam,lParam,res);
	}
	for (int i = 0 ; i < m_ChildWnd.GetSize();i++ )
	{
		CChildWnd * pChild = dynamic_cast<CChildWnd *>((CChildWnd *)m_ChildWnd.Find(m_ChildWnd.GetAt(i)));
		if (pChild)
		{
			pChild->HandleMessage(uMsg, wParam, lParam);
		}
	}
	return res;
}

void  CChildWnd::SetSkinImage(LPCTSTR sImage)
{
	SetBkImage(sImage);
}

void  CChildWnd::SetSkinImage( UINT nImage)
{
	SetBkImage(nImage);
}


CButtonUI*		CChildWnd::RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CButtonUI * pControl = new CButtonUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		CStdString sName;
		sName.Format(_T("Button:%d"),nId);
		pControl->SetName(sName);


		//m_pm.Add(pControl,m_pRoot);
		return pControl;
	}
	return NULL;
}

CLabelUI*	    CChildWnd::RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CLabelUI * pControl = new CLabelUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CRichEditUI*	CChildWnd::RegiserEdit(UINT nId, RECT rect, LPCTSTR lpszText )
{
	CRichEditUI * pControl = new CRichEditUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CProgressUI*	CChildWnd::RegiserProgress(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CProgressUI * pControl = new CProgressUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);

		return pControl;
	}
	return NULL;
}

CSliderUI*		CChildWnd::RegiserSliderCtrl(UINT nId, RECT rect, LPCTSTR lpszText )
{
	CSliderUI * pControl = new CSliderUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}

CListUI*	CChildWnd::RegiserList(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CListUI * pControl = new CListUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);

		return pControl;
	}
	return NULL;
}

CScrollBarUI*	CChildWnd::RegiserVScrollBar(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CScrollBarUI * pControl = new CScrollBarUI;
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}
CControlUI*	CChildWnd::RegiserControl(CControlUI*pControl, UINT nId, RECT rect, LPCTSTR lpszText)
{
	if (pControl)
	{
		Add(pControl);
		pControl->SetPos(rect);
		pControl->SetID(nId);
		pControl->SetText(lpszText);
		return pControl;
	}
	return NULL;
}