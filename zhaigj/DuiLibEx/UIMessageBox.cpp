
#include "StdAfx.h"
#include "UIMessageBox.h"
#include "SkinDialog.h"
/////////////////////////////////////////////////////////////////////////////////////
//
//
#define LWA_COLORKEY 0x00000001         //方式
#define WS_EX_LAYERED 0x00080000 

class CMessageBoxWnd : public CSkinDialog
{
	typedef enum
	{
		IDC_BTN_CLOSE,

		IDC_STC_TITLE,

		IDC_STC_ICON,
		IDC_STC_INFO,

		IDC_BTN_OK,
		IDC_BTN_CANCEL,

		IDC_BTN_YES,
		IDC_BTN_NO
	};

public:
	void Init(HWND hWnd, LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);

	BOOL Initialization();

	void InitControl();

	LPCTSTR GetWindowClassName() const;

	void OnFinalMessage(HWND hWnd);

	LRESULT OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	virtual void Notify(TNotifyUI& msg);

public:
	
	HWND			m_hwndParent;

	CString			m_sCaption;

	CString			m_sText;

	UINT			m_uType;

	HICON			m_hIcon;

	static int		m_nRet;			

};

/////////////////////////////////////////////////////////////////////////////////////
//
//
int CMessageBoxWnd::m_nRet = 0;
void CMessageBoxWnd::Init(HWND hWnd,  LPCTSTR lpText,LPCTSTR lpCaption, UINT uType)
{
   m_hwndParent = hWnd;
   m_sCaption = lpCaption;
   m_sText= lpText;
   m_uType = uType;

   // Position the popup window in absolute space
 
   if (!m_hwndParent)
   {
	   m_hwndParent = ::GetActiveWindow();
   }
   RECT rc = {0, 0, 250 ,250};
  
   MapWindowRect(m_hwndParent, HWND_DESKTOP, &rc);
  // Create(m_hwndParent, NULL, UI_WNDSTYLE_SKINDLG_NO_MAX|WS_MAXIMIZE|WS_MINIMIZEBOX, WS_EX_TOOLWINDOW,0,0,0,0);

   ::ShowWindow(m_hWnd, SW_SHOW);
   ::SendMessage(m_hwndParent, WM_NCACTIVATE, TRUE, 0L);
}

BOOL CMessageBoxWnd::Initialization()
{
	SetSkinImage(CUIMessageBox::m_nBKImage);
	SetSkinCorner(CRect(3,50,3,3));
	InitControl();
	return TRUE;
}

void CMessageBoxWnd::InitControl()
{
	int nCloseBtnWidth = CUIMessageBox::m_nCloseBtnWidth;//m_pPM->m_pImageSkinCloseButton->GetWidth() / 4;
	int nCloseBtnHeight =CUIMessageBox::m_nCloseBtnHeight;// m_pPM->m_pImageSkinCloseButton->GetHeight();

	int nLinks = 0;
	RECT rcTitle = { 0, 0, 9999, 100 };
	CRenderEngine::DrawHtmlText(m_pPM->GetPaintDC(), m_pPM, rcTitle, m_sCaption, RGB(1,1,1), NULL, NULL, nLinks, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);

	RECT rcInfo = { 0, 0, 9999, 100 };
	CRenderEngine::DrawHtmlText(m_pPM->GetPaintDC(), m_pPM, rcInfo, m_sText,  RGB(1,1,1), NULL, NULL, nLinks,  DT_CALCRECT);

	//图标的大小
	int liIconWidth=0;
	int liIconHeight=0;
	Bitmap* lpIconImage = NULL;

	if ((m_uType & 0X000000F0) == MB_ICONWARNING)
	{
		ExtractIconEx(TEXT("user32.dll"), 1, &m_hIcon, NULL, 1);
	}
	else if ((m_uType & 0X000000F0) ==  MB_ICONEXCLAMATION)
	{
		ExtractIconEx(TEXT("user32.dll"), 4, &m_hIcon, NULL, 1);
	}

	else if ((m_uType & 0X000000F0) == MB_ICONQUESTION)
	{
		ExtractIconEx(TEXT("user32.dll"), 2, &m_hIcon, NULL, 1);
	}
	else if ((m_uType & 0X000000F0) == MB_ICONERROR)
	{
		ExtractIconEx(TEXT("user32.dll"), 3, &m_hIcon, NULL, 1);
	}
	else
	{
		ExtractIconEx(TEXT("user32.dll"), 4, &m_hIcon, NULL, 1);
	}
	ICONINFO icInfo = { 0 };
	if (::GetIconInfo(m_hIcon, &icInfo))
	{
		BITMAP bitmap; 
		GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		Bitmap* pBitmap = NULL;
		Bitmap* pWrapBitmap = NULL;
		if (bitmap.bmBitsPixel != 32)
		{   
			pBitmap = Bitmap::FromHICON(m_hIcon);
		} 
		else
		{
			pWrapBitmap = Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
			BitmapData bitmapData;
			Rect rcImage(0,0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			pWrapBitmap->LockBits(&rcImage, ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 

			pBitmap = new Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride,  PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);

			pWrapBitmap->UnlockBits(&bitmapData);
		}

		lpIconImage = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat32bppARGB);

		HBITMAP hBit ;

		lpIconImage->GetHBITMAP(ARGB(0,0,0,0),&hBit);

		m_pPM->AddImage(_T("msgBoxIcon"),hBit,lpIconImage->GetWidth(),lpIconImage->GetHeight(),TRUE);

		DeleteObject(icInfo.hbmColor); 
		DeleteObject(icInfo.hbmMask); 
	}
	if(lpIconImage != NULL)
	{
		liIconWidth=lpIconImage->GetWidth(); 
		liIconHeight=lpIconImage->GetHeight();
	}
	//计算图口的大小

	int nWinWidth= rcInfo.right - rcInfo.left + CUIMessageBox::m_nSpaceSpan*3+liIconWidth;
	int nWinHeight = 30+CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnHeight+liIconHeight;

	int liMinWidth=CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnWidth*2;
	int liMinHeight=30+CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnHeight+rcInfo.bottom-rcInfo.top;

	nWinWidth=nWinWidth < liMinWidth ? liMinWidth:nWinWidth;
	nWinHeight=nWinHeight < liMinHeight ? liMinHeight:nWinHeight;

	if ((m_uType & 0X0000000F) == MB_YESNOCANCEL)
	{
		nWinWidth +=55;
	}
	::SetWindowPos(GetHWND(), HWND_NOTOPMOST, 0, 0, nWinWidth, nWinHeight, SWP_NOZORDER|SWP_NOMOVE);

	{
		//COLORREF maskColor=RGB(255,255,255);
		//SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE) | WS_EX_LAYERED);
		//SetLayeredWindowAttributes(m_hWnd,  maskColor, 100, LWA_COLORKEY);
	}

	CenterWindow();


	// 添加标题
	{
		if (CUIMessageBox::m_nTitleID !=0)
		{ 
			CLabelUI*m_pLableTitle = RegiserLabel(0,CRect(8,8,100,40));
			m_pLableTitle->SetBkImage(RES(CUIMessageBox::m_nTitleID));
			const TImageInfo * pImage = m_pPM->GetImageEx(RES(CUIMessageBox::m_nTitleID),_T("PNG"));
			if (pImage)
			{
				m_pLableTitle->SetFixedWidth(pImage->nX);
				m_pLableTitle->SetFixedHeight(pImage->nY);
			}
		}
	}

	// 关闭按钮
	{
		RECT rect = {nWinWidth - nCloseBtnWidth - CUIMessageBox::m_nCloseBtnPosToRightX-2, 
			CUIMessageBox::m_nCloseBtnPosToTopY, 
			nWinWidth - CUIMessageBox::m_nCloseBtnPosToRightX,
			CUIMessageBox::m_nCloseBtnPosToTopY + nCloseBtnHeight
		};
		CButtonUI* pBtn = RegiserButton(IDC_BTN_CLOSE, rect);
		if (pBtn)
		{
			pBtn->SetButtonImage(RES(CUIMessageBox::m_nCloseBtnImage));
		}
	}

	// 添加图标
	int liIconX=CUIMessageBox::m_nSpaceSpan;
	int liIconY=30+CUIMessageBox::m_nSpaceSpan;
	//------------------------------------------------------------------------------------
	{
		if(lpIconImage != NULL)
		{
			RECT rect = {liIconX, liIconY, liIconX + liIconWidth , liIconY + liIconHeight};
			CLabelUI* pLabel = RegiserLabel(IDC_STC_ICON, rect);
			pLabel->SetBkImage(_T("msgBoxIcon"));
		}
	}
	//------------------------------------------------------------------------------------
	// 添加文字
	{
		RECT rect = {liIconX+liIconWidth+CUIMessageBox::m_nSpaceSpan, liIconY+5, nWinWidth-CUIMessageBox::m_nSpaceSpan , liIconY+5+rcInfo.bottom-rcInfo.top+2};
		CLabelUI* pLabel = RegiserLabel(IDC_STC_INFO, rect, m_sText);
		pLabel->SetTextColor(CUIMessageBox::m_colorInfoText);
		pLabel->SetTextStyle(DT_LEFT);
		pLabel->SetShowHtml();
	}
	//添加普通按钮
	//------------------------------------------------------------------------------------
	{
		int nBtnWidth = CUIMessageBox::m_nCommBtnWidth;
		int nBtnHeight = CUIMessageBox::m_nCommBtnHeight;
		int nBtnSpan = CUIMessageBox::m_nCommBtnSpan;
		int nY = nWinHeight-CUIMessageBox::m_nSpaceSpan-CUIMessageBox::m_nCommBtnHeight;

		if ((m_uType & 0X0000000F) == MB_OK)
		{
			int nX = (nWinWidth - nBtnWidth)/ 2;

			RECT rect = {nX, nY, nX + nBtnWidth , nY + nBtnHeight};

			CButtonUI* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("ok"));
			if (pBtn)
			{
				pBtn->SetName(_T("ok"));
				pBtn->SetButtonImage(RES( CUIMessageBox::m_nMidBtnImage));
				pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
					CUIMessageBox::m_colorBtnTextFocused,
					CUIMessageBox::m_colorBtnTextPushed,
					CUIMessageBox::m_colorBtnTextDisabled);
				//pBtn->SetCorner(CRect(5,5,5,5));;

			}
		}
		else if ((m_uType & 0X0000000F) ==  MB_OKCANCEL)
		{
			{
				int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("ok"));
				if (pBtn)
				{
					pBtn->SetName(_T("ok"));
					pBtn->SetButtonImage(RES(CUIMessageBox::m_nLeftBtnImage) );
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
					//pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}

			{
				int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2 + nBtnSpan + nBtnWidth;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_CANCEL, rect, _T("cancel"));
				if (pBtn)
				{
					pBtn->SetName(_T("cancel"));
					pBtn->SetButtonImage( RES(CUIMessageBox::m_nRightBtnImage));
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
				//	pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}

		}

		else if ((m_uType & 0X0000000F) == MB_YESNOCANCEL)
		{
			{
				int nX = (nWinWidth - nBtnWidth * 3 - nBtnSpan)/ 3;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_YES, rect, _T("yes"));
				if (pBtn)
				{
					pBtn->SetName(_T("ok"));
					pBtn->SetButtonImage(RES( CUIMessageBox::m_nLeftBtnImage));
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
					//pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}

			{
				int nX = (nWinWidth - nBtnWidth * 3 - nBtnSpan)/ 3 + nBtnSpan + nBtnWidth;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_NO, rect, _T("no"));
				if (pBtn)
				{
					pBtn->SetName(_T("cancel"));
					pBtn->SetButtonImage( RES(CUIMessageBox::m_nMidBtnImage));
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
					//pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}
			{
				int nX = (nWinWidth - nBtnWidth * 3 - nBtnSpan)/ 3 + nBtnSpan*2 + nBtnWidth*2;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_CANCEL, rect, _T("cancel"));
				if (pBtn)
				{
					pBtn->SetName(_T("cancel"));
					pBtn->SetButtonImage(RES( CUIMessageBox::m_nRightBtnImage));
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
					//pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}
		}


		else if ((m_uType & 0X0000000F) == MB_YESNO)
		{
			{
				int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_YES, rect, _T("yes"));
				if (pBtn)
				{
					pBtn->SetName(_T("ok"));
					pBtn->SetButtonImage(RES(CUIMessageBox::m_nLeftBtnImage) );
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
					//pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}

			{
				int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2 + nBtnSpan + nBtnWidth;
				RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				CButtonUI* pBtn = RegiserButton(IDC_BTN_NO, rect, _T("no"));
				if (pBtn)
				{
					pBtn->SetName(_T("cancel"));
					pBtn->SetButtonImage(RES( CUIMessageBox::m_nRightBtnImage));
					pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						CUIMessageBox::m_colorBtnTextFocused,
						CUIMessageBox::m_colorBtnTextPushed,
						CUIMessageBox::m_colorBtnTextDisabled);
				//	pBtn->SetCorner(CRect(5,5,5,5));;
				}
			}
		}
		else
		{
			int nX = (nWinWidth - nBtnWidth)/ 2;
			RECT rect = {nX, nY, nX + nBtnWidth , nY + nBtnHeight};

			CButtonUI* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("ok"));
			if (pBtn)
			{
				pBtn->SetName(_T("ok"));
				pBtn->SetButtonImage( RES(CUIMessageBox::m_nMidBtnImage));
				pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
					CUIMessageBox::m_colorBtnTextFocused,
					CUIMessageBox::m_colorBtnTextPushed,
					CUIMessageBox::m_colorBtnTextDisabled);
				//pBtn->SetCorner(CRect(5,5,5,5));;
			}
		}

	}

}

LPCTSTR CMessageBoxWnd::GetWindowClassName() const
{
   return _T("MessageBoxWnd");
}

void CMessageBoxWnd::OnFinalMessage(HWND hWnd)
{
   //delete this;
}

LRESULT CMessageBoxWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	int nCloseBtnWidth = CUIMessageBox::m_nCloseBtnWidth;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	rc.bottom = rc.top + 30;
	rc.right = rc.right - nCloseBtnWidth;

	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	::ScreenToClient(m_hWnd, &pt);

	if (PtInRect(&rc, pt))
	{
		bHandled = TRUE;
		return HTCAPTION;
	}
	else
	{
		return HTCLIENT;
	}
}

LRESULT CMessageBoxWnd::OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
   return CSkinDialog::OnWindowMessage(uMsg, wParam, lParam,bHandled);
}

void CMessageBoxWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetID() == IDC_BTN_OK )
		{
			Close();
			m_nRet = IDOK;
		}

		if( msg.pSender->GetID() == IDC_BTN_OK )
		{
			Close();
			m_nRet = IDOK;
		}

		if( msg.pSender->GetID() == IDC_BTN_CANCEL )
		{
			Close();
			m_nRet = IDCANCEL;
		}

		if( msg.pSender->GetID() == IDC_BTN_YES )
		{
			Close();
			m_nRet = IDYES;
		}

		if( msg.pSender->GetID() == IDC_BTN_NO )
		{
			Close();
			m_nRet = IDNO;
		}
		if( msg.pSender->GetID() == IDC_BTN_CLOSE )
		{
			Close();
			m_nRet = IDCANCEL;
		}
	}
}

////////////////////////////////////////////////////////


int CUIMessageBox::m_nCloseBtnPosToRightX = 0;
int CUIMessageBox::m_nCloseBtnPosToTopY = 0;

int CUIMessageBox::m_nTitleID=0;
int CUIMessageBox::m_nSpaceSpan=20;

int CUIMessageBox::m_nCommBtnWidth=80;
int CUIMessageBox::m_nCommBtnHeight=20;
int CUIMessageBox::m_nCommBtnSpan=10;
int CUIMessageBox::m_nDlgTemplate=0;
UINT CUIMessageBox::m_nCloseBtnImage=0;
UINT CUIMessageBox::m_nLeftBtnImage=0;
UINT CUIMessageBox::m_nRightBtnImage=0;
UINT CUIMessageBox::m_nMidBtnImage=0;
UINT CUIMessageBox::m_nBKImage=0;

int CUIMessageBox::m_nCloseBtnWidth=0;
int CUIMessageBox::m_nCloseBtnHeight=0;

HMODULE CUIMessageBox::m_hResModule=NULL;

COLORREF CUIMessageBox::m_colorBtnTextNormal=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextFocused=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextPushed=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextDisabled=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorInfoText = RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorTitleText = RGB(255, 255, 255);
int CUIMessageBox::m_nCornerSpan = 5;


void CUIMessageBox::SetCloseBtnSize(int aiX, int aiY)
{
	m_nCloseBtnWidth = aiX;
	m_nCloseBtnHeight = aiY;
}
void CUIMessageBox::SetCloseBtnImage(UINT uImage)
{
	m_nCloseBtnImage = uImage;
}
void CUIMessageBox::SetCloseBtnPos(int aiXToRight, int aiYToTop)
{
	m_nCloseBtnPosToRightX = aiXToRight;
	m_nCloseBtnPosToTopY = aiYToTop;
}

void CUIMessageBox::SetSpaceSpan( int aiSpan )
{
	m_nSpaceSpan=aiSpan;
}
void CUIMessageBox::SetCornerSpan(int nSpan)
{
	m_nCornerSpan = nSpan;
}

void CUIMessageBox::SetTitleID(UINT nTitleID)
{
	m_nTitleID = nTitleID;
}

void  CUIMessageBox::SetDlgTemplate(int nDlgTemplate)
{
	m_nDlgTemplate = nDlgTemplate;
}

 void CUIMessageBox::SetResModule(HMODULE hResModule)
 {
	 m_hResModule = hResModule;
 }

void CUIMessageBox::SetBtnTextColor(COLORREF crNormal, COLORREF crFocused, COLORREF crPushed, COLORREF crDisabled)
{
	m_colorBtnTextNormal=crNormal;
	m_colorBtnTextFocused=crFocused;
	m_colorBtnTextPushed=crFocused;
	m_colorBtnTextDisabled=crDisabled;
}

void CUIMessageBox::SetTitleTextColor(COLORREF color)
{
	m_colorTitleText = color;
}

void CUIMessageBox::SetInfoTextColor(COLORREF color)
{
	m_colorInfoText = color;
}
void CUIMessageBox::SetCommBtnSize( int aiWidth, int aiHeight )
{
	m_nCommBtnWidth=aiWidth;
	m_nCommBtnHeight=aiHeight;
}

void CUIMessageBox::SetBKImage(UINT uImage)
{
	m_nBKImage = uImage;
}

void CUIMessageBox::SetLeftImage(UINT uImage)
{
	m_nLeftBtnImage = uImage;
}

void CUIMessageBox::SetMidImage(UINT uImage)
{
	m_nMidBtnImage = uImage;
}

void CUIMessageBox::SetRightImage(UINT uImage)
{
	m_nRightBtnImage = uImage;
}

int CUIMessageBox::Show(HWND hWnd, 
				 LPCTSTR lpText,
				 LPCTSTR lpCaption,
				 UINT uType
				 )
{
	/*CMessageBoxWnd* pMsgWnd = new CMessageBoxWnd;
	if (pMsgWnd == NULL)
	{
		return IDOK;
	}*/

	//pMsgWnd->Init(hWnd, lpText, lpCaption, uType);

	//pMsgWnd->ShowModal();

	//delete pMsgWnd;

	CMessageBoxWnd dlg;
	dlg.Init(hWnd,lpText,lpCaption,uType);
	dlg.ShowModal(hWnd);

	int nRetValue = dlg.m_nRet;

	return nRetValue;
}
int  UIMessageBox(HWND hWnd, 
			 LPCTSTR lpText,
			 LPCTSTR lpCaption,
			 UINT uType
			 )
{
	return CUIMessageBox::Show(hWnd, lpText, lpCaption, uType);
}

int UIMessageBox(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	return CUIMessageBox::Show(NULL, lpText, lpCaption, uType);
}