#include "StdAfx.h"
#include "LabelImage.h"
#include "Gif.h"
CLabelImage::CLabelImage(void)
{
	m_pCurImage = NULL;
	m_pImage    = NULL;
	m_nCurFrame = 0;
}

CLabelImage::~CLabelImage(void)
{
}

LPCTSTR CLabelImage::GetClass() const
{
	return _T("LabelImage");
}
LPVOID CLabelImage::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("LabelImage")) == 0 ) return static_cast<CLabelImage*>(this);
	return CControlUI::GetInterface(pstrName);
}
UINT CLabelImage::GetControlFlags() const
{
	return 0;
}

void CLabelImage::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CControlUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_TIMER && IsEnabled() &&IsVisible())
	{
		if (event.wParam = ID_TIMER_FLASH)
		{
			m_nCurFrame ++ ;
		}
		Invalidate();
		return;
	}
}

void CLabelImage::SetImage(LPCTSTR strPath)
{
	m_strImagePath = strPath;
	
//	m_PlayGif.LoadGIF(strPath);

	m_pImage = Bitmap::FromFile(strPath);
	Invalidate();
}
void CLabelImage::SetImage(UINT    nResID,LPCTSTR strType /*= _T("PNG")*/)
{
	HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(nResID), strType);
	if( hResource == NULL ) return ;
	HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( hGlobal == NULL ) {
		FreeResource(hResource);
		return ;
	}

	DWORD dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( dwSize == 0 ) return;
	BYTE *pData = new BYTE[ dwSize +1];
	ZeroMemory(pData,dwSize +1);
	::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
	::FreeResource(hResource);

	HGLOBAL hBuffer = NULL;
	HBITMAP hBitmap = NULL;
	int x = 0;
	int y = 0;
	BOOL baph = FALSE;
	hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pData, dwSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pImage = Bitmap::FromStream(pStream);
				pStream->Release();
			}
		}
	}
	delete pData;
	Invalidate();
}

void CLabelImage::Play()
{
	this->SetTimer(ID_TIMER_FLASH,100);
}

void CLabelImage::Stop()
{
	this->KillTimer(ID_TIMER_FLASH);
}

void CLabelImage::PaintStatusImage(HDC hDC)
{
	if (!m_pImage)
	{
		return ;
	}
	CRect rc(0,0,0,0);
	CRect rcC(0,0,0,0);
	GUID    Guid = FrameDimensionTime;  

	UINT    nCount = m_pImage->GetFrameDimensionsCount();
	UINT    nCount1 = m_pImage->GetFrameCount(&Guid);
	if (m_nCurFrame>=nCount1)
	{
		m_nCurFrame = 0;
	}
	if (m_nCurFrame < nCount1)
	{
		m_pImage->SelectActiveFrame(&Guid,m_nCurFrame);  

		Graphics   gh(hDC);  
		RectF rcf(m_rcItem.left,m_rcItem.top,m_rcItem.right - m_rcItem.left,m_rcItem.bottom-m_rcItem.top);
		gh.DrawImage(m_pImage,rcf,0,0,m_pImage->GetWidth(),m_pImage->GetHeight(),UnitPixel);
	}

}

void CLabelImage::InitImage(CHAR * pBuff ,DWORD dSize)
{
}

// // DWORD  CLabelImage::DrawGIF(LPVOID l) 
// { 
// 	_sTHPar *par=(_sTHPar*)l; 
// 	int FrameCount=0; 
// 	long lPause=0; 
// 	HDC dc=::GetWindowDC(par->m_hWnd); 
// 	RECT rWin; 
// 	::GetClientRect(par->m_hWnd,&rWin); 
// 	RectF rF(rWin.left,rWin.top,rWin.right,rWin.bottom); 
// 	GUID    Guid = FrameDimensionTime;   
// 	par->m_Img->SelectActiveFrame(&Guid,FrameCount);   
// 	Graphics   gh(dc);    
// 	gh.DrawImage(par->m_Img,rF,0,0,par->m_Img->GetWidth(),par->m_Img->GetHeight(),UnitPixel);   
// 	DWORD dTick=GetTickCount(); 
// 	lPause = ((long*)par->m_item->value)[FrameCount]*10;   
// 	while(1) 
// 	{ 
// 		Sleep(1); 
// 		MSG msg; 
// 		if(PeekMessage(&msg,0,0,0,PM_REMOVE)) 
// 		{ 
// 			if(msg.message==WM_GIFSTOP) 
// 			{ 
// 				::ReleaseDC(par->m_hWnd,dc); 
// 				delete par->m_Img; 
// 				delete[] par->m_item; 
// 				delete par; 
// 				return 0; 
// 			} 
// 		} 
// 		else 
// 		{ 
// 			if(par->m_Count>1) 
// 			{ 
// 				if(GetTickCount()-dTick>=lPause) 
// 				{ 
// 					if ((FrameCount+1)==par->m_Count)   
// 					{    
// 						FrameCount=0;   
// 						par->m_Img->SelectActiveFrame(&Guid,0);   
// 					}   
// 					else   
// 					{   
// 						par->m_Img->SelectActiveFrame(&Guid,++FrameCount);   
// 					}   
// 					Graphics   gh(dc);    
// 					gh.DrawImage(par->m_Img,rF,0,0,par->m_Img->GetWidth(),par->m_Img->GetHeight(),UnitPixel);    
// 					lPause = ((long*)par->m_item->value)[FrameCount]*10;   
// 					dTick=GetTickCount(); 
// 				} 
// 			}    
// 		} 
// 	} 
// 	return 0; 
// } 