#include "StdAfx.h"
#include "PicHScrollUI.h"


INT CPicHScrollUI::m_ExpendGroup[MAX_INDEX] = {5,5,5,6,7,8,9,10,12,14,16,20,24,28,32,36,36,36,32,28,24,20,16,14,12,10,8,8,6,6};
CPicHScrollUI::CPicHScrollUI(void)
{
	m_nCurGroup = 0;
	m_nCol = 0;
	m_nCurIndex= 0;
	m_nCXoffset = 0;
	ZeroMemory(&m_sItemGap,sizeof(m_sItemGap));
}

CPicHScrollUI::~CPicHScrollUI(void)
{
}
LPCTSTR CPicHScrollUI::GetClass() const
{
	return _T("PicHScrollUI");
}

LPVOID CPicHScrollUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("PicHScrollUI")) == 0 ) return static_cast<CVerticalLayoutUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

UINT CPicHScrollUI::GetControlFlags() const
{
	if( IsEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

void CPicHScrollUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	rc = m_rcItem;

	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	if( m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}
	ProcessScrollBar(rc,CalculateItemPos(rc),  0); //¼ÆËãItemÎ»ÖÃ
}

void CPicHScrollUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
}

void CPicHScrollUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
}

int  CPicHScrollUI::GetCurGroup()
{
	return m_nCurGroup;
}

int  CPicHScrollUI::GetGroupCount()
{
	int nLose =  m_items.GetSize()%(m_nCol*m_nRow);
	int nGSize =  m_items.GetSize()/(m_nCol*m_nRow);

	if (nLose>0)
	{
		nGSize+=1;
	}
	return nGSize;
}

void CPicHScrollUI::SetHSrollPos(int hPos)
{
	int cx = 0;

	int iLastScrollPos = m_nCXoffset;
	m_nCXoffset = hPos;
	cx = m_nCXoffset - iLastScrollPos;

	if (cx==0)return;

	RECT rcPos;
	for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;

		rcPos = pControl->GetPos();
		rcPos.left -= cx;
		rcPos.right -= cx;
		pControl->SetPos(rcPos);
	}

	Invalidate();
}

void CPicHScrollUI::DoEvent(TEventUI& event)
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
	if( event.Type == UIEVENT_TIMER && IsEnabled() )
	{
		UINT nTotlePersent = 0;
		UINT nCurPersent = 0;
		if (event.wParam == ID_LEFT_TIMER)
		{
			m_nCurIndex ++;
			if (m_nCurIndex>=MAX_INDEX)
			{
				m_nCurIndex = MAX_INDEX;
				GetManager()->KillTimer(this);
			}
			for (int i = 0 ; i < MAX_INDEX ; i++){
				if (i<m_nCurIndex)nCurPersent += CPicHScrollUI::m_ExpendGroup[i];
				nTotlePersent += CPicHScrollUI::m_ExpendGroup[i];
			}
			if (event.wParam == ID_LEFT_TIMER)
			{
				UINT nItemWidth = m_rcPicItem.right - m_rcPicItem.left;
				UINT nItemHeight = m_rcPicItem.bottom - m_rcPicItem.top;
				int nGroupWidth = m_nCol*(nItemWidth+m_sItemGap.cx);

				UINT nCurWidth = ((nCurPersent)*nGroupWidth)/nTotlePersent;

				SetHSrollPos(m_nCurGroup*nGroupWidth-nCurWidth);
			}
			if (m_nCurIndex == MAX_INDEX)
			{
				m_nCurIndex = 0;
				int nGroup =GetGroupCount();
				if (m_nCurGroup>0)
				{
					m_nCurGroup--;
					if(m_pManager)m_pManager->SendNotify(this,_T("pic_hscroll_group_change"),(WPARAM)m_nCurGroup,(LPARAM)nGroup);
				}
			}
		}
		else if(event.wParam == ID_RIGHT_TIMER)
		{
			m_nCurIndex ++;
			if (m_nCurIndex>=MAX_INDEX)
			{
				m_nCurIndex =MAX_INDEX;
				GetManager()->KillTimer(this);
				
			}
			for (int i = 0 ; i < MAX_INDEX ; i++){
				if (i<m_nCurIndex)nCurPersent += CPicHScrollUI::m_ExpendGroup[i];
				nTotlePersent += CPicHScrollUI::m_ExpendGroup[i];
			}
			if(event.wParam == ID_RIGHT_TIMER)
			{
				UINT nItemWidth = m_rcPicItem.right - m_rcPicItem.left;
				UINT nItemHeight = m_rcPicItem.bottom - m_rcPicItem.top;
				int nGroupWidth = m_nCol*(nItemWidth+m_sItemGap.cx);

				UINT nCurWidth = (nCurPersent*nGroupWidth)/nTotlePersent;

				SetHSrollPos((m_nCurGroup)*nGroupWidth + nCurWidth);
			}
			if (m_nCurIndex == MAX_INDEX)
			{
				m_nCurIndex = 0;
				int nGroup = GetGroupCount();
				if (m_nCurGroup<nGroup)
				{
					m_nCurGroup++;
					if(m_pManager)m_pManager->SendNotify(this,_T("pic_hscroll_group_change"),(WPARAM)m_nCurGroup,(LPARAM)nGroup);
				}
			}
		}
		
	}
	CContainerUI::DoEvent(event);
}

CPicListItemUI* CPicHScrollUI::InsertItem(LPCTSTR ItemName)
{
	CPicListItemUI* pItem = new CPicListItemUI;
	if (pItem)
	{
		pItem->SetText(ItemName);
		pItem->SetID(m_items.GetSize());
		Add(pItem);
		SetPos(GetPos());
		return pItem;
	}
	return NULL;
}

void CPicHScrollUI::RemoveAll()
{
	CContainerUI::RemoveAll();
	m_nCurGroup = 0;
	m_nCXoffset = 0;
}

void CPicHScrollUI::SetColRow(UINT nCol,UINT nRow)
{
	m_nCol = nCol;
	m_nRow = nRow;
}

void CPicHScrollUI::SetItemGap(SIZE sGap)
{
	m_sItemGap.cx = sGap.cx;
	m_sItemGap.cy = sGap.cy;
}

void CPicHScrollUI::SetItemRect(RECT rc)
{
	m_rcPicItem = rc;
}

UINT CPicHScrollUI::CalculateItemPos(RECT rc)
{
	UINT nCXRequird = 0;
	if (m_items.GetSize() == 0)return nCXRequird ;

	int nGroups = GetGroupCount();
	if (m_nCurGroup>=nGroups &&nGroups!=0)
	{
		m_nCurGroup = nGroups-1;
	}
	for (int i = 0 ; i<m_items.GetSize() ; i++)
	{
		UINT nItemWidth = m_rcPicItem.right - m_rcPicItem.left;
		UINT nItemHeight = m_rcPicItem.bottom - m_rcPicItem.top;

		UINT nCol = m_nCol;
		UINT nRow = m_nRow;

		int nOneGroup = nCol*nRow;
		int nCurGroup = i/nOneGroup;

		int nGroupWidth = m_nCol*(nItemWidth+m_sItemGap.cx);

		if (nCol==0)return nCXRequird; 

		CControlUI *pControl  =(CControlUI * )m_items.GetAt(i);
		if (pControl)
		{
			int nGroupIndex = i-nCurGroup*nOneGroup;
			UINT nCurCol = nGroupIndex%nCol;
			UINT nCurRow = nGroupIndex/nCol;
			INT  nLeftOffset = 0;
			if( m_pHorizontalScrollBar ) nLeftOffset = m_pHorizontalScrollBar->GetScrollPos();
			RECT rc ;
			rc.left = nCurCol*(nItemWidth+m_sItemGap.cx) + m_rcItem.left+m_rcInset.left + nGroupWidth*nCurGroup;
			rc.right = rc.left + nItemWidth;

			rc.top = nCurRow*(nItemHeight+m_sItemGap.cy)+ m_rcItem.top+m_rcInset.top - nLeftOffset;
			rc.bottom = rc.top + nItemHeight;
			pControl->SetPos(rc);
			nCXRequird = nGroupWidth*(nCurGroup+1);
		}
	}
	return nCXRequird;
}

void CPicHScrollUI::GroupLeft()
{
	if (m_nCurGroup>0)
	{
		GetManager()->KillTimer(this);
		GetManager()->SetTimer(this,ID_LEFT_TIMER,20);
	}
}

void CPicHScrollUI::GroupRight()
{
	int nGroup = GetGroupCount();
	if (m_nCurGroup<nGroup)
	{
		GetManager()->KillTimer(this);
		GetManager()->SetTimer(this,ID_RIGHT_TIMER,20);
	}
}
