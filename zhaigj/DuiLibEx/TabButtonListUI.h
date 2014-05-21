#pragma once

class CTabButtonItemUI :public CLabelUI
{
public:
	CTabButtonItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	BOOL Activate();
	void DoEvent(TEventUI& event);

	void SetStatueImage(UINT);

	void SetStatueImage(LPCTSTR);

	void SetTextColor(COLORREF,COLORREF,COLORREF,COLORREF);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);

	void PaintStatusImage(HDC hDC);

protected:
	UINT		m_nStatueImage;
	CStdString  m_strStatueImage;

	UINT		m_uButtonState;

	DWORD       m_TextColNormal;
	DWORD       m_TextColHot;
	DWORD       m_TextColPressed;
	DWORD       m_TextColDisable;
};

class CTabButtonListUI : public CVerticalLayoutUI
{
public:
	CTabButtonListUI(void);
	~CTabButtonListUI(void);
public:

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DoEvent(TEventUI& event);

	void SetPos(RECT rc);

	void SetHeaderVisable(BOOL bVisable = TRUE);

	
protected:
	CStdPtrArray		m_PicItems;
};

