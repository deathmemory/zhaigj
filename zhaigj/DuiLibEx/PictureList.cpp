#include "StdAfx.h"
#include "PictureList.h"
//...................................................
//....................................................
CPicListItemUI::CPicListItemUI()
{
	m_nStatueImage	 = _INVALID_RESOURCE_ID;

	m_nPicIamge		 = _INVALID_RESOURCE_ID;

	m_uButtonState	 = 0;

	m_TextColNormal  = _INVALID_COLOR;
	m_TextColHot	 = _INVALID_COLOR;
	m_TextColPressed = _INVALID_COLOR;
	m_TextColDisable = _INVALID_COLOR;
	m_uTextStyle				 = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	::ZeroMemory(&m_rcPicPadding, sizeof(m_rcPicPadding));
	m_bChildBtnEnable = FALSE;
	m_uChildBtnState = 0;
	m_bChildBtnVisable = FALSE;
	m_pOwner = NULL;
}

LPCTSTR CPicListItemUI::GetClass() const
{
	return _T("CPicListItemUI");
}
LPVOID CPicListItemUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CPicListItemUI")) == 0 ) return static_cast<CPicListItemUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}
UINT CPicListItemUI::GetControlFlags() const
{
	if( IsEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

BOOL CPicListItemUI::Activate()
{
	if( !CControlUI::Activate() ) return FALSE;

	TEventUI event = { 0 };
	event.dwTimestamp = ::GetTickCount();
	event.Type = UIEVENT_CHILDCLICK;
	event.pSender = this;
	if (GetParent())
	{
		GetParent()->DoEvent(event);
	}
	return TRUE;
}

void CPicListItemUI::SetEnabled(BOOL bEnable)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		m_uButtonState = 0;
	}
}
void  CPicListItemUI::SetCheck(BOOL bCheck)
{
	if (bCheck) m_uButtonState	|= UISTATE_PUSHED | UISTATE_CAPTURED;
	else    m_uButtonState		&= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	CControlUI::SetCheck(bCheck);
}

void CPicListItemUI::SendSelToParent()
{
	TEventUI eventParent = {0};
	eventParent.Type = UIEVENT_SETCHECKED;
	eventParent.pSender = this;
	CControlUI * pParent = GetParent();
	if (pParent)
	{
		pParent->DoEvent(eventParent);
	}
}

void CPicListItemUI::DoEvent(TEventUI& event)
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
	if ( event.Type == UIEVENT_RBUTTONUP)
	{
		if(m_pManager)m_pManager->SendNotify(this,_T("pic_item_r_button_up"),(WPARAM)event.pSender,event.pSender->GetTag());
	}
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        }
		BOOL bInBtn = FALSE;
		if (m_bChildBtnEnable&&m_bChildBtnVisable)
		{
			CRect rcBtn = m_rcChildBtn;
			OffsetRect(&rcBtn,m_rcItem.left,m_rcItem.top);
			if( ::PtInRect(&rcBtn, event.ptMouse) && IsEnabled() ) {
				m_uChildBtnState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
				bInBtn = TRUE;
			}
		}
		if (event.Type == UIEVENT_DBLCLICK && !bInBtn)
		{
			if(m_pManager)m_pManager->SendNotify(this,_T("pic_item_double_click"),(WPARAM)event.pSender,event.pSender->GetTag());
		}
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {

		//if (m_pOwner)
		//{
		//	CScrollBarUI * pVert = m_pOwner->GetVerticalScrollBar();
		//	if (pVert&&pVert->IsVisible())
		//	{
		//		m_pOwner->SetFocus();
		//	}
		//}
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
            else m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
		if (m_bChildBtnEnable&&m_bChildBtnVisable)
		{
			CRect rcBtn = m_rcChildBtn;
			OffsetRect(&rcBtn,m_rcItem.left,m_rcItem.top);
			if( (m_uChildBtnState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&rcBtn, event.ptMouse) ) m_uChildBtnState |= UISTATE_PUSHED;
				else m_uChildBtnState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			else
			{
				if( ::PtInRect(&rcBtn, event.ptMouse) )
				{
					m_uChildBtnState |= UISTATE_HOT;
					Invalidate();
				}
				else
				{
					m_uChildBtnState &= ~UISTATE_HOT;
					Invalidate();
				}
			}
		}
		if (m_bMouseMoveNotify&&::PtInRect(&m_rcItem,event.ptMouse))
		{
			m_bMouseMoved = TRUE;
			if(m_pManager)m_pManager->SendNotify(this,_T("pic_mousemove"),(WPARAM)event.pSender,event.pSender->GetTag());
		}
		if (m_bChildBtnEnable)
		{
			if (!m_bChildBtnVisable)
			{
				m_bChildBtnVisable = TRUE;
				Invalidate();
			}
		}
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();	
           // m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			SetCheck(TRUE);

			SendSelToParent();
        }
		if (m_bChildBtnEnable&&m_bChildBtnVisable)
		{
			CRect rcBtn = m_rcChildBtn;
			OffsetRect(&rcBtn,m_rcItem.left,m_rcItem.top);
			if( (m_uChildBtnState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&rcBtn, event.ptMouse) )	
				if(m_pManager)m_pManager->SendNotify(this,_T("pic_item_child_button"),(WPARAM)event.pSender,event.pSender->GetTag());
				m_uChildBtnState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			}
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
			m_uButtonState &= ~UISTATE_CAPTURED;
			
            Invalidate();
        }
		if (!IsChecked())
		{
			m_uButtonState &= ~UISTATE_PUSHED;
		}

		if (m_bMouseMoveNotify&&m_bMouseMoved)
		{
			m_bMouseMoved = FALSE;
			if(m_pManager)m_pManager->SendNotify(this,_T("pic_mouseleave"),(WPARAM)event.pSender,event.pSender->GetTag());
		}
		if (m_bChildBtnEnable)
		{
			if (m_bChildBtnVisable)
			{
				m_bChildBtnVisable = FALSE;
				Invalidate();
			}
		}
        return;
    }
    if( event.Type == UIEVENT_SETCURSOR ) {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
        return;
    }

	if( m_pOwner != NULL ) m_pOwner->DoEvent(event); 
	return;
}

void CPicListItemUI::SetOwner(CVerticalLayoutUI * pOwner)
{
	m_pOwner = pOwner;
}

void CPicListItemUI::SetStatueImage(UINT nStatueImage)
{
	m_nStatueImage = nStatueImage;
}
void CPicListItemUI::SetStatueImage(LPCTSTR strStatueImage)
{
	m_strStatueImage = strStatueImage;
}

void CPicListItemUI::SetPicImage(UINT nPicIamge)
{
	m_nPicIamge = nPicIamge;
}
void CPicListItemUI::SetPicImage(LPCTSTR strPicIamge)
{
	m_strPicImage = strPicIamge;
}

void CPicListItemUI::SetTextColor(COLORREF nor,COLORREF hot,COLORREF pressed,COLORREF disable)
{
	m_TextColNormal  = nor;
	m_TextColHot	 = hot;
	m_TextColPressed = pressed;
	m_TextColDisable = disable;
}

void CPicListItemUI::SetChildBtnEnable(BOOL bEn)
{
	m_bChildBtnEnable = bEn;
}

void CPicListItemUI::SetChildImage(UINT nID)
{
	m_nChildBtnImage = nID;
}

void CPicListItemUI::SetChildRect(RECT rc)
{
	m_rcChildBtn = rc;
}

void CPicListItemUI::PaintChildImage(HDC hDC)
{
	if( IsFocused() ) m_uChildBtnState |= UISTATE_FOCUSED;
	else m_uChildBtnState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uChildBtnState |= UISTATE_DISABLED;
	else m_uChildBtnState &= ~ UISTATE_DISABLED;

	CRect rcItem = GetPos();
	CRect rcBtn	 = m_rcChildBtn;
	//OffsetRect(&rcBtn,rcItem.left,rcItem.top);
	CString strDest;strDest.Format(_T("dest='%d,%d,%d,%d'"),rcBtn.left,rcBtn.top,rcBtn.right,rcBtn.bottom);
	if( (m_uChildBtnState & UISTATE_DISABLED) != 0 ) {
		CString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)strDest,75,0,100,100);
		if (m_nChildBtnImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nChildBtnImage),(LPCTSTR)strModify) ) m_nChildBtnImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uChildBtnState & UISTATE_PUSHED) != 0 ) {
		CString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)strDest,50,0,75,100);
		if (m_nChildBtnImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nChildBtnImage),(LPCTSTR)strModify) ) m_nChildBtnImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uChildBtnState & UISTATE_HOT) != 0 ) {
		CString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)strDest,25,0,50,100);
		if (m_nChildBtnImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nChildBtnImage),(LPCTSTR)strModify) ) m_nChildBtnImage=0;
			//else goto PICTURE;
		}
	}
	else
	{
		CString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),(LPCTSTR)strDest,0,0,25,100);
		if (m_nChildBtnImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nChildBtnImage),(LPCTSTR)strModify) ) m_nChildBtnImage=0;
			//else goto PICTURE;
		}
	}

}

void CPicListItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{

}

void CPicListItemUI::PaintText(HDC hDC)
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

void CPicListItemUI::SetPicImagePadding(RECT rc)
{
	m_rcPicPadding = rc;
}

void CPicListItemUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),75,0,100,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nStatueImage),(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),50,0,75,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nStatueImage),(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),25,0,50,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nStatueImage),(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),0,0,25,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nStatueImage),(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}
	else
	{
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),0,0,25,100);
		if( !m_strStatueImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strStatueImage) ) m_strStatueImage.Empty();
			//else goto PICTURE;
		}
		else if (m_nStatueImage!=0)
		{
			if( !DrawImage(hDC, RES(m_nStatueImage),(LPCTSTR)strModify) ) m_nStatueImage=0;
			//else goto PICTURE;
		}
	}

	{
		RECT rc = m_rcItem;
		rc.left		+= m_rcPicPadding.left;
		rc.right	-= m_rcPicPadding.right;
		rc.top		+= m_rcPicPadding.top;
		rc.bottom	-= m_rcPicPadding.bottom;
		UINT uDestHeight = m_rcItem.bottom - m_rcItem.top  - m_rcPicPadding.bottom;
		UINT uDestWidth = m_rcItem.right - m_rcItem.left - m_rcPicPadding.right;
		CString strM;strM.Format(_T("dest='%d,%d,%d,%d'"),m_rcPicPadding.left,m_rcPicPadding.top,uDestWidth,uDestHeight);
		if( !m_strPicImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_strPicImage,(LPCTSTR)strM) ) m_strPicImage.Empty();
		}
		else if (m_nPicIamge!=0)
		{
			CString strModify;
			//if (!m_sResType.IsEmpty())
			//{
			//	strModify.Format(_T("restype='%s'"),m_sResType);
			//}
			//strModify+=strM;
			if( !DrawImage(hDC, RES(m_nPicIamge),(LPCTSTR)strModify) ) m_nPicIamge=0;
		}
	}
	if (m_bChildBtnEnable&&m_bChildBtnVisable)
	{
		PaintChildImage(hDC);
	}
};

//...................................................//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT CPictureList::m_ExpendGroup[MAX_INDEX] = {5,5,5,6,7,8,10,12,16,20,24,26,30,35,40,45,40,38,36,34,32,30,28,26,24,20,16,12,10,10};
CPictureList::CPictureList(void)
{
	m_nCol = 0;
	m_bAutoCol = FALSE;
	m_uExpendHeight = 0;
	m_bExpend  = TRUE;
	m_nCurIndex= MAX_INDEX;
	ZeroMemory(&m_sItemGap,sizeof(m_sItemGap));
}

CPictureList::~CPictureList(void)
{
}
LPCTSTR CPictureList::GetClass() const
{
	return _T("CPictureList");
}

LPVOID CPictureList::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CPictureList")) == 0 ) return static_cast<CVerticalLayoutUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

UINT CPictureList::GetControlFlags() const
{
	if( IsEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

void CPictureList::SetPos(RECT rc)
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
	
	//CalculateItemPos()
	
	// Process the scrollbar
	ProcessScrollBar(rc, 0, CalculateItemPos(rc)); //¼ÆËãItemÎ»ÖÃ
}

void CPictureList::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
}

void CPictureList::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("TRUE")) == 0);
	else CContainerUI::SetAttribute(pstrName, pstrValue);
}

BOOL CPictureList::Expend(BOOL bExpend)
{
	m_bExpend = bExpend;
	if (m_bExpend)
	{
		GetManager()->KillTimer(this);
		GetManager()->SetTimer(this, ID_TIMER_EXPEND, 10);
	}
	else
	{
		GetManager()->KillTimer(this);
		GetManager()->SetTimer(this, ID_TIMER_UNEXPEND, 10);
	}
	EnableScrollBar(FALSE,FALSE);
	return TRUE;
}

BOOL CPictureList::IsExpend()
{
	return m_bExpend;
}

void CPictureList::SetMaxExpendHeight(UINT uExpendHeight)
{
	m_uExpendHeight = uExpendHeight;
}

void CPictureList::DoEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_SETCHECKED && IsEnabled() )
	{
		for (int i = 0 ; i<m_items.GetSize() ; i++)
		{
			CControlUI * pControl =(CControlUI * )m_items.GetAt(i);
			if (pControl&&pControl != event.pSender)
			{
				pControl->SetCheck(FALSE);
			}
			else if (pControl)
			{
				if(m_pManager)m_pManager->SendNotify(this,_T("picitem_click"),(WPARAM)pControl,(LPARAM)pControl->GetTag());
			}
		}
	}
	if( event.Type == UIEVENT_MOUSEMOVE)
	{

	}
	if( event.Type == UIEVENT_TIMER && IsEnabled() )
	{
		UINT nTotlePersent = 0;
		UINT nCurPersent = 0;
		if (event.wParam == ID_TIMER_EXPEND)
		{
			m_nCurIndex ++;
			if (m_nCurIndex>=MAX_INDEX)
			{
				m_nCurIndex = MAX_INDEX;
				GetManager()->KillTimer(this);
				EnableScrollBar(TRUE,FALSE);
			}
		}
		else if(event.wParam == ID_TIMER_UNEXPEND)
		{
			m_nCurIndex --;
			if (m_nCurIndex<0)
			{
				m_nCurIndex = 0;
				GetManager()->KillTimer(this);
			}
		}
		for (int i = 0 ; i < MAX_INDEX ; i++){
			if (i<m_nCurIndex)nCurPersent += CPictureList::m_ExpendGroup[i];
			nTotlePersent += CPictureList::m_ExpendGroup[i];
		}
		UINT nCurHeight = (nCurPersent*GetMaxHeight())/nTotlePersent;
		RECT rc = GetPos();
		rc.bottom = rc.top + nCurHeight;
		SetPos(rc);
		if( CControlUI::Activate() )
		if( m_pManager != NULL ) 
			m_pManager->SendNotify(this, _T("piclist_expending"));
	}
	CContainerUI::DoEvent(event);
}

void CPictureList::InsertGroup(UINT nID,LPCTSTR pstrGroupName)
{
	
}

CPicListItemUI* CPictureList::InsertItem(LPCTSTR ItemName)
{
	CPicListItemUI* pItem = new CPicListItemUI;
	if (pItem)
	{
		pItem->SetOwner(this);
		pItem->SetText(ItemName);
		pItem->SetID(m_items.GetSize());
		Add(pItem);
		SetPos(GetPos());
		return pItem;
	}
	return NULL;
}


void CPictureList::SetCol(UINT nCol)
{
	m_nCol = nCol;
}
void CPictureList::SetAutoCol(BOOL bAuto)
{
	m_bAutoCol = bAuto;
}

void CPictureList::SetItemGap(SIZE sGap)
{
	m_sItemGap.cx = sGap.cx;
	m_sItemGap.cy = sGap.cy;
}

void CPictureList::SetItemRect(RECT rc)
{
	m_rcPicItem = rc;
}

void CPictureList::SetSel(int nID)
{
	if (nID<GetCount()&&nID>=0)
	{
		CPicListItemUI * pCon = dynamic_cast<CPicListItemUI *>(GetItemAt(nID));
		if (pCon)
		{
			pCon->SetCheck(TRUE);
			pCon->SendSelToParent();

			CScrollBarUI * pVert = GetVerticalScrollBar();
			if (pVert &&pVert->IsVisible())
			{
				CRect rcParent = GetPos();
				CRect rcItem = pCon->GetPos();
				int nCur = pVert->GetScrollPos();
				if (rcItem.bottom>rcParent.bottom)
				{
					SetScrollPos(CSize(0,nCur + (rcItem.bottom-rcParent.bottom)));
				}
				if (rcItem.top<rcParent.top)
				{
					SetScrollPos(CSize(0,nCur + (rcItem.top-rcParent.top)));
				}
			}
		}
	}
}
void CPictureList::SetSelSilent(int nID)
{
	if (nID<GetCount()&&nID>=0)
	{
		CPicListItemUI * pCon = dynamic_cast<CPicListItemUI *>(GetItemAt(nID));
		if (pCon)
		{
			pCon->SetCheck(TRUE);
			for (int i = 0 ; i<m_items.GetSize() ; i++)
			{
				CControlUI * pControl =(CControlUI * )m_items.GetAt(i);
				if (pControl&&pControl != pCon)
				{
					pControl->SetCheck(FALSE);
				}
			}

			CScrollBarUI * pVert = GetVerticalScrollBar();
			if (pVert &&pVert->IsVisible())
			{
				CRect rcParent = GetPos();
				CRect rcItem = pCon->GetPos();
				int nCur = pVert->GetScrollPos();
				if (rcItem.bottom>rcParent.bottom)
				{
					SetScrollPos(CSize(0,nCur + (rcItem.bottom-rcParent.bottom)));
				}
				if (rcItem.top<rcParent.top)
				{
					SetScrollPos(CSize(0,nCur + (rcItem.top-rcParent.top)));
				}
			}
		}
	}
}

int CPictureList::GetSel()
{
	for (int i = 0 ; i< GetCount() ; i++)
	{
		CControlUI * pCon = GetItemAt(i);
		if (pCon)
		{
			if (pCon->IsChecked())
			{
				return i;
			}
		}
	}
	return -1;
}

UINT CPictureList::CalculateItemPos(RECT rc)
{
	UINT nCyRequird = 0;
	if (m_items.GetSize() == 0)return nCyRequird ;

	for (int i = 0 ; i<m_items.GetSize() ; i++)
	{
		UINT nItemWidth = m_rcPicItem.right - m_rcPicItem.left;
		UINT nItemHeight = m_rcPicItem.bottom - m_rcPicItem.top;
		if (m_bAutoCol)
		{
			m_nCol = (rc.right - rc.left)/(nItemWidth + m_sItemGap.cx);
		}
		UINT nCol = m_nCol;
		if (nCol==0)return nCyRequird; 

		CControlUI *pControl  =(CControlUI * )m_items.GetAt(i);
		if (pControl)
		{
			UINT nCurCol = i%nCol;
			UINT nCurRow = i/nCol;
			INT  nTopOffset = 0;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) nTopOffset = m_pVerticalScrollBar->GetScrollPos();
			RECT rc ;
			rc.left = nCurCol*(nItemWidth+m_sItemGap.cx) + m_rcItem.left+m_rcInset.left;
			rc.right = rc.left + nItemWidth;

			rc.top = nCurRow*(nItemHeight+m_sItemGap.cy)+ m_rcItem.top+m_rcInset.top - nTopOffset;
			rc.bottom = rc.top + nItemHeight;
			pControl->SetPos(rc);
			nCyRequird = nCurRow*(nItemHeight+m_sItemGap.cy) + nItemHeight;
		}
	}
	return nCyRequird;
}