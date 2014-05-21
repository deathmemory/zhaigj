#include "StdAfx.h"
#include "TabButtonListUI.h"


CTabButtonItemUI::CTabButtonItemUI()
{
	m_nStatueImage	 = _INVALID_RESOURCE_ID;

	m_uButtonState	 = 0;

	m_TextColNormal  = _INVALID_COLOR;
	m_TextColHot	 = _INVALID_COLOR;
	m_TextColPressed = _INVALID_COLOR;
	m_TextColDisable = _INVALID_COLOR;
}

LPCTSTR CTabButtonItemUI::GetClass() const
{
	return _T("CPicListItemUI");
}
LPVOID CTabButtonItemUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CPicListItemUI")) == 0 ) return static_cast<CTabButtonItemUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}
UINT CTabButtonItemUI::GetControlFlags() const
{
	if( IsEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

BOOL CTabButtonItemUI::Activate()
{
	if( !CControlUI::Activate() ) return FALSE;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("picitem_click"));
	return TRUE;
}

void CTabButtonItemUI::SetEnabled(BOOL bEnable)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		m_uButtonState = 0;
	}
}

void CTabButtonItemUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CLabelUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_KEYDOWN )
	{
		if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
			Activate();
			return;
		}
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
			m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
			else m_uButtonState &= ~UISTATE_PUSHED;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
			m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		if( IsContextMenuUsed() ) {
			m_pManager->SendNotify(this, _T("menu"), event.wParam, event.lParam);
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_SETCURSOR ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		CControlUI * pParent = GetParent();
		if (pParent)
		{
			pParent->DoEvent(event);
		}
	}
}

void CTabButtonItemUI::SetStatueImage(UINT nStatueImage)
{
	m_nStatueImage = nStatueImage;
}
void CTabButtonItemUI::SetStatueImage(LPCTSTR strStatueImage)
{
	m_strStatueImage = strStatueImage;
}

void CTabButtonItemUI::SetTextColor(COLORREF nor,COLORREF hot,COLORREF pressed,COLORREF disable)
{
	m_TextColNormal  = nor;
	m_TextColHot	 = hot;
	m_TextColPressed = pressed;
	m_TextColDisable = disable;
}

void CTabButtonItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{

}
void CTabButtonItemUI::PaintText(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

	if( m_sText.IsEmpty() ) return;
	int nLinks = 0;
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	DWORD clrColor = IsEnabled()?m_TextColNormal:m_TextColDisable;

	if( ((m_uButtonState & UISTATE_PUSHED) != 0) )
		clrColor = m_TextColPressed;
	else if( ((m_uButtonState & UISTATE_HOT) != 0)  )
		clrColor = m_TextColHot;
	else if( ((m_uButtonState & UISTATE_FOCUSED) != 0)  )
		clrColor = m_TextColNormal;

	if( m_bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor, \
		NULL, NULL, nLinks, m_uTextStyle);
	else
		CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor, \
		m_iFont, m_uTextStyle);
}

void CTabButtonItemUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		CStdString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sImageModify,75,0,100,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage,(LPCTSTR)m_sImageModify) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, m_nStatueImage,(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		CStdString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sImageModify,50,0,75,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage,(LPCTSTR)m_sImageModify) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, m_nStatueImage,(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		CStdString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sImageModify,25,0,50,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage,(LPCTSTR)m_sImageModify) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, m_nStatueImage,(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		CStdString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sImageModify,0,0,25,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage,(LPCTSTR)m_sImageModify) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, m_nStatueImage,(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	CStdString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)m_sImageModify,0,0,25,100);
	if( !m_strStatueImage.IsEmpty() ) {
		if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage,(LPCTSTR)m_sImageModify) ) m_strStatueImage.Empty();
		//else goto PICTURE;
	}
	else if (m_nStatueImage!=0)
	{
		if( !DrawImage(hDC, m_nStatueImage,(LPCTSTR)strModify) ) m_nStatueImage=0;
		//else goto PICTURE;
	}

	{
		RECT rc = m_rcItem;
		rc.left		+= m_rcPicPadding.left;
		rc.right	-= m_rcPicPadding.right;
		rc.top		+= m_rcPicPadding.top;
		rc.bottom	-= m_rcPicPadding.bottom;
		CStdString strM;strM.Format(_T("%s dest='%d,%d,%d,%d'"),(LPCTSTR)m_sImageModify,m_rcPicPadding.left,m_rcPicPadding.top,m_rcPicPadding.right,m_rcPicPadding.bottom);
		if( !m_strPicImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strPicImage,(LPCTSTR)strM) ) m_strPicImage.Empty();
			else return;
		}
		else if (m_nPicIamge!=0)
		{
			if( !DrawImage(hDC, m_nPicIamge,(LPCTSTR)strM) ) m_nPicIamge=0;
			else return;
		}
	}
};
//..................................................................................................................................
CTabButtonListUI::CTabButtonListUI(void)
{
}


CTabButtonListUI::~CTabButtonListUI(void)
{
}
LPCTSTR CTabButtonListUI::GetClass() const
{
	return _T("CTabButtonListUI");
}

LPVOID CTabButtonListUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CTabButtonListUI")) == 0 ) return static_cast<CTabButtonListUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

UINT CTabButtonListUI::GetControlFlags() const
{
	if( IsEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

void CTabButtonListUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	if( m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}



	// Process the scrollbar
	ProcessScrollBar(rc, 0, 0); //º∆À„ItemŒª÷√
}

void CTabButtonListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("TRUE")) == 0);
	else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CTabButtonListUI::DoEvent(TEventUI& event)
{
	CContainerUI::DoEvent(event);
}
