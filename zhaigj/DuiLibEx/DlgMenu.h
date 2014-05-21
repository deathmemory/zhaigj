#pragma once
#include "stdafx.h"
#include "SkinDialog.h"
#include <vector>
using namespace std;
#define WM_MENU_CLICK WM_USER+70
typedef struct tagMenuItem
{
	int nID;
	CString sText;
	int HeadimageID;
}MenuItem;

class CMenuBtnUI :public CButtonUI
{
public:
	CMenuBtnUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void	SetIconImage(UINT nID,RECT rcIcon,LPCTSTR strModify = NULL);
	void PaintStatusImage(HDC hDC);
private:
	UINT	m_nIconID;
	CString m_sIconModify;
	RECT    m_rcIcon;
};

class CDlgMenu : public CSkinDialog
{

public:
	CDlgMenu();   // standard constructor
	virtual ~CDlgMenu();
	
	virtual void InitControls();
	virtual void Notify(TNotifyUI& msg);

	LPCTSTR GetWindowClassName() const { return _T("CDlgMenu"); };
public:
	BOOL Initialization();
	void AddItem(int nID,CString ItemText,int ImageheaderID = 0);
	void InitItemList();
	void MenuPopup(HWND hWndParent,int nX,int nY);
	void SetItemBkImage(UINT nID);
	void SetItemWidth(UINT nID);
	void SetSkin(UINT nBK);
	void SetCorner(RECT rc);
	void SetID(UINT nID);
	UINT GetID();
	void SetMenuIconRect(RECT rc);
	virtual LRESULT OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled);
private:
	vector<MenuItem*>	m_ItemList;
	UINT				m_nItemBKID;
	UINT				m_nItemWidth;
	UINT				m_nSkinBK;
	RECT				m_rcCorner;
	RECT				m_rcIcon;
	UINT				m_nID;
};
