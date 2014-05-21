#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;
class UILIB_API Gdiplus::RectF;
struct UILIB_API Gdiplus::GdiplusStartupInput;

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		UINT	GetControlFlags() const;

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

		void		SetEnabledEffect(BOOL _EnabledEffect);
		BOOL		GetEnabledEffect();
		void		SetText(LPCTSTR pstrText);
		CDuiString	GetText() const;
		void		SetTransShadow(int _TransShadow);
		int			GetTransShadow();
		void		SetTransShadow1(int _TransShadow);
		int			GetTransShadow1();
		void		SetTransText(int _TransText);
		int			GetTransText();
		void		SetTransText1(int _TransText);
		int			GetTransText1();
		void		SetTransStroke(int _TransStroke);
		int			GetTransStroke();
		void		SetGradientLength(int _GradientLength);
		int			GetGradientLength();
		void		SetTextRenderingHintAntiAlias(int _TextRenderingHintAntiAlias);
		int			GetTextRenderingHintAntiAlias();
		void		SetShadowOffset(int _offset,int _angle);
		RectF		GetShadowOffset();
		void		SetTextColor1(DWORD _TextColor1);
		DWORD		GetTextColor1();
		void		SetTextShadowColorA(DWORD _TextShadowColorA);
		DWORD		GetTextShadowColorA();
		void		SetTextShadowColorB(DWORD _TextShadowColorB);
		DWORD		GetTextShadowColorB();
		void		SetStrokeColor(DWORD _StrokeColor);
		DWORD		GetStrokeColor();
		void		SetGradientAngle(int _SetGradientAngle);
		int			GetGradientAngle();
		void		SetEnabledStroke(BOOL _EnabledStroke);
		BOOL		GetEnabledStroke();
		void		SetEnabledShadow(BOOL _EnabledShadowe);
		BOOL		GetEnabledShadow();

		void SetLabelClick(BOOL bClick = TRUE);
		
	protected:
		DWORD	m_dwTextColor;
		DWORD	m_dwDisabledTextColor;
		int		m_iFont;
		UINT	m_uTextStyle;
		RECT	m_rcTextPadding;
		BOOL	m_bShowHtml;
		BOOL    m_bCanCilck;

		int						m_hAlign;
		int						m_vAlign;
		int						m_TransShadow;
		int						m_TransShadow1;
		int						m_TransText;
		int						m_TransText1;
		int						m_TransStroke;
		int						m_GradientLength;
		int						m_GradientAngle;
		BOOL					m_EnableEffect;
		BOOL					m_EnabledStroke;
		BOOL					m_EnabledShadow;
		DWORD					m_dwTextColor1;
		DWORD					m_dwTextShadowColorA;
		DWORD					m_dwTextShadowColorB;
		DWORD					m_dwStrokeColor;
		RectF					m_ShadowOffset;
		CDuiString				m_TextValue;
		ULONG_PTR				m_gdiplusToken;
		GdiplusStartupInput		m_gdiplusStartupInput;
		TextRenderingHint		m_TextRenderingHintAntiAlias;
	};
}

#endif // __UILABEL_H__