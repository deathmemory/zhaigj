#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTabLayoutUI : public CContainerUI
	{
	public:
		CTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		BOOL Add(CControlUI* pControl);
		BOOL AddAt(CControlUI* pControl, int iIndex);
		BOOL Remove(CControlUI* pControl);
		void RemoveAll();
		int GetCurSel() const;
		BOOL SelectItem(int iIndex);
		BOOL SelectItem(CControlUI* pControl);

		void SetPos(RECT rc);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		int m_iCurSel;
	};
}
#endif // __UITABLAYOUT_H__
