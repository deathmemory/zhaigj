#ifndef __UICOMMONCONTROLS_H__
#define __UICOMMONCONTROLS_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CLabelUI : public CControlUI
{
public:
    CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

    void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
	DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
	DWORD GetDisabledTextColor() const;
    void SetFont(int index);
	int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    BOOL IsShowHtml();
    void SetShowHtml(BOOL bShowHtml = TRUE);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);

	virtual void SetVisible(BOOL bVisible  = TRUE ); 

	void SetImage(UINT);
	void SetImage(LPCTSTR);

	void SetImageModify(LPCTSTR);

	void SetLabelClick(BOOL bClick = TRUE);

	void PaintBkImage(HDC hDC);
	
protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    BOOL m_bShowHtml;
	BOOL m_bLabelClick;

	CString m_sImage;
	CString m_sImageModify;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CButtonUI : public CLabelUI
{
public:
    CButtonUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    BOOL Activate();
    void SetEnabled(BOOL bEnable = TRUE);
    void DoEvent(TEventUI& event);

    LPCTSTR GetImageStr();
    void SetImage(LPCTSTR pStrImage);
	void SetImage(UINT);

	void SetModifyString(LPCTSTR pStrModify);

    void SetHotTextColor(DWORD dwColor);
	DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
	DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
	DWORD GetFocusedTextColor() const;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

	void SetCheckGroup(UINT nMin , UINT nMax);
	void SetButtonType(UINT uType);
	
	void SetEightStateImageSize(SIZE size);
	void SetTextColor(DWORD dNormalColor,DWORD dHotColor = _INVALID_COLOR,DWORD dPressColor = _INVALID_COLOR,DWORD ddisableColor = _INVALID_COLOR);
	void SetCheck(BOOL bIsC = TRUE);
protected:
    UINT m_uButtonState;
	UINT m_uButtonType;

	SIZE m_sEightStateImageSize;
	UINT m_uCheckGroupMinID;
	UINT m_uCheckGroupMaxID;

	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;

    CString	m_sImage;
	UINT		m_uImageID;

	CString m_sImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API COptionUI : public CButtonUI
{
public:
    COptionUI();
    ~COptionUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, BOOL bInit = TRUE);

    BOOL Activate();
    void SetEnabled(BOOL bEnable = TRUE);

    LPCTSTR GetSelectedImage();
    void SetSelectedImage(LPCTSTR pStrImage);
	
	void SetSelectedTextColor(DWORD dwTextColor);
	DWORD GetSelectedTextColor();
		
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);
    BOOL IsSelected() const;
    void Selected(BOOL bSelected);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);

	

protected:
    BOOL m_bSelected;
    CString m_sGroupName;

	DWORD m_dwSelectedTextColor;

    CString m_sSelectedImage;
	CString m_sForeImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CTextUI : public CLabelUI
{
public:
    CTextUI();
    ~CTextUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CString* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);

    void PaintText(HDC hDC);

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CString m_sLinks[MAX_LINK];
    int m_nHoverLink;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CProgressUI : public CLabelUI
{
public:
    CProgressUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	void DoEvent(TEventUI& event);

    BOOL IsHorizontal();
    void SetHorizontal(BOOL bHorizontal = TRUE);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetFgImage() const;
    void SetFgImage(LPCTSTR pStrImage);
	void SetFgImage(UINT uImage);

	void SetAnmiImage(UINT uAsImage,UINT uAbImage);
	void SetShowAnmi(BOOL bShowAnmi = TRUE);
	BOOL GetShowAnmi();
	void SetnABWidth(int nW);
	void SetBKInsert(RECT rc);

	void SetSmooth(BOOL bEnable = TRUE);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);
	enum
	{
		ID_TIMER = 45,
		ID_TIMER_NELA = 10,
	};
protected:
	enum{
		 ID_ELAPSE			=100,
		 ID_TIMER_SMOOTH	=100,
		 ID_SMOOTH_STEP		=20
	};

    BOOL m_bHorizontal;
	BOOL m_bShowAnmi;

	BOOL m_bSmooth;
	int  m_nSmValue;
	int  m_nSmStep;

    int m_nMax;
    int m_nMin;
    int m_nValue;

    CString m_sFgImage;
	UINT	   m_uFgImage;

	UINT	   m_uAsImage;
	UINT	   m_uAbImage;

	int		   m_nABpos;

	RECT	   m_rcBKInsert;
	int		   m_nABWidth;
    CString m_sFgImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CSliderUI : public CProgressUI
{
public:
    CSliderUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(BOOL bEnable = TRUE);

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    LPCTSTR GetThumbImage() const;
    void SetThumbImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage() const;
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage() const;
    void SetThumbPushedImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

protected:
    SIZE m_szThumb;
    UINT m_uButtonState;
    int m_nStep;

    CString m_sThumbImage;
    CString m_sThumbHotImage;
    CString m_sThumbPushedImage;

    CString m_sImageModify;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CEditWnd;

class UILIB_API CEditUI : public CLabelUI
{
    friend CEditWnd;
public:
    CEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(BOOL bEnable = TRUE);
    void SetText(LPCTSTR pstrText);
    void SetMaxChar(UINT uMax);
    UINT GetMaxChar();
    void SetReadOnly(BOOL bReadOnly);
    BOOL IsReadOnly() const;
    void SetPasswordMode(BOOL bPasswordMode);
    BOOL IsPasswordMode() const;
    void SetPasswordChar(TCHAR cPasswordChar);
    TCHAR GetPasswordChar() const;

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
    void SetNativeEditBkColor(DWORD dwBkColor);
    DWORD GetNativeEditBkColor() const;

    void SetPos(RECT rc);
    void SetVisible(BOOL bVisible = TRUE);
    void SetInternVisible(BOOL bVisible = TRUE);
    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
    void PaintText(HDC hDC);

protected:
    CEditWnd* m_pWindow;

    UINT m_uMaxChar;
    BOOL m_bReadOnly;
    BOOL m_bPasswordMode;
    TCHAR m_cPasswordChar;
    UINT m_uButtonState;
    CString m_sNormalImage;
    CString m_sHotImage;
    CString m_sFocusedImage;
    CString m_sDisabledImage;
    DWORD m_dwEditbkColor;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CScrollBarUI : public CControlUI
{
public:
    CScrollBarUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CContainerUI* GetOwner() const;
    void SetOwner(CContainerUI* pOwner);

    void SetEnabled(BOOL bEnable = TRUE);
    void SetFocus();

    BOOL IsHorizontal();
    void SetHorizontal(BOOL bHorizontal = TRUE);
    int GetScrollRange() const;
    void SetScrollRange(int nRange);
    int GetScrollPos() const;
    void SetScrollPos(int nPos);
    int GetLineSize() const;
    void SetLineSize(int nSize);

    BOOL GetShowButton1();
    void SetShowButton1(BOOL bShow);
    LPCTSTR GetButton1NormalImage();
    void SetButton1NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1HotImage();
    void SetButton1HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1PushedImage();
    void SetButton1PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1DisabledImage();
    void SetButton1DisabledImage(LPCTSTR pStrImage);

    BOOL GetShowButton2();
    void SetShowButton2(BOOL bShow);
    LPCTSTR GetButton2NormalImage();
    void SetButton2NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2HotImage();
    void SetButton2HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2PushedImage();
    void SetButton2PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2DisabledImage();
    void SetButton2DisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetThumbNormalImage();
    void SetThumbNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage();
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage();
    void SetThumbPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbDisabledImage();
    void SetThumbDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetRailNormalImage();
    void SetRailNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetRailHotImage();
    void SetRailHotImage(LPCTSTR pStrImage);
    LPCTSTR GetRailPushedImage();
    void SetRailPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetRailDisabledImage();
    void SetRailDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetBkNormalImage();
    void SetBkNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetBkHotImage();
    void SetBkHotImage(LPCTSTR pStrImage);
    LPCTSTR GetBkPushedImage();
    void SetBkPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetBkDisabledImage();
    void SetBkDisabledImage(LPCTSTR pStrImage);

	UINT GetReguleImage();
	void SetRegulerImage(UINT uImage,int nblockW,int nblockH);


    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DoPaint(HDC hDC, const RECT& rcPaint);

    void PaintBk(HDC hDC);
    void PaintButton1(HDC hDC);
    void PaintButton2(HDC hDC);
    void PaintThumb(HDC hDC);
    void PaintRail(HDC hDC);

protected:

    enum { 
        DEFAULT_SCROLLBAR_SIZE = 16,
        DEFAULT_TIMERID = 10,
    };

    BOOL m_bHorizontal;
    int m_nRange;
    int m_nScrollPos;
    int m_nLineSize;
    CContainerUI* m_pOwner;
    POINT ptLastMouse;
    int m_nLastScrollPos;
    int m_nLastScrollOffset;
    int m_nScrollRepeatDelay;

	int m_uImageID;

    CString m_sBkNormalImage;
    CString m_sBkHotImage;
    CString m_sBkPushedImage;
    CString m_sBkDisabledImage;

    BOOL m_bShowButton1;
    RECT m_rcButton1;
    UINT m_uButton1State;
    CString m_sButton1NormalImage;
    CString m_sButton1HotImage;
    CString m_sButton1PushedImage;
    CString m_sButton1DisabledImage;

    BOOL m_bShowButton2;
    RECT m_rcButton2;
    UINT m_uButton2State;
    CString m_sButton2NormalImage;
    CString m_sButton2HotImage;
    CString m_sButton2PushedImage;
    CString m_sButton2DisabledImage;

    RECT m_rcThumb;
    UINT m_uThumbState;
    CString m_sThumbNormalImage;
    CString m_sThumbHotImage;
    CString m_sThumbPushedImage;
    CString m_sThumbDisabledImage;

    CString m_sRailNormalImage;
    CString m_sRailHotImage;
    CString m_sRailPushedImage;
    CString m_sRailDisabledImage;

    CString m_sImageModify;
};

} // namespace DuiLib

#endif // __UICOMMONCONTROLS_H__

