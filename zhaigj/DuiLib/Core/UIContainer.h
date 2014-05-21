#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class IContainerUI
{
public:
    virtual CControlUI* GetItemAt(int iIndex) const = 0;
    virtual int GetItemIndex(CControlUI* pControl) const  = 0;
    virtual BOOL SetItemIndex(CControlUI* pControl, int iIndex)  = 0;
    virtual int GetCount() const = 0;
    virtual BOOL Add(CControlUI* pControl) = 0;
    virtual BOOL AddAt(CControlUI* pControl, int iIndex)  = 0;
    virtual BOOL Remove(CControlUI* pControl) = 0;
    virtual BOOL RemoveAt(int iIndex)  = 0;
    virtual void RemoveAll() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
class CScrollBarUI;

class UILIB_API CContainerUI : public CControlUI, public IContainerUI
{
public:
    CContainerUI();
    virtual ~CContainerUI();

public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CControlUI* GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI* pControl) const;
    BOOL SetItemIndex(CControlUI* pControl, int iIndex);
    int GetCount() const;
    BOOL Add(CControlUI* pControl);
    BOOL AddAt(CControlUI* pControl, int iIndex);
    BOOL Remove(CControlUI* pControl);
    BOOL RemoveAt(int iIndex);
    void RemoveAll();

    void DoEvent(TEventUI& event);
    void SetVisible(BOOL bVisible = TRUE);
    void SetInternVisible(BOOL bVisible = TRUE);
    void SetMouseEnabled(BOOL bEnable = TRUE);

    virtual RECT GetInset() const;
    virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
    virtual int GetChildPadding() const;
    virtual void SetChildPadding(int iPadding);
    virtual BOOL IsAutoDestroy() const;
    virtual void SetAutoDestroy(BOOL bAuto);
    virtual BOOL IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(BOOL bDelayed);
    virtual BOOL IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(BOOL bEnable = TRUE);

    virtual int FindSelectable(int iIndex, BOOL bForward = TRUE) const;

    void SetPos(RECT rc);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, BOOL bInit = TRUE);
    CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	BOOL SetSubControlText(LPCTSTR pstrSubControlName,LPCTSTR pstrText);
	BOOL SetSubControlFixedHeight(LPCTSTR pstrSubControlName,int cy);
	BOOL SetSubControlFixedWdith(LPCTSTR pstrSubControlName,int cx);
	BOOL SetSubControlUserData(LPCTSTR pstrSubControlName,LPCTSTR pstrText);

	CDuiString GetSubControlText(LPCTSTR pstrSubControlName);
	int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
	int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
	const CDuiString GetSubControlUserData(LPCTSTR pstrSubControlName);
	CControlUI* FindSubControl(LPCTSTR pstrSubControlName);

    virtual SIZE GetScrollPos() const;
    virtual SIZE GetScrollRange() const;
    virtual void SetScrollPos(SIZE szPos);
    virtual void LineUp();
    virtual void LineDown();
    virtual void PageUp();
    virtual void PageDown();
    virtual void HomeUp();
    virtual void EndDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void HomeLeft();
    virtual void EndRight();
    virtual void EnableScrollBar(BOOL bEnableVertical = TRUE, BOOL bEnableHorizontal = FALSE);
    virtual CScrollBarUI* GetVerticalScrollBar() const;
    virtual CScrollBarUI* GetHorizontalScrollBar() const;

protected:
    virtual void SetFloatPos(int iIndex);
    virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
    CStdPtrArray m_items;
    RECT m_rcInset;
    int m_iChildPadding;
    BOOL m_bAutoDestroy;
    BOOL m_bDelayedDestroy;
    BOOL m_bMouseChildEnabled;
    BOOL m_bScrollProcess; // 防止SetPos循环调用

    CScrollBarUI* m_pVerticalScrollBar;
    CScrollBarUI* m_pHorizontalScrollBar;
};

} // namespace DuiLib

#endif // __UICONTAINER_H__
