#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace DuiLib
{
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

} // namespace DuiLib

#endif // __UIPROGRESS_H__
