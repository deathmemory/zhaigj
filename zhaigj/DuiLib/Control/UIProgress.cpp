#include "stdafx.h"
#include "UIProgress.h"

namespace DuiLib
{
	CProgressUI::CProgressUI() : m_bHorizontal(TRUE), m_nMin(0), m_nMax(100), m_nValue(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_uFgImage = 0;
		m_uAsImage = 0;
		m_uAbImage = 0;
		m_nABWidth	= 0;
		ZeroMemory(&m_rcBKInsert,sizeof(m_rcBKInsert));
		m_nABpos = 0;
		m_bSmooth = 0;
		m_nSmValue = 0;
		m_nSmStep  = 0;
	}

	LPCTSTR CProgressUI::GetClass() const
	{
		return _T("ProgressUI");
	}

	LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Progress")) == 0 ) return static_cast<CProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CProgressUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_TIMER ) {

			if (event.wParam == ID_TIMER)
			{
				m_nABpos+=3;

				int nTotal = m_nMax - m_nMin;
				CRect rcCilent = m_rcItem;
				rcCilent.left += m_rcBKInsert.left;
				rcCilent.top += m_rcBKInsert.top;
				rcCilent.right -= m_rcBKInsert.right;
				rcCilent.bottom -= m_rcBKInsert.bottom;
				//int nCurWidth   = (rcCilent.GetWidth()*m_nValue/nTotal) + m_nABWidth;

				if (m_nABpos>rcCilent.GetWidth()+m_nABWidth)
				{
					m_nABpos=0;
				}
				Invalidate();
			}
			else if (event.wParam == ID_TIMER_SMOOTH)
			{
				m_nValue+=m_nSmStep;

				if (m_nSmValue>0&&m_nValue>=m_nSmValue)
				{
					KillTimer(ID_TIMER_SMOOTH);
				}	
				if (m_nSmValue<0&&m_nValue<=m_nSmValue)
				{
					KillTimer(ID_TIMER_SMOOTH);
				}
				Invalidate();
			}
		}
	}

	BOOL CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(BOOL bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		if (m_bSmooth)
		{
			SetTimer(ID_TIMER_SMOOTH,50);
			m_nSmValue = nValue;
			int nGap = nValue - m_nValue;
			m_nSmStep = nGap/ID_SMOOTH_STEP;
		}
		else
		{
			m_nValue = nValue;
		}
		Invalidate();
	}

	LPCTSTR CProgressUI::GetFgImage() const
	{
		return m_sFgImage;
	}

	void CProgressUI::SetFgImage(LPCTSTR pStrImage)
	{
		if( m_sFgImage == pStrImage ) return;

		m_sFgImage = pStrImage;
		Invalidate();
	}
	void CProgressUI::SetFgImage(UINT uImage)
	{
		if (m_uFgImage == uImage)return;

		m_uFgImage = uImage;
		Invalidate();
	}

	void CProgressUI::SetShowAnmi(BOOL bShowAnmi)
	{
		m_bShowAnmi = bShowAnmi;
		if (m_bShowAnmi)
		{
			SetTimer(ID_TIMER,ID_TIMER_NELA);
		}
		else
		{
			KillTimer(ID_TIMER);
		}
	}

	void CProgressUI::SetSmooth(BOOL b)
	{
		m_bSmooth = b;
		if (!m_bSmooth)
		{
			KillTimer(ID_TIMER_SMOOTH);
		}
	}

	BOOL CProgressUI::GetShowAnmi()
	{
		return m_bShowAnmi;
	}

	void CProgressUI::SetAnmiImage(UINT uAsImage,UINT uAbImage)
	{
		m_uAbImage = uAbImage;
		m_uAsImage = uAsImage;
		Invalidate();
	}

	void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("fgimage")) == 0 ) SetFgImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("TRUE")) == 0);
		else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::SetnABWidth(int nW)
	{
		m_nABWidth = nW;
	}

	void CProgressUI::SetBKInsert(RECT rc)
	{
		m_rcBKInsert = rc;
	}

	void CProgressUI::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		if (m_bShowAnmi)
		{
			int nTotal = m_nMax - m_nMin;
			CRect rcCilent = m_rcItem;
			rcCilent.left += m_rcBKInsert.left;
			rcCilent.top += m_rcBKInsert.top;
			rcCilent.right -= m_rcBKInsert.right;
			rcCilent.bottom -= m_rcBKInsert.bottom;

			int nTotalWidth = rcCilent.GetWidth();// m_nValue*nTotal
			int nCurWidth   = rcCilent.GetWidth()*m_nValue/nTotal;

			CRect rcEffect(rcCilent.left,rcCilent.top,rcCilent.left+nCurWidth,rcCilent.top+rcCilent.GetHeight());
			if (m_uAsImage)
			{
				CString strModify;strModify.Format(_T("dest='%d,%d,%d,%d' corner='2,2,2,2'"), m_rcBKInsert.left, m_rcBKInsert.top,m_rcBKInsert.left+ nCurWidth,m_rcBKInsert.top+rcCilent.GetHeight());
				if( !DrawImage(hDC, RES(m_uAsImage), (LPCTSTR)strModify) ) m_uAsImage = 0;
			}
			if (m_uAbImage)
			{
				if(m_nABpos>nCurWidth+m_nABWidth)return;

				int nPos = m_nABpos - m_nABWidth;

				CRect rcdest(nPos,m_rcBKInsert.top,nPos+m_nABWidth,m_rcBKInsert.top+rcCilent.GetHeight());
				CRect rcsou(0,0,m_nABWidth,rcCilent.GetHeight());
				if (nPos<0)
				{
					rcsou.left = 0-nPos;
					rcdest.left = 0;
				}
				if (nPos>nCurWidth-m_nABWidth)
				{
					rcsou.right = nCurWidth-nPos;
					rcdest.right = m_rcBKInsert.left+nCurWidth;
				}
				CString depart;

				depart.Format(_T("source='%d,%d,%d,%d' dest='%d,%d,%d,%d'"),rcsou.left ,rcsou.top ,rcsou.right ,rcsou.bottom,rcdest.left ,rcdest.top ,rcdest.right ,rcdest.bottom);

				if( !DrawImage(hDC, RES(m_uAbImage), (LPCTSTR)depart) ) m_uAbImage = 0;
			}
		}
		else
		{
			if( !m_sFgImage.IsEmpty() ) {
				m_sFgImageModify.Empty();
				m_sFgImageModify.Format(_T("dest='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);

				if( !DrawImage(hDC, (LPCTSTR)m_sFgImage, (LPCTSTR)m_sFgImageModify) ) m_sFgImage.Empty();
				else return;
			}
			else if (m_uFgImage!=0)
			{
				m_sFgImageModify.Empty();
				m_sFgImageModify.Format(_T("dest='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);

				if( !DrawImage(hDC, RES(m_uFgImage), (LPCTSTR)m_sFgImageModify) ) m_uFgImage = 0;
				else return;
			}
		}
	}


}
