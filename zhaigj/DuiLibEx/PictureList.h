#pragma once
namespace DuiLib
{

#define ID_TIMER_EXPEND 10000
#define ID_TIMER_UNEXPEND 10001
class CPicListItemUI ;
typedef struct UILIB_API tgaPictureGroup
{
	CButtonUI	*	m_pTitleBtn;
	CStdPtrArray	m_items;
	UINT		    m_nGroupID;
	BOOL        *   m_bExpended;
}PIC_GROUP;

typedef struct UILIB_API tgaPictureItem
{
	CPicListItemUI	*	m_pItem;
	UINT				m_nItemID;
}PIC_ITEM;

typedef enum 
{
	ITEM_NORMAL = 0,
	ITEM_HOT,
	ITEM_PRESSED,
	ITEM_DISABLE
} ITEM_STATUE;

class UILIB_API CPicListItemUI :public CLabelUI
{
public:
	CPicListItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	BOOL Activate();
	void SetEnabled(BOOL bEnable = TRUE);
	void DoEvent(TEventUI& event);

	void SetStatueImage(UINT);
	void SetStatueImage(LPCTSTR);

	void SetPicImage(UINT);
	void SetPicImage(LPCTSTR);
	void SetPicImagePadding(RECT rc);

	void SetTextColor(COLORREF,COLORREF,COLORREF,COLORREF);

	virtual void SetCheck(BOOL bCheck = TRUE);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
	void PaintStatusImage(HDC hDC);
	void PaintChildImage(HDC hDC);

	void SetChildBtnEnable(BOOL bEn = TRUE);
	void SetChildImage(UINT nID);
	void SetChildRect(RECT rc);
	void SetOwner(CVerticalLayoutUI * );

	void SendSelToParent();
protected:
	UINT		m_nChildBtnImage;

	UINT		m_nStatueImage;
	CString  m_strStatueImage;
	BOOL		m_bChildBtnEnable;
	BOOL		m_bChildBtnVisable;
	UINT		m_nPicIamge;
	CString	m_strPicImage;
	RECT		m_rcPicPadding;
	RECT		m_rcChildBtn;

	UINT		m_uButtonState;
	UINT		m_uChildBtnState;

	DWORD       m_TextColNormal;
	DWORD       m_TextColHot;
	DWORD       m_TextColPressed;
	DWORD       m_TextColDisable;

	CVerticalLayoutUI * m_pOwner;
};

class UILIB_API CPictureList : public CVerticalLayoutUI
{
public:
	CPictureList(void);
	~CPictureList(void);

public:

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

	void SetCol(UINT nCol);
	void SetAutoCol(BOOL bAuto);

	void SetItemGap(SIZE sGap);

	void SetItemRect(RECT rc);

	virtual UINT CalculateItemPos(RECT rc);

	BOOL Expend(BOOL bExpend);
	BOOL IsExpend();
	void SetMaxExpendHeight(UINT uExpendHeight);

	void SetSel(int nID);
	void SetSelSilent(int nID);
	int GetSel();
protected:
	enum                { MAX_INDEX = 30};
	POINT				ptLastMouse;
	RECT				m_rcPicItem;
	
	CStdPtrArray		m_PicItems;

	BOOL				m_bAutoCol;
	UINT                m_nCol;
	SIZE                m_sItemGap;//Item ¼ä¸ô

	BOOL				m_bExpend;
	UINT                m_uExpendHeight;
	static  INT         m_ExpendGroup[MAX_INDEX];       
	INT				m_nCurIndex;
};
}

