#pragma once
#include "PictureList.h"

#define  ID_LEFT_TIMER 10
#define  ID_RIGHT_TIMER 11
class CPicHScrollUI: public CVerticalLayoutUI
{
public:
	CPicHScrollUI(void);
	~CPicHScrollUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetTitleBtnImage(UINT m_nTitleBtnImage);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void DoEvent(TEventUI& event);

	void SetPos(RECT rc);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);

	void InsertGroup(UINT nID,LPCTSTR pstrGroupName);
	CPicListItemUI* InsertItem(LPCTSTR ItemName);

	void SetColRow(UINT nCol,UINT nRow);
	void SetAutoCol(BOOL bAuto);

	void SetItemGap(SIZE sGap);

	void SetItemRect(RECT rc);

	UINT CalculateItemPos(RECT rc);

	int  GetCurGroup();
	int  GetGroupCount();

	void SetHSrollPos(int hPos);

	BOOL Expend(BOOL bExpend);
	BOOL IsExpend();
	void SetMaxExpendHeight(UINT uExpendHeight);
	
	void GroupLeft();
	void GroupRight();

	void RemoveAll();

protected:
	enum                { MAX_INDEX = 30};
	POINT				ptLastMouse;
	RECT				m_rcPicItem;

	CStdPtrArray		m_PicItems;
	
	UINT				m_nRow;
	UINT                m_nCol;
	SIZE                m_sItemGap;//Item ¼ä¸ô
	int					m_nCXoffset;
	static  INT         m_ExpendGroup[MAX_INDEX];

	int					m_nCurGroup;
	INT					m_nCurIndex;
};
