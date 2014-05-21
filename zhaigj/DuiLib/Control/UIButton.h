#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib
{
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

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage();
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);
		LPCTSTR GetHotForeImage();
		void SetHotForeImage(LPCTSTR pStrImage);

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetButtonImage(LPCTSTR pstrName);
		void SetButtonImage(UINT uName);

		void SetImage(UINT uName);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		void SetCheckGroup(UINT nMin , UINT nMax);
		void SetButtonType(UINT uType);
		void SetCheck(BOOL bIsC = TRUE);
		void SetEightStateImageSize(SIZE size);

		void SetTextColor(DWORD dNormalColor,DWORD dHotColor = _INVALID_COLOR,DWORD dPressColor = _INVALID_COLOR,DWORD ddisableColor = _INVALID_COLOR);

		void SetCorner(CRect corner);

		int  GetTextRight();

		void SetSelfCheck(BOOL sC);

		void SetCheckBoxImagePadding(int nX);

		void SetActiveString(CString str);

	protected:
		CString m_strActive;
		int  m_nImagePadding;
		BOOL m_bSelfCheck;
		UINT m_uButtonState;
		UINT m_uButtonType;

		DWORD m_dwHotBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sHotForeImage;
		CDuiString m_sPushedImage;
		CDuiString m_sPushedForeImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;

		SIZE m_sEightStateImageSize;
		UINT m_uCheckGroupMinID;
		UINT m_uCheckGroupMaxID;

		RECT		m_rcCorner;
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__