#include "StdAfx.h"
#include "MutiTip.h"

CMutiTip::CMutiTip(void)
{
	m_uCloseImage = 0;
	m_uBkImage = 0;
	m_uTriangleImage = 0;
	m_uNoNextImage = 0;
	m_bCloseVisable = NULL;
	m_bTriangleVisable = NULL;
	m_bNoNextVisable = NULL;
	m_bAutoHeight = NULL;

	m_nFade = 255;
	m_uCloseBtnState = 0;
	m_uNoNextBtnState = 0;
	m_bNoNextChecked = FALSE;
	m_bAutoWidth = FALSE;
	m_rcCorner = CRect(0,0,0,0);
	m_pPM      = NULL;
}

CMutiTip::~CMutiTip(void)
{
	if (m_pPM)
	{
		delete m_pPM;
		m_pPM = NULL;
	}
}

void CMutiTip::SetCloseImage(RECT RcCloseBtn,UINT pCloseImage,BOOL bVisable)
{
	m_uCloseImage = pCloseImage;
	m_rcCloseImage = RcCloseBtn;
	m_bCloseVisable = bVisable;
}
void CMutiTip::SetBkImage(UINT pBkImage)
{
	m_uBkImage = pBkImage;
}

void CMutiTip::SetNoNextTimeImage(RECT RcNoNextImage,UINT pNoNextImage,LPCTSTR pStr,BOOL bVisable)
{
	m_uNoNextImage = pNoNextImage;
	m_rcNoNextImage = RcNoNextImage;
	m_bNoNextVisable = bVisable;
	m_sNoNextKey = pStr;
}
void CMutiTip::SetTriangleImage(RECT RcTriangle,UINT pTriangleImage,BOOL bVisable)
{
	m_uTriangleImage = pTriangleImage;
	m_rcTriangleImage = RcTriangle;
	m_bTriangleVisable = bVisable;
}

UINT CMutiTip::CalculateTextHeight(TipText &sTipText,UINT nWidth)
{
	UINT nNeedHeight = 0;
	HDC hDC = ::GetDC(GetHWND());
	if (hDC)
	{
		CString sText = sTipText.strText;
		sText.Replace(_T("<n>"),_T("\r\n"));
		LPCTSTR pstrText = sText;
		CRect rc(0,0,nWidth,9999);

		TEXTMETRIC tm = m_pPM->GetDefaultFontInfo()->tm;
		Graphics Gr(hDC);
		RectF rcin(rc.left,rc.top,rc.GetWidth(),rc.GetHeight()),rcout;
		StringFormat drawFormat;
		drawFormat.SetAlignment(StringAlignmentNear);   
		int leng,line;
		BOOL bDeleteFont = FALSE;
		if (sTipText.hFont == NULL)
		{
			sTipText.hFont=m_pPM->GetDefaultFontInfo()->hFont;
			bDeleteFont = FALSE;
		}
		Gdiplus::Font font(hDC, sTipText.hFont);
		Gr.MeasureString(sText,sText.GetLength(),&font,rcin,&drawFormat,&rcout,&leng,&line);
		nNeedHeight = rcout.Height;
		if (bDeleteFont)
		{
			DeleteObject(sTipText.hFont);
			sTipText.hFont = NULL;
		}
		ReleaseDC(GetHWND(),hDC);
		DeleteDC(hDC);
	}
	return nNeedHeight;
}

UINT CMutiTip::CalculateTextWidth(TipText &pStrText)
{
	UINT nNeedWidth = 0;
	HDC hDC = ::GetDC(GetHWND());
	if (hDC)
	{
		CString sText = pStrText.strText;
		sText.Replace(_T("<n>"),_T("\r\n"));
		LPCTSTR pstrText = sText;
		CRect rc(0,0,9999,9999);

		TEXTMETRIC tm = m_pPM->GetDefaultFontInfo()->tm;
		Graphics Gr(hDC);
		RectF rcin(rc.left,rc.top,rc.GetWidth(),rc.GetHeight()),rcout;
		StringFormat drawFormat;
		drawFormat.SetAlignment(StringAlignmentNear);   
		int leng,line;
		BOOL bDeleteFont = FALSE;
		if (pStrText.hFont == NULL)
		{
			pStrText.hFont=m_pPM->GetDefaultFontInfo()->hFont;
			bDeleteFont = FALSE;
		}
		Gdiplus::Font font(hDC, pStrText.hFont);
		Gr.MeasureString(sText,sText.GetLength(),&font,rcin,&drawFormat,&rcout,&leng,&line);
		if (rcout.Width)
		{
			nNeedWidth = rcout.Width;
		}
		if (bDeleteFont)
		{
			DeleteObject(pStrText.hFont);
			pStrText.hFont = NULL;
		}
		ReleaseDC(GetHWND(),hDC);
		DeleteDC(hDC);
	}
	return nNeedWidth;
}

void CMutiTip::ShowTip(INT nX,INT nY,INT nW,INT nH,BOOL bCenterPos)
{
	if (IsWindow(GetHWND()))
	{
		ShowWindow(FALSE,FALSE);
		if (!m_pPM->GetPaintWindow())
		{
			m_pPM->Init(GetHWND());
			m_pPM->SetDefaultSelectedBkColor(ARGB(254,0,0,0));
		}
		BOOL bNeedUpdate = FALSE;
		if (m_bAutoWidth)
		{
			int nMostLong = 0;
			vector<TIPTEXT>::iterator iit  = m_vTipText.begin();
			for (;iit!=m_vTipText.end();iit++)
			{
				TIPTEXT *tt = &(*iit);
				int nLen = CalculateTextWidth(*tt);
				if(nLen>nMostLong)
				{
					nMostLong = nLen;
				}
			}
			if (nMostLong!=0)
			{
				int nWndX = nX;
				int nWndY = nY;
				if (bCenterPos)
				{
					nWndX  = nWndX - (nW + nMostLong)/2;
					nWndY  = nWndY - nH;
				}

				SetWindowPos(GetHWND(),NULL,nWndX,nWndY,nW + nMostLong,nH,SWP_NOACTIVATE|SWP_NOZORDER);
			}
		}
		else if (m_bAutoHeight)
		{
			UINT nTextTop = 0;
			UINT nTextButtom = 0;
			UINT nHeightExpend = 0;
			
			vector<TIPTEXT>::iterator iit  = m_vTipText.begin();
			for (;iit!=m_vTipText.end();iit++)
			{
				TIPTEXT *tt = &(*iit);
				UINT nHeight = CalculateTextHeight(*tt,tt->rcText.GetWidth());
				UINT nPerHeight =  nHeightExpend;
				if (nHeight>tt->rcText.GetHeight())
				{
					nHeightExpend +=(nHeight - tt->rcText.GetHeight());
					//tt->rcText.bottom = tt->rcText.top + nHeight;
				}
				tt->rcText.top += nPerHeight;
				tt->rcText.bottom = tt->rcText.top + nHeight;
			}
			int nWndX = nX;
			int nWndY = nY;
			if (bCenterPos)
			{
				nWndX  = nWndX - (nW)/2;
				nWndY  = nWndY - nH+nHeightExpend;
			}

			SetWindowPos(GetHWND(),NULL,nX,nY,nW,nH+nHeightExpend,SWP_NOACTIVATE|SWP_NOZORDER);
			
			bNeedUpdate = TRUE;
		}
		else
			SetWindowPos(GetHWND(),NULL,nX,nY,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		KillTimer(GetHWND(),ID_TIMER_HIDE);
		m_nFade = 255;
		UpdataImage();
		
		ShowWindow(TRUE,FALSE);
	}
}

void CMutiTip::SetBKImageCorner(CRect rc)
{
	m_rcCorner = rc;
}

void CMutiTip::SetCheckBoxSize(SIZE sz)
{
	m_CheckBoxSize = sz;
}

void CMutiTip::UpdataImage()
{
	HDC hMemDC = NULL;
	HDC pDC = ::GetDC(GetHWND());
	if (!pDC)
	{
		return;
	}
	CRect rc;
	GetClientRect(GetHWND(),&rc);
	hMemDC = CreateCompatibleDC(pDC);

	BYTE                           * pBits ;
	BITMAPINFOHEADER          bmih;
	ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );

	bmih.biSize                  = sizeof (BITMAPINFOHEADER) ;
	bmih.biWidth                 = rc.GetWidth() ;
	bmih.biHeight                = rc.GetHeight();
	bmih.biPlanes                = 1 ;
	bmih.biBitCount              = 32;								//这里一定要是32
	bmih.biCompression           = BI_RGB ;
	bmih.biSizeImage             = 0 ;
	bmih.biXPelsPerMeter         = 0 ;
	bmih.biYPelsPerMeter         = 0 ;
	bmih.biClrUsed               = 0 ;
	bmih.biClrImportant          = 0 ;

	HBITMAP bitmap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, NULL, NULL, 0);

	SelectBitmap(hMemDC,bitmap);

	BLENDFUNCTION blend = {0};
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = m_nFade<0?0:(m_nFade>255?255:m_nFade);

	//CRenderEngine::DrawGradient(hMemDC,rc,)
	if (m_uBkImage)
	{
		CString strCorner ; strCorner.Format(_T("corner='%d,%d,%d,%d'"),m_rcCorner.left,m_rcCorner.top,m_rcCorner.right,m_rcCorner.bottom);
		CRenderEngine::DrawImageString(hMemDC,m_pPM, rc, rc, RES(m_uBkImage) , strCorner);
	}
	if (m_uCloseImage&&m_bCloseVisable)
	{
		UINT nLeft,nRight;
		if (m_uCloseBtnState == 1) {nLeft = 0;nRight = 25;}
		if (m_uCloseBtnState == 2) {nLeft = 25;nRight = 50;}
		if (m_uCloseBtnState == 3) {nLeft = 50;nRight = 75;}
		if (m_uCloseBtnState == 4) {nLeft = 75;nRight = 100;}
		CString strModify;strModify.Format(_T("source='%d,%d,%d,%d' sourcePersent='TRUE'"),nLeft,0,nRight,100);
		CRenderEngine::DrawImageString(hMemDC,m_pPM, m_rcCloseImage, rc, RES(m_uCloseImage), strModify);
	}
	if (m_uNoNextImage&&m_bNoNextChecked)
	{
		UINT nLeft,nRight,nTop,nButtom,nTopBottomGap = 0;
		CString strDest;
		if (m_bNoNextChecked){nTop = 50;nButtom = 100;}
		else {nTop = 0;nButtom = 50;}
		if (m_uNoNextBtnState == 1) {nLeft = 0;nRight = 25;}
		if (m_uNoNextBtnState == 2) {nLeft = 25;nRight = 50;}
		if (m_uNoNextBtnState == 3) {nLeft = 50;nRight = 75;}
		if (m_uNoNextBtnState == 4) {nLeft = 75;nRight = 100;}

		if (m_CheckBoxSize.cx!=0&&m_CheckBoxSize.cy!=0)
		{
			nTopBottomGap = (m_rcNoNextImage.right -m_rcNoNextImage.left  - m_CheckBoxSize.cy)/2;
			strDest.Format(_T("dest='%d,%d,%d,%d'"),nTopBottomGap,nTopBottomGap,nTopBottomGap+m_CheckBoxSize.cx,nTopBottomGap+m_CheckBoxSize.cy);
		}

		CString strModify;strModify.Format(_T("%s source='%d,%d,%d,%d' sourcePersent='TRUE'"),strDest,nLeft,nTop,nRight,nButtom);
		CRenderEngine::DrawImageString(hMemDC,m_pPM, m_rcNoNextImage, rc,RES(m_uNoNextImage), strModify);
		CRect rcText = m_rcNoNextImage;
		rcText.left +=nTopBottomGap+m_CheckBoxSize.cx*2;
		int nL = 0;
		CRenderEngine::DrawHtmlText(hMemDC,m_pPM,rcText,_T("no_more_next"),RGB(0,0,0),NULL,NULL,nL,DT_LEFT);
	}
	if (m_uTriangleImage&&m_bTriangleVisable)
	{
		CRenderEngine::DrawImageString(hMemDC,m_pPM, m_rcTriangleImage, rc,RES(m_uTriangleImage) , _T(""));
	}
	vector<TipImage>::iterator it  = m_vTipImage.begin();
	for (;it!=m_vTipImage.end();it++)
	{
		TipImage tt = *it;
		CString strModify;strModify.Format(_T("restype='%s' fade='254'"),tt.sType);
		if (tt.nPosX!=-1&&tt.nPosY!=-1)
		{
			CRect rcImage;
			rcImage.left = tt.nPosX*rc.GetWidth()/100 - tt.nPosX*tt.rcImage.right/100;
			rcImage.right = rcImage.left + tt.rcImage.right;
			rcImage.top  = tt.nPosY*rc.GetHeight()/100 - tt.nPosY*tt.rcImage.bottom/100;
			rcImage.bottom  = rcImage.top + tt.rcImage.bottom;

			CRenderEngine::DrawImageString(hMemDC,m_pPM,rcImage,rc,RES(tt.uTipImage),strModify);
		}
		else
			CRenderEngine::DrawImageString(hMemDC,m_pPM,tt.rcImage,rc,RES(tt.uTipImage),strModify);
	}

	vector<TIPTEXT>::iterator iit  = m_vTipText.begin();
	for (;iit!=m_vTipText.end();iit++)
	{
		Graphics g(hMemDC);
		TIPTEXT tt = *iit;
		CString sText = tt.strText;
		sText.Replace(_T("<n>"),_T("\r\n"));
		RectF rcin(tt.rcText.left,tt.rcText.top,tt.rcText.GetWidth(),tt.rcText.GetHeight()),rcout;
		StringFormat drawFormat;
		drawFormat.SetAlignment((StringAlignment)tt.dStyle);
		BOOL bDeleteFont = FALSE;
		if (tt.hFont == NULL)
		{
			tt.hFont=(HFONT)m_pPM->GetDefaultFontInfo()->hFont;
			bDeleteFont = FALSE;
		}
		Gdiplus::Font font(hMemDC,tt.hFont);
		Gdiplus::RectF rcc(tt.rcText.left,tt.rcText.top,tt.rcText.GetWidth(),tt.rcText.GetHeight());;
		SolidBrush brush(tt.cText);
		g.DrawString(sText,sText.GetLength(),&font,rcc,&drawFormat , &brush);	
		if (bDeleteFont)
		{
			DeleteObject(tt.hFont);
			tt.hFont = NULL;
		}
	}
	CPoint ptDst(0, 0);
	CPoint ptWinPos(rc.left, rc.top);

	ClientToScreen(GetHWND(),&ptWinPos);
	CSize size(rc.right - rc.left, rc.bottom - rc.top);
	
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle ^ 0x80000);

	BOOL ret = ::UpdateLayeredWindow(m_hWnd,pDC, &ptWinPos, &size, hMemDC, &ptDst, 0, &blend, ULW_ALPHA);
	DeleteObject(bitmap);
	ReleaseDC(GetHWND(),hMemDC);
	ReleaseDC(GetHWND(),pDC);
	DeleteDC(hMemDC);
	DeleteDC(pDC);
	return ;
}

void CMutiTip::AlphaText(HDC hdc,CRect rc,CRect rcText)
{
	HBITMAP hbitmap = ::CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	//::SelectObject(hdc, hbitmap);

	BITMAP bmptemp = {0};
	if (!::GetObject(hbitmap, sizeof(BITMAP), (LPBYTE)&bmptemp))   
		return ;  
	int imgSize = bmptemp.bmBitsPixel/8; //兼容24bit和32bit位图
	BITMAPINFO bi;   
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);   
	bi.bmiHeader.biWidth = bmptemp.bmWidth;   
	bi.bmiHeader.biHeight = bmptemp.bmHeight;   
	bi.bmiHeader.biPlanes = 1;   
	bi.bmiHeader.biBitCount = bmptemp.bmBitsPixel;    
	bi.bmiHeader.biCompression = BI_RGB;    
	bi.bmiHeader.biSizeImage = bmptemp.bmWidth * imgSize * bmptemp.bmHeight; 
	bi.bmiHeader.biClrUsed = 0;   
	bi.bmiHeader.biClrImportant = 0;   
	BYTE* lpvBits = new BYTE[bi.bmiHeader.biSizeImage]; 
	::GetDIBits(hdc, hbitmap, 0, bmptemp.bmHeight, lpvBits, &bi, DIB_RGB_COLORS);
	for (int i = 0 ; i < rc.GetWidth() ; i ++ )
	{
		for (int j = 0 ; j< rc.GetHeight()  ; j++)
		{
			if (PtInRect(&rcText,CPoint(i,j)))
			{
				//lpvBits[j * rc.GetWidth() * 4  + j * 4 + 3]	= 254;
			}
			
		}
	}
	::SetDIBits(hdc, hbitmap, 0, rc.GetHeight(), lpvBits, &bi, DIB_RGB_COLORS);
	//SelectBitmap(hdc,hbitmap);
	//DeleteObject(bitmap);

}

void CMutiTip::HideTip()
{
	SetTimer(GetHWND(),ID_TIMER_HIDE,30,NULL);
}

void CMutiTip::SetAutoHeight(BOOL bAuto)									// 目前只适用于出装推荐的 装备说明
{
	m_bAutoHeight = bAuto;
}

void CMutiTip::SetAutoWidth(BOOL bAuto)
{
	m_bAutoWidth = bAuto;
}

void CMutiTip::SetTexts(TipText* pTipTexts,UINT nCount)
{
	m_vTipText.clear();
	for (int i = 0 ; i<nCount ; i++)
	{
		TipText tt = pTipTexts[i];
		m_vTipText.push_back(tt);
	}
}
void CMutiTip::SetImages(TipImage* pTipImages,UINT nCount)
{
	m_vTipImage.clear();
	for (int i = 0 ; i<nCount ; i++)
	{
		TipImage tt = pTipImages[i];
		m_vTipImage.push_back(tt);
	}
}


LRESULT CMutiTip::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			m_pPM = new CPaintManagerUI;
		}break;

	case WM_TIMER:
		{
			if (wParam == ID_TIMER_HIDE)
			{
				m_nFade-=30;
				if (m_nFade<=0)
				{
					m_nFade = 0;
					KillTimer(GetHWND(),ID_TIMER_HIDE);
					ShowWindow(FALSE);
				}
				UpdataImage();
			}
			
		}break;
	}

	return CWindowWnd::HandleMessage(uMsg,wParam,lParam);
}