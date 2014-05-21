#include "StdAfx.h"
#include "BalloonTip.h"
#include "UIMessageBox.h"
#define ID_ELAPSE_SHOW 25
HWND CBalloonTip::m_hOnlyHwnd = NULL;
#define MAX(a,b) a>=b?a:b
CBalloonTip::CBalloonTip()   // standard constructor
{
	m_nFade = 0;
	m_nBK = 0;
	m_nClose = 0;
	m_nIcon = 0;
	m_uLinkMsg = 0;
	m_bLinkSendOnce = FALSE;
	m_dwMsgStyle = 0;
	m_btnTextCount = 0;
	m_dwRet = 1;
}
CBalloonTip::~CBalloonTip()
{

}

void CBalloonTip::InitControls()
{
	CLabelUI * pLab = NULL;
	CButtonUI* pBtn = NULL;
	CTextUI	 * pText= NULL; 

	SIZE sz;GetDrawTextSize(m_strInfo,sz);
	if (sz.cx<9999&&sz.cy<300)
	{

		SetWindowPos(GetHWND(),HWND_TOPMOST,0,0,MAX(sz.cx+60,260),sz.cy+((m_dwMsgStyle!=0)?90:50),SWP_NOMOVE);
	}
	CRect rc;
	GetClientRect(GetHWND(),&rc);
	int nWidth = rc.GetWidth();
	int nHeight = rc.GetHeight();
	m_pPM->AddFont(MSYH,16,FALSE,FALSE,FALSE);

	int s = m_pPM->GetFontIndex(MSYH,16,FALSE,FALSE,FALSE);
	pLab = RegiserLabel(ID_LAB_TITLE,CRect(15,8,200,25),m_strTitle);
	if (pLab)
	{
		pLab->SetTextColor(RGB(73,73,73));
		pLab->SetText(m_strTitle);
		pLab->SetShowHtml(TRUE);
	}

	int nInfoStart = (nWidth - sz.cx)/2;
	pText = (CTextUI*)RegiserControl(CONTROL(CTextUI),ID_LAB_INFO,CRect(nInfoStart ,35,nWidth,40 + sz.cy));
	if (pText)
	{
		pText->SetShowHtml(TRUE);
		pText->SetText(m_strInfo);
		pText->SetTextColor(RGB(73,73,73));
		pText->SetTextStyle(DT_VCENTER|DT_LEFT);
	}

	pBtn = RegiserButton(ID_BTN_COSE,CRect(nWidth-31,2,nWidth-2,27));
	if (pBtn)
	{
		pBtn->SetImage(m_nClose);
	}
	if (m_dwMsgStyle != 0)
	{
		if ((m_dwMsgStyle & 0X0000000F) == MB_YESNO)
		{
			
			CButtonUI* pBtn = RegiserButton(ID_BTN_YES, CRect(30,sz.cy+50,120,sz.cy+80), _T("yes"));
			if (pBtn)
			{
				pBtn->SetImage( CUIMessageBox::m_nLeftBtnImage);
				pBtn->SetCorner(CRect(5,5,5,5));
			}
			pBtn = RegiserButton(ID_BTN_NO, CRect(nWidth - 120,sz.cy+50,nWidth - 30,sz.cy+80), _T("no"));
			if (pBtn)
			{
				pBtn->SetImage( CUIMessageBox::m_nLeftBtnImage);
				pBtn->SetCorner(CRect(5,5,5,5));
			}

		}
	}
}

void CBalloonTip::GetDrawTextSize(LPCTSTR lpStr, SIZE &retSize)
{
	RECT rcInfo = { 0, 0, 9999, 300 };
	int nLinks = 0;
	CRenderEngine::DrawHtmlText(m_pPM->GetPaintDC(), m_pPM, rcInfo, lpStr, _INVALID_COLOR, NULL, NULL, nLinks,  DT_CALCRECT);
	retSize.cx = rcInfo.right - rcInfo.left;
	retSize.cy = rcInfo.bottom - rcInfo.top;
}

BOOL CBalloonTip::Initialization()
{
	SetSkinImage(m_nBK);
	SetSkinCorner(CRect(5,30,5,5));
	InitIconImage();
	InitControls();
	SetTimer(GetHWND(),ID_TIMER_START_SHOW,ID_ELAPSE_SHOW,NULL);
	if (m_nTitleID!=0)
	{
		SetTitleIcon(m_nTitleID);
	}
	SetForegroundWindow(GetHWND());
	return TRUE;
}

void CBalloonTip::SetImage(int nbk,int nClose,int nIcon)
{
	m_nBK = nbk;
	m_nClose = nClose;
	m_nIcon = nIcon;

}

void CBalloonTip::InitIconImage()
{
	HICON hIcon = LoadIcon(CPaintManagerUI::GetResourceDll(),MAKEINTRESOURCE(m_nIcon));
	if (hIcon)
	{
		ICONINFO icInfo = { 0 };
		if (::GetIconInfo(hIcon, &icInfo))
		{
			BITMAP bitmap; 
			GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

			Bitmap* pBitmap = NULL;
			Bitmap* pWrapBitmap = NULL;
			if (bitmap.bmBitsPixel != 32)
			{   
				pBitmap = Bitmap::FromHICON(hIcon);
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

			//lpIconImage = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat32bppARGB);

			HBITMAP hBit ;

			pBitmap->GetHBITMAP(ARGB(0,0,0,0),&hBit);

			m_pPM->AddImage(_T("BalloonIcon"),hBit,pBitmap->GetWidth(),pBitmap->GetHeight(),TRUE);

			DeleteObject(icInfo.hbmColor); 
			DeleteObject(icInfo.hbmMask); 
		}
		DeleteObject(hIcon);
	}
}

void CBalloonTip::SetTitleID(UINT nID)
{
	m_nTitleID = nID;
}

DWORD CBalloonTip::BalloonPopup(HWND hParent,LPCTSTR pText,UINT uTitle,int nX,int nY,UINT uMsg)
{
	m_nTitleID = uTitle;
	m_strInfo  = pText;
	m_hWndMsg = hParent;
	m_uLinkMsg = uMsg;
	if (CBalloonTip::m_hOnlyHwnd&&IsWindow(CBalloonTip::m_hOnlyHwnd))
	{
		::SendMessage(CBalloonTip::m_hOnlyHwnd,WM_CLOSE,0,0);
	}
	Create(GetDesktopWindow(),_T("Balloon"),WS_POPUP,WS_EX_LAYERED|WS_EX_TOOLWINDOW,0,0,nX,nY);
	if(!::IsWindow(m_hWnd))
	{
		return FALSE;
	}
	CBalloonTip::m_hOnlyHwnd = m_hWnd;
	//SetWindowPos(m_hWnd,NULL,0,0,nX,nY,SWP_NOMOVE|SWP_NOACTIVATE);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
		}
		if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	return m_dwRet;
}

void CBalloonTip::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click"))
	{
		UINT luSenderId=msg.pSender->GetID();
		if (luSenderId == ID_BTN_COSE)
		{
			m_dwRet = IDNO;
			OnTipDisappered();
		}
		if (luSenderId == ID_BTN_YES)
		{
			m_dwRet = IDYES;
			OnTipDisappered();
		}
		if (luSenderId == ID_BTN_NO)
		{
			m_dwRet = IDNO;
			OnTipDisappered();
		}
	}
	else if (msg.sType == _T("link"))
	{
		UINT luSenderId=msg.pSender->GetID();
		if (luSenderId == ID_LAB_INFO&&!m_bLinkSendOnce)
		{
			m_bLinkSendOnce = TRUE;
			::PostMessage(m_hWndMsg,WM_BALLOON_MSG,m_uLinkMsg,msg.wParam);
		}
	}
}

LRESULT CBalloonTip::OnTimer(UINT nevent)
{
	if (nevent == ID_TIMER_HIDE_BF_TIP)
	{
		KillTimer(GetHWND(),ID_TIMER_HIDE_BF_TIP);
		SetTimer(GetHWND(),ID_TIMER_START_HIDE,ID_NELAPSE_TIMER,NULL);
	}
	else if (nevent == ID_TIMER_SHOW_BF_TIP)
	{

	}
	else if (nevent == ID_TIMER_BTN_TEXT)
	{
		m_btnTextCount--;
		CButtonUI * pbtn = dynamic_cast<CButtonUI *>(FindControl(ID_BTN_NO));
		if(pbtn)
		{
			CString strText;strText.Format(_T("%s(%d)"),_T("no"),m_btnTextCount);
			pbtn->SetText(strText);
		}
		if (m_btnTextCount <=0)
		{
			KillTimer(GetHWND(),ID_TIMER_BTN_TEXT);
			m_dwRet = IDNO;
			SetTimer(GetHWND(),ID_TIMER_HIDE_BF_TIP,10,NULL);
		}
	}
	else if (nevent == ID_TIMER_START_SHOW)  //½¥ÏÔ
	{
		m_nFade = (m_nFade+20)<=255?(m_nFade+20):255;
		if (m_nFade >= 255)
		{
			m_nFade = 255;
			KillTimer(GetHWND(),ID_TIMER_START_SHOW);
			
			if (m_dwMsgStyle!=0)
			{
				if ((m_dwMsgStyle & 0X0000000F) == MB_YESNO)
				{
					m_btnTextCount = 11;
					SetTimer(GetHWND(),ID_TIMER_BTN_TEXT,1000,NULL);
				}
			}
			else
			{
				SetTimer(GetHWND(),ID_TIMER_HIDE_BF_TIP,10000,NULL);
			}
		}
		ShowWindow(SW_SHOW);
	}
	else if (nevent == ID_TIMER_START_HIDE)  //½¥Òþ
	{
		KillTimer(GetHWND(),ID_TIMER_START_SHOW);
		m_nFade = (m_nFade-20)>=0?(m_nFade-20):0;
		if (m_nFade <= 0)
		{
			m_nFade = 0;
			KillTimer(GetHWND(),ID_TIMER_START_HIDE);
			ShowWindow(SW_HIDE);
			OnTipDisappered();
		}
	}
	UpdateWindow();
	return 0 ;
}

void CBalloonTip::UpdateWindow()
{
	CRect rc;
	GetClientRect(GetHWND(),&rc);

	int nBorder = GetSystemMetrics(SM_CXFRAME);
	int nWidth = GetSystemMetrics(SM_CXMAXIMIZED)- 2*nBorder;
	int nHeight= GetSystemMetrics(SM_CYMAXIMIZED)- 2*nBorder;

	int nClientWidth = rc.GetWidth();// m_BKImageBig->GetWidth() ;
	int nClientHeight = rc.GetHeight();//m_BKImageBig->GetHeight() ;

	int bTopPos = (30*(255-m_nFade))/255;
	SetWindowPos(GetHWND(),NULL,nWidth-nClientWidth,nHeight-nClientHeight+bTopPos,nClientWidth,nClientHeight,SWP_NOZORDER|SWP_NOACTIVATE);

	SetLayeredWindowAttributes(GetHWND(), NULL, m_nFade, LWA_ALPHA);

}
void CBalloonTip::OnTipDisappered()
{
	Close();
}
void CBalloonTip::SetMessageStyle(DWORD dwStyle)
{
	m_dwMsgStyle = dwStyle;
}
LRESULT CBalloonTip::OnWindowMessage(UINT uMsg,WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (uMsg == WM_TIMER)
	{
		int nEventID = wParam;
		OnTimer(nEventID);
	}
	else if (uMsg == WM_CLOSE)
	{

	}
	return CSkinDialog::OnWindowMessage(uMsg,wParam,lParam,bHandled);
}