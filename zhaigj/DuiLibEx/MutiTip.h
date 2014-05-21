#pragma once
#include <vector>
using namespace std;
#include <Gdiplus.h>
#pragma comment( lib, "gdiplus.lib" )
using namespace Gdiplus;
namespace DuiLib {


#define  ID_TIMER_HIDE 300
typedef struct TagTipText
{
	CString		strText;
	CRect		rcText;
	COLORREF   	cText;
	UINT       dStyle;
	HFONT			hFont;
	TagTipText(CString str,CRect rc,COLORREF col,UINT Style,HFONT	Font)
	{
		strText = str;
		rcText = rc;
		cText = col;
		dStyle= Style;
		hFont = Font;
	}
}TipText,TIPTEXT;

typedef struct TagTipImage
{
	CRect		 rcImage;
	UINT		 uTipImage;
	CString		 sType;
	int			 nPosX;
	int			 nPosY;
	TagTipImage(CRect rc,UINT  uImage, CString sTy = _T(""))
	{
		rcImage		= rc;
		uTipImage	= uImage;
		sType		= sTy;
		nPosX		= -1;
		nPosY       = -1;
	}
	TagTipImage(UINT uImage,int nX,int nY,int nW,int nH)
	{
		uTipImage	= uImage;
		nPosX		= nX;
		nPosY       = nY;
		rcImage.left= 0;
		rcImage.top = 0;
		rcImage.right=nW;
		rcImage.bottom=nH;
	}
}TipImage,TIPIMAGE;

class CMutiTip :public CWindowWnd
{
public:
	CMutiTip();
	~CMutiTip();
	virtual LPCTSTR GetWindowClassName() const{return _T("CMutiTip");};
public:
	void SetCloseImage(RECT RcCloseBtn,UINT pCloseImage,BOOL bVisable = TRUE);
	void SetBkImage(UINT pBkImage);

	void SetNoNextTimeImage(RECT RcNoNextImage,UINT pNoNextImage,LPCTSTR pStrKey,BOOL bVisable = TRUE); 
	void SetTriangleImage(RECT RcTriangle,UINT pTriangleImage,BOOL bVisable = TRUE);
	void UpdataImage();

	void SetAutoHeight(BOOL bAuto = TRUE);				// 目前只适用于出装推荐的 装备说明

	void SetAutoWidth(BOOL bAuto = TRUE);			

	void SetTexts(TipText* pTipTexts,UINT nCount);
	void SetImages(TipImage* pTipImages,UINT nCount);

	UINT CalculateTextHeight(TipText &pStrText,UINT nWidth);

	UINT CalculateTextWidth(TipText &pStrText);

	void ShowTip(INT nX,INT nY,INT nW,INT nH,BOOL bCenterPos = FALSE);

	void SetCheckBoxSize(SIZE);

	void AlphaText(HDC hdc,CRect rc,CRect rcText);

	void SetBKImageCorner(CRect rc);

	void HideTip();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	UINT		 		m_uCloseImage;
	UINT		 		m_uBkImage;
	UINT		 		m_uTriangleImage;
	UINT		 		m_uNoNextImage;
	RECT				m_rcCloseImage;
	RECT				m_rcTriangleImage;
	RECT				m_rcNoNextImage;

	RECT				m_rcCorner;

	BOOL				m_bCloseVisable;
	BOOL				m_bTriangleVisable;
	BOOL				m_bNoNextVisable;
	BOOL				m_bAutoHeight;
	BOOL				m_bAutoWidth;
	BOOL				m_bNoNextChecked;
	vector<TipText>		m_vTipText;
	vector<TipImage>	m_vTipImage;

	CString				m_sNoNextKey;

	INT					m_nFade;
	UINT				m_uCloseBtnState;
	UINT				m_uNoNextBtnState;
	CPaintManagerUI	*	m_pPM;
	CSize				m_CheckBoxSize;
};
}