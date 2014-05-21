#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class UILIB_API CControlUI
{
public:
    CControlUI();
    virtual ~CControlUI();

public:
    virtual CString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;

    virtual BOOL Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, BOOL bInit = TRUE);
    virtual CControlUI* GetParent() const;

	// 定时器
	virtual void SetTimer(UINT nID , UINT nElapse);
	virtual void KillTimer(UINT nID);
    // 文本相关
    virtual CString GetText() const;
    virtual void SetText(LPCTSTR pstrText);

    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
	void SetBkImage(UINT nImage);

	void SetBkImageModify(LPCTSTR pStrImage);
	CString GetBkImageModify();

	void SetResType(LPCTSTR str);
	CString  GetResType();
    DWORD GetBorderColor() const;
    void SetBorderColor(DWORD dwBorderColor);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    BOOL IsColorHSL() const;
    void SetColorHSL(BOOL bColorHSL);
    int GetBorderSize() const;
    void SetBorderSize(int nSize);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    BOOL DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
	BOOL DrawImage(HDC hDC, UINT nID, LPCTSTR pStrModify = NULL);
    // 位置相关
    virtual const RECT& GetPos() const;
    virtual void SetPos(RECT rc);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为TRUE时有效
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
    virtual void SetRelativePos(SIZE szMove,SIZE szZoom);
    virtual void SetRelativeParentSize(SIZE sz);
    virtual TRelativePosUI GetRelativePos() const;
    virtual BOOL IsRelativePos() const;

    // 鼠标提示
    virtual CString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual BOOL IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(BOOL bMenuUsed);

    // 用户属性
    virtual const CString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

    // 一些重要的属性
    virtual BOOL IsVisible() const;
    virtual void SetVisible(BOOL bVisible = TRUE);
    virtual void SetInternVisible(BOOL bVisible = TRUE); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual BOOL IsEnabled() const;
    virtual void SetEnabled(BOOL bEnable = TRUE);
    virtual BOOL IsMouseEnabled() const;
    virtual void SetMouseEnabled(BOOL bEnable = TRUE);
    virtual BOOL IsFocused() const;
    virtual void SetFocus();
    virtual BOOL IsFloat() const;
    virtual void SetFloat(BOOL bFloat = TRUE);
	virtual UINT GetID() const;
	virtual void SetID(UINT nID);

	virtual RECT GetStretchPos() const;
	virtual void SetStretchPos(RECT rcPos);

	virtual BOOL GetMouseMoveNotify() const;
	virtual void SetMouseMoveNotify(BOOL nID);

	virtual BOOL IsChecked() const;
	virtual void SetCheck(BOOL bCheck = TRUE);

	virtual BOOL IsParentNotify() const;
	virtual void SetParentNotify(BOOL bCheck = TRUE);

    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    BOOL IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    CControlUI* ApplyAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;

protected:
    CPaintManagerUI* m_pManager;
    CControlUI* m_pParent;
    CString m_sName;
	CString m_sResType;
    BOOL m_bUpdateNeeded;
    BOOL m_bMenuUsed;
    RECT m_rcItem;
    RECT m_rcPadding;
	RECT m_rcItemStretch;
    SIZE m_cXY;  //相对于parent的xy
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    BOOL m_bVisible;
    BOOL m_bInternVisible;
    BOOL m_bEnabled;
    BOOL m_bMouseEnabled;
    BOOL m_bFocused;
    BOOL m_bFloat;
    BOOL m_bSetPos; // 防止SetPos循环调用

	BOOL m_bFirstSetPos;

	BOOL m_bIsChecked;
	BOOL m_bMouseMoveNotify;
	BOOL m_bMouseMoved;
	BOOL m_bNotifyParent;

	UINT m_nControlID;
    TRelativePosUI m_tRelativePos;

    CString m_sText;
    CString m_sToolTip;
    TCHAR m_chShortcut;
    CString m_sUserData;
    UINT_PTR m_pTag;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    CString m_sBkImage;
	UINT  m_nBkImage;
	CString m_sBkImageModify; 

    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    BOOL m_bColorHSL;
    int m_nBorderSize;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
