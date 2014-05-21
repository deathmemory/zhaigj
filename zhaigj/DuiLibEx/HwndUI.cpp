#include "StdAfx.h"
#include "HwndUI.h"
using namespace DuiLib;
CHwndUI::CHwndUI(void)
{
	m_UIHwnd = NULL;
}

CHwndUI::~CHwndUI(void)
{

}

LPCTSTR CHwndUI::GetClass() const
{
	return _T("CHwndUI");
}
LPVOID CHwndUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CHwndUI")) == 0 ) return static_cast<CHwndUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}
UINT CHwndUI::GetControlFlags() const
{
	if( IsEnabled() ) return 0;
	else return 0;
}

void CHwndUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	CRect rcUI = m_rcItem;
	if (m_UIHwnd&&::IsWindow(m_UIHwnd))
	{
		::SetWindowPos(m_UIHwnd,NULL,rcUI.left,rcUI.top,rcUI.GetWidth(),rcUI.GetHeight(),SWP_NOZORDER);
	}
}

void CHwndUI::SetVisible(BOOL bVisible /* = TRUE */ )
{
	CControlUI::SetVisible(bVisible);
	if (m_UIHwnd&&::IsWindow(m_UIHwnd))
	{
		ShowWindow(m_UIHwnd,bVisible?SW_SHOW:SW_HIDE);
		//::SetWindowPos(m_UIHwnd,NULL,rcUI.left,rcUI.top,rcUI.GetWidth(),rcUI.GetHeight(),SWP_NOZORDER);
	}
}

HWND CHwndUI::GetUIHwnd()
{
	return m_UIHwnd;
}

void CHwndUI::SetUIHwnd(HWND hWnd)
{
	m_UIHwnd = hWnd;
}
