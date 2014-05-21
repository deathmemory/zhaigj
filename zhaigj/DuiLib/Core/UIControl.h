#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once




namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
static CString GetResString(UINT uID)
{
	CString strRet;
	strRet.Format(_T("%d"),uID);
	return strRet;
}

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class UILIB_API CControlUI
{
public:
    CControlUI();
    virtual ~CControlUI();

public:
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;

    virtual BOOL Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, BOOL bInit = TRUE);
    virtual CControlUI* GetParent() const;

    // 文本相关
    virtual CDuiString GetText() const;
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
	void SetBkImage(UINT uID);
	void SetBkImageModify(LPCTSTR pmodify);

	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    BOOL IsColorHSL() const;
    void SetColorHSL(BOOL bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    BOOL DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);

	void SetBorderSize(RECT rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

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

	virtual void SetFixedPos(RECT rcP);

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
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual BOOL IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(BOOL bMenuUsed);

    // 用户属性
    virtual const CDuiString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用
	virtual DWORD GetTag1() const; // 辅助函数，供用户使用1
	virtual void SetTag1(DWORD pTag); // 辅助函数，供用户使用1

    // 一些重要的属性
    virtual BOOL IsVisible() const;
    virtual void SetVisible(BOOL bVisible = TRUE);
    virtual void SetInternVisible(BOOL bVisible = TRUE); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual BOOL IsEnabled() const;
    virtual void SetEnabled(BOOL bEnable = TRUE);
    virtual BOOL IsMouseEnabled() const;
    virtual void SetMouseEnabled(BOOL bEnable = TRUE);
    virtual BOOL IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(BOOL bEnable = TRUE);
    virtual BOOL IsFocused() const;
    virtual void SetFocus();
    virtual BOOL IsFloat() const;
    virtual void SetFloat(BOOL bFloat = TRUE);

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

	//虚拟窗口参数
	void SetVirtualWnd(LPCTSTR pstrValue);
	CDuiString GetVirtualWnd() const;

	// 定时器
	virtual void SetTimer(UINT nID , UINT nElapse);
	virtual void KillTimer(UINT nID);

	void SetID(UINT uID);
	UINT GetID();
	virtual BOOL GetMouseMoveNotify() const;
	virtual void SetMouseMoveNotify(BOOL nID);

	virtual BOOL IsChecked() const;
	virtual void SetCheck(BOOL bCheck = TRUE);

	virtual BOOL IsParentNotify() const;
	virtual void SetParentNotify(BOOL bCheck = TRUE);

	void SetStretchMode(DWORD dwStret,RECT rcStretchMode);
	DWORD GetStretchMode();

	RECT GetStretchRect();
public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;

protected:
    CPaintManagerUI* m_pManager;
    CControlUI* m_pParent;
	CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    BOOL m_bUpdateNeeded;
    BOOL m_bMenuUsed;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    BOOL m_bVisible;
    BOOL m_bInternVisible;
    BOOL m_bEnabled;
    BOOL m_bMouseEnabled;
	BOOL m_bKeyboardEnabled ;
    BOOL m_bFocused;
    BOOL m_bFloat;
    BOOL m_bSetPos; // 防止SetPos循环调用
    TRelativePosUI m_tRelativePos;

    CDuiString m_sText;
    CDuiString m_sToolTip;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;
	DWORD m_dwTag1;

	RECT  m_rcStretch;
	DWORD m_dwStretchMode;
    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    CDuiString m_sBkImage;
	CDuiString m_sForeImage;
    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    BOOL m_bColorHSL;
    int m_nBorderSize;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
	RECT m_rcBorderSize;

	CString m_strBkImageModify;
	UINT m_uID;
	BOOL m_bIsChecked;
	BOOL m_bMouseMoveNotify;
	BOOL m_bMouseMoved;
	BOOL m_bNotifyParent;
};
#define RES(x) (LPCTSTR)GetResString(x)
} // namespace DuiLib

#endif // __UICONTROL_H__
