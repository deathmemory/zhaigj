#if !defined(AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////////////
//
#include "shellapi.h"

using namespace DuiLib;
class CMessageBoxWnd;

class CUIMessageBox
{
	friend CMessageBoxWnd;
public:
	static int m_nCloseBtnPosToRightX;
	static int m_nCloseBtnPosToTopY;
	static UINT m_nTitleTextStyle;
	static COLORREF m_colorBtnTextNormal;
	static COLORREF m_colorBtnTextFocused;
	static COLORREF m_colorBtnTextPushed;
	static COLORREF m_colorBtnTextDisabled;
	static COLORREF m_colorInfoText;
	static COLORREF m_colorTitleText;
	static int  m_nCornerSpan;
	static UINT m_nCloseBtnImage;
	static UINT m_nLeftBtnImage;
	static UINT m_nRightBtnImage;
	static UINT m_nMidBtnImage;
	static UINT m_nBKImage;

	static int m_nTitleID;
	static int m_nSpaceSpan;
	static int m_nCommBtnWidth;
	static int m_nCommBtnHeight;
	static int m_nCommBtnSpan;
	static int m_nDlgTemplate;
	static HMODULE m_hResModule;

	static int m_nCloseBtnWidth;
	static int m_nCloseBtnHeight;
	//static CAppModule* m_hModule;
public:
	static void SetCloseBtnPos(int aiXToRight, int aiYToTop);//设置相对于右上角的坐标
	static void SetTitleID(UINT nID);
	static void SetSpaceSpan(int aiSpan);
	static void SetCornerSpan(int nSpan);
	static void SetBtnTextColor(COLORREF crNormal, COLORREF crFocused, COLORREF crPushed, COLORREF crDisabled);
	static void SetTitleTextColor(COLORREF color);
	static void SetInfoTextColor(COLORREF color);
	static void SetCommBtnSize(int aiWidth, int aiHeight);
	static void SetDlgTemplate(int nDlgTemplate);
	static void SetResModule(HMODULE hResModule);
	static void SetBKImage(UINT uImage);
	static void SetLeftImage(UINT uImage);
	static void SetMidImage(UINT uImage);
	static void SetRightImage(UINT uImage);
	static void SetCloseBtnImage(UINT uImage);
	static void SetCloseBtnSize(int aiWidth, int aiHeight);
	static int Show(HWND hWnd, 
		LPCTSTR lpText,
		LPCTSTR lpCaption = NULL,
		UINT uType = MB_OK | MB_ICONWARNING
		);
};


int UIMessageBox(HWND hWnd,  LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK | MB_ICONINFORMATION );

int UIMessageBox(LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK | MB_ICONINFORMATION );
//#define  UIMessageBox CUIMessageBox::Show 


#endif // !defined(AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

