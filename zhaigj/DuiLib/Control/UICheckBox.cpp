#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}

	void CCheckBoxUI::SetCheck(BOOL bCheck)
	{
		Selected(bCheck);
	}

	BOOL  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}
}
