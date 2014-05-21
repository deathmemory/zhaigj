#include "StdAfx.h"
#include <zmouse.h>

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT MapKeyState()
{
    UINT uState = 0;
    if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
    if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
    if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
    if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
    if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
    return uState;
}

typedef struct tagFINDTABINFO
{
    CControlUI* pFocus;
    CControlUI* pLast;
    BOOL bForward;
    BOOL bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
    TCHAR ch;
    BOOL bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
    CControlUI* pSender;
    UINT nLocalID;
    HWND hWnd;
    UINT uWinTimer;
    BOOL bKilled;
} TIMERINFO;


/////////////////////////////////////////////////////////////////////////////////////

HPEN m_hUpdateRectPen = NULL;
HINSTANCE CPaintManagerUI::m_hInstance = NULL;
HINSTANCE CPaintManagerUI::m_hResourceInstance = NULL;
CString CPaintManagerUI::m_pStrResourcePath;
CString CPaintManagerUI::m_pStrResourceZip;
CString CPaintManagerUI::m_sDefaultResType;

short CPaintManagerUI::m_H = 180;
short CPaintManagerUI::m_S = 100;
short CPaintManagerUI::m_L = 100;
CStdPtrArray CPaintManagerUI::m_aPreMessages;


CPaintManagerUI::CPaintManagerUI() :
m_hWndPaint(NULL),
m_hDcPaint(NULL),
m_hDcOffscreen(NULL),
m_hDcBackground(NULL),
m_hbmpOffscreen(NULL),
m_hbmpBackground(NULL),
m_hwndTooltip(NULL),
m_bShowUpdateRect(FALSE),
m_uTimerID(0x1000),
m_pRoot(NULL),
m_pFocus(NULL),
m_pEventHover(NULL),
m_pEventClick(NULL),
m_pEventKey(NULL),
m_bFirstLayout(TRUE),
m_bFocusNeeded(FALSE),
m_bUpdateNeeded(FALSE),
m_bMouseTracking(FALSE),
m_bMouseCapture(FALSE),
m_bOffscreenPaint(TRUE),
m_bAlphaBackground(FALSE),
m_pParentResourcePM(NULL),
m_bFakeWnd(FALSE)
{
    m_dwDefaultDisabledColor = 0xFFA7A6AA;
    m_dwDefaultFontColor = 0xFF000000;
    m_dwDefaultLinkFontColor = 0xFF000000;
    m_dwDefaultLinkHoverFontColor = 0xFF000000;
    m_dwDefaultSelectedBkColor = 0xFFBAE4FF;
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -12;
	lf.lfWeight = 0;
	StrCpy(lf.lfFaceName,_T("SimSun\0"));
//	MessageBox(NULL,lf.lfFaceName,0,0);
    HFONT hDefaultFont = ::CreateFontIndirect(&lf);
    m_DefaultFontInfo.hFont = hDefaultFont;
//    m_DefaultFontInfo.sFontName = lf.lfFaceName;
	StrCpy( m_DefaultFontInfo.sFontName , lf.lfFaceName);
    m_DefaultFontInfo.iSize = -lf.lfHeight;
    m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
    m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
    m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
    ::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));

    if( m_hUpdateRectPen == NULL ) {
        m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
        // Boot Windows Common Controls (for the ToolTip control)
        ::InitCommonControls();
        ::LoadLibrary(_T("msimg32.dll"));
    }

    m_szMinWindow.cx = 0;
    m_szMinWindow.cy = 0;
    m_szMaxWindow.cx = 0;
    m_szMaxWindow.cy = 0;
    m_szInitWindowSize.cx = 0;
    m_szInitWindowSize.cy = 0;
    m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
    ::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
    ::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
}

CPaintManagerUI::~CPaintManagerUI()
{
    // Delete the control-tree structures
    for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
    m_mNameHash.Resize(0);
    delete m_pRoot;

    ::DeleteObject(m_DefaultFontInfo.hFont);
    RemoveAllFonts();
    RemoveAllImages();
    RemoveAllDefaultAttributeList();
    RemoveAllOptionGroups();
    RemoveAllTimers();

    // Reset other parts...
    if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
    if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
    if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
    if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
    if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
    if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
    CPaintManagerUI::m_aPreMessages.Remove(m_aPreMessages.Find(this));
}

void CPaintManagerUI::Init(HWND hWnd,BOOL bFakeWnd)
{
    ASSERT(::IsWindow(hWnd));

	m_bFakeWnd = bFakeWnd;
    // Remember the window context we came from
    m_hWndPaint = hWnd;
    m_hDcPaint = ::GetDC(hWnd);
    // We'll want to filter messages globally too
    m_aPreMessages.Add(this);
}



HINSTANCE CPaintManagerUI::GetInstance()
{
    return m_hInstance;
}

CString CPaintManagerUI::GetInstancePath()
{
    if( m_hInstance == NULL ) return _T('\0');
    
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
    CString sInstancePath = tszModule;
    int pos = sInstancePath.ReverseFind(_T('\\'));
    if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
    return sInstancePath;
}

CString CPaintManagerUI::GetCurrentPath()
{
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetCurrentDirectory(MAX_PATH, tszModule);
    return tszModule;
}

HINSTANCE CPaintManagerUI::GetResourceDll()
{
    if( m_hResourceInstance == NULL ) return m_hInstance;
    return m_hResourceInstance;
}

LPCTSTR CPaintManagerUI::GetDefaultResType()
{
	return m_sDefaultResType;
}

const CString& CPaintManagerUI::GetResourcePath()
{
    return m_pStrResourcePath;
}

const CString& CPaintManagerUI::GetResourceZip()
{
    return m_pStrResourceZip;
}

void CPaintManagerUI::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}

void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
{
    ::SetCurrentDirectory(pStrPath);
}

void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
{
    m_hResourceInstance = hInst;
}

void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
{
    m_pStrResourcePath = pStrPath;
    if( m_pStrResourcePath.IsEmpty() ) return;
    TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
    if( cEnd != _T('\\') && cEnd != _T('/') ) m_pStrResourcePath += _T('\\');
}

void CPaintManagerUI::SetResourceZip(LPCTSTR pStrPath)
{
    m_pStrResourceZip = pStrPath;
}

void CPaintManagerUI::SetDefaultResType(LPCTSTR pstrType)
{
	m_sDefaultResType = pstrType;
}

void CPaintManagerUI::GetHSL(short* H, short* S, short* L)
{
    *H = m_H;
    *S = m_S;
    *L = m_L;
}

void CPaintManagerUI::SetHSL(BOOL bUseHSL, short H, short S, short L)
{
    if( H == m_H && S == m_S && L == m_L ) return;
    m_H = CLAMP(H, 0, 360);
    m_S = CLAMP(S, 0, 200);
    m_L = CLAMP(L, 0, 200);
    for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
        CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
        if( pManager != NULL && pManager->GetRoot() != NULL )
            pManager->GetRoot()->Invalidate();
    }
}

void CPaintManagerUI::ReloadSkin()
{
    for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
        CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
        pManager->ReloadAllImages();
    }
}

TImageInfo* CPaintManagerUI::SetImage(UINT nID,HMODULE hModule,LPCTSTR strImageName)
{
	TImageInfo* data = NULL;
	CString type = CPaintManagerUI::GetDefaultResType();
	data = CRenderEngine::LoadImage(nID,type , hModule);

	if( !data ) return NULL;
	if( !type.IsEmpty() ) 
	{
		StrCpy(data->sResType,type.GetString());
	}
	data->dwMask = 0;
	if( !m_mImageHash.Insert(strImageName, data) ) {
		::DeleteObject(data->hBitmap);
		delete data;
	}

	return data;
}

TImageInfo* CPaintManagerUI::SetImage(HBITMAP hBitMap,UINT nX,UINT nY,BOOL bAph,LPCTSTR strImageName)
{
	if (hBitMap)
	{
		TImageInfo *pBitMap = new TImageInfo;
		pBitMap->alphaChannel = bAph;
		pBitMap->hBitmap = hBitMap;
		pBitMap->dwMask = 0;
		pBitMap->nX = nX;
		pBitMap->nY = nY;
		if( !m_mImageHash.Insert(strImageName, pBitMap) ) {
			::DeleteObject(pBitMap->hBitmap);
			delete pBitMap;
			pBitMap = NULL;
		}
		return pBitMap;
	}
	return NULL;
}

TImageInfo* CPaintManagerUI::SetImage(UINT nID,HMODULE hModule,LPCTSTR sType,LPCTSTR strImageName)
{
	TImageInfo* data = NULL;
	CString type = sType;
	data = CRenderEngine::LoadImage(nID,type , hModule);

	if( !data ) return NULL;
	if( !type.IsEmpty() ) 
	{
		StrCpy(data->sResType,type.GetString());
	}
	data->dwMask = 0;
	if( !m_mImageHash.Insert(strImageName, data) ) {
		::DeleteObject(data->hBitmap);
		delete data;
	}
	return data;
}

HWND CPaintManagerUI::GetPaintWindow() const
{
    return m_hWndPaint;
}

HWND CPaintManagerUI::GetTooltipWindow() const
{
    return m_hwndTooltip;
}

HDC CPaintManagerUI::GetPaintDC() const
{
    return m_hDcPaint;
}

POINT CPaintManagerUI::GetMousePos() const
{
    return m_ptLastMousePos;
}

SIZE CPaintManagerUI::GetClientSize() const
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    return CSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE CPaintManagerUI::GetInitSize()
{
    return m_szInitWindowSize;
}

void CPaintManagerUI::SetInitSize(int cx, int cy)
{
    m_szInitWindowSize.cx = cx;
    m_szInitWindowSize.cy = cy;
    if( m_pRoot == NULL && m_hWndPaint != NULL ) {
        ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

RECT& CPaintManagerUI::GetSizeBox()
{
    return m_rcSizeBox;
}

void CPaintManagerUI::SetSizeBox(RECT& rcSizeBox)
{
    m_rcSizeBox = rcSizeBox;
}

RECT& CPaintManagerUI::GetCaptionRect()
{
    return m_rcCaption;
}

void CPaintManagerUI::SetCaptionRect(RECT& rcCaption)
{
    m_rcCaption = rcCaption;
}

SIZE CPaintManagerUI::GetRoundCorner() const
{
    return m_szRoundCorner;
}

void CPaintManagerUI::SetRoundCorner(int cx, int cy)
{
    m_szRoundCorner.cx = cx;
    m_szRoundCorner.cy = cy;
}

SIZE CPaintManagerUI::GetMinInfo() const
{
	return m_szMinWindow;
}

void CPaintManagerUI::SetMinInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMinWindow.cx = cx;
    m_szMinWindow.cy = cy;
}

SIZE CPaintManagerUI::GetMaxInfo() const
{
    return m_szMaxWindow;
}

void CPaintManagerUI::SetMaxInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMaxWindow.cx = cx;
    m_szMaxWindow.cy = cy;
}

void CPaintManagerUI::SetTransparent(int nOpacity)
{
    if( m_hWndPaint != NULL ) {
        typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
        PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

        HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
        if (hUser32)
        {
            fSetLayeredWindowAttributes = 
                (PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
            if( fSetLayeredWindowAttributes == NULL ) return;
        }

        DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
        DWORD dwNewStyle = dwStyle;
        if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
        else dwNewStyle &= ~WS_EX_LAYERED;
        if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
        fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
    }
}

void CPaintManagerUI::SetBackgroundTransparent(BOOL bTrans)
{
    m_bAlphaBackground = bTrans;
}

BOOL CPaintManagerUI::IsShowUpdateRect() const
{
	return m_bShowUpdateRect;
}

void CPaintManagerUI::SetShowUpdateRect(BOOL show)
{
    m_bShowUpdateRect = show;
}

BOOL CPaintManagerUI::PreMessageHandler(LPMSG lpMsg, LRESULT& /*lRes*/)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
    {
        BOOL bHandled = FALSE;
        LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(lpMsg, bHandled);
        if( bHandled ) {
            return TRUE;
        }
    }
	UINT uMsg = lpMsg->message;
	WPARAM wParam = lpMsg->wParam;
	LPARAM lParam = lpMsg->lParam;
    switch( uMsg ) {
    case WM_KEYDOWN:
        {
           // Tabbing between controls
           if( wParam == VK_TAB ) {
               if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL ) {
                   if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return FALSE;
               }
               SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
               return TRUE;
           }
        }
        break;
    case WM_SYSCHAR:
        {
           // Handle ALT-shortcut key-combinations
           FINDSHORTCUT fs = { 0 };
           fs.ch = toupper((int)wParam);
           CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
           if( pControl != NULL ) {
               pControl->SetFocus();
               pControl->Activate();
               return TRUE;
           }
        }
        break;
    case WM_SYSKEYDOWN:
        {
           if( m_pFocus != NULL ) {
               TEventUI event = { 0 };
               event.Type = UIEVENT_SYSKEY;
               event.chKey = (TCHAR)wParam;
               event.ptMouse = m_ptLastMousePos;
               event.wKeyState = MapKeyState();
               event.dwTimestamp = ::GetTickCount();
               m_pFocus->Event(event);
           }
        }
        break;
    }
    return FALSE;
}

BOOL CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
//#ifdef _DEBUG
//    switch( uMsg ) {
//    case WM_NCPAINT:
//    case WM_NCHITTEST:
//    case WM_SETCURSOR:
//       break;
//    default:
//       DUITRACE(_T("MSG: %-20s (%08ld)"), TRACEMSG(uMsg), ::GetTickCount());
//    }
//#endif
    // Not ready yet?
    if( m_hWndPaint == NULL ) return FALSE;
    
    TNotifyUI* pMsg = NULL;
    while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
        m_aAsyncNotify.Remove(0);
        if( pMsg->pSender != NULL ) {
            if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
        }
        for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
        }
        delete pMsg;
    }
    
    // Cycle through listeners
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
    {
        BOOL bHandled = FALSE;
		MSG msg = {0};
		msg.hwnd = m_hWndPaint;
		msg.message = uMsg;
		msg.lParam  = lParam;
		msg.wParam  = wParam;
        LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(&msg, bHandled);
        if( bHandled ) {
            lRes = lResult;
            return TRUE;
        }
    }
    // Custom handling of events
    switch( uMsg ) {
	case WM_NCPAINT:
		lRes = 0;
		return TRUE;
    case WM_APP + 1:
        {
            for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
                delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
            m_aDelayedCleanup.Empty();
        }
        break;
    case WM_CLOSE:
        {
            // Make sure all matching "closing" events are sent
            TEventUI event = { 0 };
            event.ptMouse = m_ptLastMousePos;
            event.dwTimestamp = ::GetTickCount();
            if( m_pEventHover != NULL ) {
                event.Type = UIEVENT_MOUSELEAVE;
                event.pSender = m_pEventHover;
                m_pEventHover->Event(event);
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_BUTTONUP;
                event.pSender = m_pEventClick;
                m_pEventClick->Event(event);
            }

            SetFocus(NULL);

            // Hmmph, the usual Windows tricks to avoid
            // focus loss...
            HWND hwndParent = GetWindowOwner(m_hWndPaint);
            if( hwndParent != NULL ) ::SetFocus(hwndParent);
        }

        break;
	case WM_KILLFOCUS:
		{
			if( m_pFocus != NULL ) 
			{
				TEventUI event = { 0 };
				event.Type = UIEVENT_KILLFOCUS;
				event.pSender = m_pFocus;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
				SendNotify(m_pFocus, _T("killfocus"));
				m_pFocus = NULL;
			}
		}
    case WM_ERASEBKGND:
        {
            // We'll do the painting here...
            lRes = 1;
        }
        return TRUE;
    case WM_PAINT:
        {
            // Should we paint?
            RECT rcPaint = { 0 };
            if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)&&!m_bFakeWnd ) return TRUE;
            if( m_pRoot == NULL ) {
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint, &ps);
                ::EndPaint(m_hWndPaint, &ps);
                return TRUE;
            } 
            // Do we need to resize anything?
            // This is the time where we layout the controls on the form.
            // We delay this even from the WM_SIZE messages since resizing can be
            // a very expensize operation.
            if( m_bUpdateNeeded ) {
                m_bUpdateNeeded = FALSE;
                RECT rcClient = { 0 };
                ::GetClientRect(m_hWndPaint, &rcClient);
                if( !::IsRectEmpty(&rcClient) ) {
                    if( m_pRoot->IsUpdateNeeded() ) {
						m_pRoot->SetPos(rcClient);
                        if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
                        if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
                        if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
                        if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
                        m_hDcOffscreen = NULL;
                        m_hDcBackground = NULL;
                        m_hbmpOffscreen = NULL;
                        m_hbmpBackground = NULL;
                    }
                    else {
                        CControlUI* pControl = NULL;
                        while( pControl = m_pRoot->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) ) {
                            pControl->SetPos( pControl->GetPos() );
                        }
                    }
                    // We'll want to notify the window when it is first initialized
                    // with the correct layout. The window form would take the time
                    // to submit swipes/animations.
                    if( m_bFirstLayout ) {
                        m_bFirstLayout = FALSE;
                        SendNotify(m_pRoot, _T("windowinit"),  0, 0, FALSE);
                    }
                }
            }
            // Set focus to first control?
            if( m_bFocusNeeded ) {
                SetNextTabControl();
            }
            //
            // Render screen
            //
            // Prepare offscreen bitmap?
            if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
            {
                RECT rcClient = { 0 };
                ::GetClientRect(m_hWndPaint, &rcClient);
                m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
                m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
                ASSERT(m_hDcOffscreen);
                ASSERT(m_hbmpOffscreen);
            }
            // Begin Windows paint
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(m_hWndPaint, &ps);
            if( m_bOffscreenPaint )
            {
                HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
                int iSaveDC = ::SaveDC(m_hDcOffscreen);
                if( m_bAlphaBackground ) {
                    if( m_hbmpBackground == NULL ) {
                        RECT rcClient = { 0 };
                        ::GetClientRect(m_hWndPaint, &rcClient);
                        m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);;
                        m_hbmpBackground = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
                        ASSERT(m_hDcBackground);
                        ASSERT(m_hbmpBackground);
                        ::SelectObject(m_hDcBackground, m_hbmpBackground);
                        ::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                            ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                    }
                    else
                        ::SelectObject(m_hDcBackground, m_hbmpBackground);
                    ::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                }
                m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);
                for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
                    CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);  //modify by lyb 
                    pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
                }
                ::RestoreDC(m_hDcOffscreen, iSaveDC);
                ::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                    ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                ::SelectObject(m_hDcOffscreen, hOldBitmap);

                if( m_bShowUpdateRect ) {
                    HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
                    ::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
                    ::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
                    ::SelectObject(ps.hdc, hOldPen);
                }
            }
            else
            {
                // A standard paint job
                int iSaveDC = ::SaveDC(ps.hdc);
                m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
                ::RestoreDC(ps.hdc, iSaveDC);
            }
            // All Done!
            ::EndPaint(m_hWndPaint, &ps);
        }
        // If any of the painting requested a resize again, we'll need
        // to invalidate the entire window once more.
        if( m_bUpdateNeeded ) {
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        }
        return TRUE;
    case WM_PRINTCLIENT:
        {
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            HDC hDC = (HDC) wParam;
            int save = ::SaveDC(hDC);
            m_pRoot->DoPaint(hDC, rcClient);
            // Check for traversing children. The crux is that WM_PRINT will assume
            // that the DC is positioned at frame coordinates and will paint the child
            // control at the wrong position. We'll simulate the entire thing instead.
            if( (lParam & PRF_CHILDREN) != 0 ) {
                HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                while( hWndChild != NULL ) {
                    RECT rcPos = { 0 };
                    ::GetWindowRect(hWndChild, &rcPos);
                    ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
                    ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
                    // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
                    //       since the latter will not print the nonclient correctly for
                    //       EDIT controls.
                    ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
                    hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                }
            }
            ::RestoreDC(hDC, save);
        }
        break;
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
            if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
            if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
            if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
        }
        break;
    case WM_SIZE:
        {
            if( m_pFocus != NULL ) {
                TEventUI event = { 0 };
                event.Type = UIEVENT_WINDOWSIZE;
                event.pSender = m_pFocus;
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            }
            if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
        }
        return TRUE;
    case WM_TIMER:
        {
            for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
                const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
                if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == FALSE) {
                    TEventUI event = { 0 };
                    event.Type = UIEVENT_TIMER;
                    event.pSender = pTimer->pSender;
                    event.wParam = pTimer->nLocalID;
                    event.dwTimestamp = ::GetTickCount();
                    pTimer->pSender->Event(event);
                    break;
                }
            }
        }
        break;
    case WM_MOUSEHOVER:
        {
            m_bMouseTracking = FALSE;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            CControlUI* pHover = FindControl(pt);
            if( pHover == NULL ) break;
            // Generate mouse hover event
            if( m_pEventHover != NULL ) {
                TEventUI event = { 0 };
                event.ptMouse = pt;
                event.Type = UIEVENT_MOUSEHOVER;
                event.pSender = m_pEventHover;
                event.dwTimestamp = ::GetTickCount();
                m_pEventHover->Event(event);
            }
            // Create tooltip information
           // CString sToolTip = CUILanguage::ConvertLanguage(pHover->GetToolTip());
			CString sToolTip = pHover->GetToolTip();
            if( sToolTip.IsEmpty() ) return TRUE;
            ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
            m_ToolTip.cbSize = sizeof(TOOLINFO);
            m_ToolTip.uFlags = TTF_IDISHWND;
            m_ToolTip.hwnd = m_hWndPaint;
            m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
            m_ToolTip.hinst = m_hInstance;
            m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
            m_ToolTip.rect = pHover->GetPos();
            if( m_hwndTooltip == NULL ) {
                m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
                ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
				SendMessage(m_hwndTooltip,   TTM_SETMAXTIPWIDTH,   0,   300);
            }
            ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
            ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
        }
        return TRUE;
    case WM_MOUSELEAVE:
        {
            if( m_hwndTooltip != NULL ) 
				::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
            if( m_bMouseTracking) 
				::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
            m_bMouseTracking = FALSE;
        }
        break;
    case WM_MOUSEMOVE:
        {
            // Start tracking this entire window again...
            if( !m_bMouseTracking ) {
                TRACKMOUSEEVENT tme = { 0 };
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = m_hWndPaint;
                tme.dwHoverTime = m_hwndTooltip == NULL ? 600UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                _TrackMouseEvent(&tme);
                m_bMouseTracking = TRUE;
            }
            // Generate the appropriate mouse messages
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pNewHover = FindControl(pt);
			//DUITRACE(_T("Hover: %d\n"),pNewHover->GetID());
            if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
            TEventUI event = { 0 };
            event.ptMouse = pt;
            event.dwTimestamp = ::GetTickCount();
            if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
                event.Type = UIEVENT_MOUSELEAVE;
                event.pSender = m_pEventHover;
                m_pEventHover->Event(event);
                m_pEventHover = NULL;
                if( m_hwndTooltip != NULL ) 
				{
					::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
					//DUITRACE(_T("m_ToolTip%d,%d\n"),pt.x,pt.y);
				}
				else
				{
					//DUITRACE(_T("no  m_ToolTip%d,%d\n"),pt.x,pt.y);
				}
            }
            if( pNewHover != m_pEventHover && pNewHover != NULL ) {
                event.Type = UIEVENT_MOUSEENTER;
                event.pSender = pNewHover;
                pNewHover->Event(event);
                m_pEventHover = pNewHover;
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = m_pEventClick;
                m_pEventClick->Event(event);
            }
            else if( pNewHover != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = pNewHover;
                pNewHover->Event(event);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            // We alway set focus back to our app (this helps
            // when Win32 child windows are placed on the dialog
            // and we need to remove them on focus change).
            ::SetFocus(m_hWndPaint);

            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);	
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            m_pEventClick = pControl;
            pControl->SetFocus();
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        }
        break;
    case WM_LBUTTONDBLCLK:
        {
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_DBLCLICK;
            event.pSender = pControl;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
            m_pEventClick = pControl;
        }
        break;
    case WM_LBUTTONUP:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            ReleaseCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONUP;
            event.pSender = m_pEventClick;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            m_pEventClick->Event(event);
            m_pEventClick = NULL;
        }
        break;
    case WM_RBUTTONDOWN:
        {
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            pControl->SetFocus();
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_RBUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
            m_pEventClick = pControl;
        }
        break;
	case WM_RBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL ) break;
			ReleaseCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_RBUTTONUP;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = NULL;
		}
		break;
    case WM_CONTEXTMENU:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            ReleaseCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_CONTEXTMENU;
            event.pSender = m_pEventClick;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.lParam = (LPARAM)m_pEventClick;
            event.dwTimestamp = ::GetTickCount();
            m_pEventClick->Event(event);
            m_pEventClick = NULL;
        }
        break;
    case WM_MOUSEWHEEL:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            int zDelta = (int) (short) HIWORD(wParam);
            TEventUI event = { 0 };
            event.Type = UIEVENT_SCROLLWHEEL;
            event.pSender = pControl;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);

            // Let's make sure that the scroll item below the cursor is the same as before...
            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
        }
        break;
    case WM_CHAR:
        {
            if( m_pFocus == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_CHAR;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
        }
        break;
    case WM_KEYDOWN:
        {
            if( m_pFocus == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            m_pEventKey = m_pFocus;
        }
        break;
    case WM_KEYUP:
        {
            if( m_pEventKey == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYUP;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pEventKey->Event(event);
            m_pEventKey = NULL;
        }
        break;
    case WM_SETCURSOR:
        {
            if( LOWORD(lParam) != HTCLIENT ) break;
            if( m_bMouseCapture ) return TRUE;

            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_hWndPaint, &pt);
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_SETCURSOR;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        }
        return TRUE;
// 	case WM_KILLFOCUS:
// 		{
// 			SetFocus(NULL);
// 		}
// 		return TRUE;
    case WM_NOTIFY:
        {
            LPNMHDR lpNMHDR = (LPNMHDR) lParam;
            if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
            return TRUE;
        }
        break;
    case WM_COMMAND:
        {
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return TRUE;
        }
        break;
    case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
        {
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return TRUE;
        }
        break;
    default:
        break;
    }

    pMsg = NULL;
    while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
        m_aAsyncNotify.Remove(0);
        if( pMsg->pSender != NULL ) {
            if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
        }
        for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
        }
        delete pMsg;
    }

    return FALSE;
}

void CPaintManagerUI::ClearEventClick(CControlUI * pControl)
{
	m_pEventClick = pControl;
	m_pEventHover = pControl;
}

void CPaintManagerUI::NeedUpdate()
{
    m_bUpdateNeeded = TRUE;
}

void CPaintManagerUI::Invalidate(RECT& rcItem)
{
    ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

BOOL CPaintManagerUI::AttachDialog(CControlUI* pControl)
{
    ASSERT(::IsWindow(m_hWndPaint));
    // Reset any previous attachment
    SetFocus(NULL);
    m_pEventKey = NULL;
    m_pEventHover = NULL;
    m_pEventClick = NULL;
    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if( m_pRoot != NULL ) {
        m_aPostPaintControls.Empty();
        AddDelayedCleanup(m_pRoot);
    }
    // Set the dialog root element
    m_pRoot = pControl;
    // Go ahead...
    m_bUpdateNeeded = TRUE;
    m_bFirstLayout = TRUE;
    m_bFocusNeeded = TRUE;
    // Initiate all control
    return InitControls(pControl);
}

BOOL CPaintManagerUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
{
    ASSERT(pControl);
    if( pControl == NULL ) return FALSE;
    pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), TRUE);
    pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
    return TRUE;
}

void CPaintManagerUI::ReapObjects(CControlUI* pControl)
{
    if( pControl == m_pEventKey ) m_pEventKey = NULL;
    if( pControl == m_pEventHover ) m_pEventHover = NULL;
    if( pControl == m_pEventClick ) m_pEventClick = NULL;
    if( pControl == m_pFocus ) m_pFocus = NULL;
    KillTimer(pControl);
    const CString& sName = pControl->GetName();
    if( !sName.IsEmpty() ) {
        if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
    }
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
        TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
        if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
    }    
}

BOOL CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) {
        CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
        for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
            if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
                return FALSE;
            }
        }
        aOptionGroup->Add(pControl);
    }
    else {
        CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
        aOptionGroup->Add(pControl);
        m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
    }
    return TRUE;
}

CStdPtrArray* CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) return static_cast<CStdPtrArray*>(lp);
    return NULL;
}

void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
{
	LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
	if( lp ) {
		CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
		if( aOptionGroup == NULL ) return;
		for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
			if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
				aOptionGroup->Remove(i);
				break;
			}
		}
		if( aOptionGroup->IsEmpty() ) {
			delete aOptionGroup;
			m_mOptionGroup.Remove(pStrGroupName);
		}
	}
}

void CPaintManagerUI::RemoveAllOptionGroups()
{
	CStdPtrArray* aOptionGroup;
	for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
		if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
			aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
			delete aOptionGroup;
		}
	}
}

void CPaintManagerUI::MessageLoop()
{
    MSG msg = { 0 };
    while( ::GetMessage(&msg, NULL, 0, 0) ) {
        if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

BOOL CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{
    // Pretranslate Message takes care of system-wide messages, such as
    // tabbing and shortcut key-combos. We'll look for all messages for
    // each window and any child control attached.
    HWND hwndParent = ::GetParent(pMsg->hwnd);
    UINT uStyle = GetWindowStyle(pMsg->hwnd);
    LRESULT lRes = 0;
    for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
        CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
		if(1)//pMsg->hwnd == pT->GetPaintWindow()|| (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0))||(IsChild(hwndParent,pT->GetPaintWindow())))
        {
            if( pT->PreMessageHandler(pMsg, lRes) ) return TRUE;
        }
    }
    return FALSE;
}

CControlUI* CPaintManagerUI::GetFocus() const
{
    return m_pFocus;
}

void CPaintManagerUI::SetFocus(CControlUI* pControl)
{
    // Paint manager window has focus?
	//DUITRACE(_T("setfocus a\n"));
    HWND hFocusWnd = ::GetFocus();
	if( hFocusWnd != m_hWndPaint&& pControl != m_pFocus ){ ::SetFocus(m_hWndPaint);return;}
    // Already has focus?
    if( pControl == m_pFocus ) return;
    // Remove focus from old control
	
    if( m_pFocus != NULL ) 
    {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, _T("killfocus"));
        m_pFocus = NULL;
    }
    if( pControl == NULL ) return;
    // Set focus to new control
    if( pControl != NULL 
        && pControl->GetManager() == this 
        && pControl->IsVisible() 
        && pControl->IsEnabled() ) 
    {
        m_pFocus = pControl;
        TEventUI event = { 0 };
        event.Type = UIEVENT_SETFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, _T("setfocus"));
    }
}

void CPaintManagerUI::SetFocusNeeded(CControlUI* pControl)
{
    ::SetFocus(m_hWndPaint);
    if( pControl == NULL ) return;
    if( m_pFocus != NULL ) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, _T("killfocus"));
        m_pFocus = NULL;
    }
    FINDTABINFO info = { 0 };
    info.pFocus = pControl;
    info.bForward = FALSE;
    m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    m_bFocusNeeded = TRUE;
    if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
}

BOOL CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse)
{
    ASSERT(pControl!=NULL);
    ASSERT(uElapse>0);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID )
        {
            if( pTimer->bKilled == TRUE ) {
                if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) {
                    pTimer->bKilled = FALSE;
                    return TRUE;
                }

                return FALSE;
            }

            return FALSE;
        }
    }

    m_uTimerID = (++m_uTimerID) % 0xFF;
    if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
    TIMERINFO* pTimer = new TIMERINFO;
    if( pTimer == NULL ) return FALSE;
    pTimer->hWnd = m_hWndPaint;
    pTimer->pSender = pControl;
    pTimer->nLocalID = nTimerID;
    pTimer->uWinTimer = m_uTimerID;
    pTimer->bKilled = FALSE;
    return m_aTimers.Add(pTimer);
}

BOOL CPaintManagerUI::KillTimer(CControlUI* pControl, UINT nTimerID)
{
    ASSERT(pControl!=NULL);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID )
        {
            if( pTimer->bKilled == FALSE ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                pTimer->bKilled = TRUE;
				//delete pTimer;
                return TRUE;
            }
        }
    }
    return FALSE;
}

void CPaintManagerUI::KillTimer(CControlUI* pControl)
{
    ASSERT(pControl!=NULL);
    int count = m_aTimers.GetSize();
    for( int i = 0, j = 0; i < count; i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
        if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
            if( pTimer->bKilled == FALSE ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
            m_aTimers.Remove(i);
			//delete pTimer;
            j++;
        }
    }
}

void CPaintManagerUI::RemoveAllTimers()
{
    for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->hWnd == m_hWndPaint )
        {
            if( pTimer->bKilled == FALSE ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
            }
            delete pTimer;
        }
    }

    m_aTimers.Empty();
}

void CPaintManagerUI::SetCapture()
{
    ::SetCapture(m_hWndPaint);
    m_bMouseCapture = TRUE;
}

void CPaintManagerUI::ReleaseCapture()
{
    ::ReleaseCapture();
    m_bMouseCapture = FALSE;
}

BOOL CPaintManagerUI::IsCaptured()
{
    return m_bMouseCapture;
}

BOOL CPaintManagerUI::SetNextTabControl(BOOL bForward)
{
    // If we're in the process of restructuring the layout we can delay the
    // focus calulation until the next repaint.
    if( m_bUpdateNeeded && bForward ) {
        m_bFocusNeeded = TRUE;
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return TRUE;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.pFocus = m_pFocus;
    info1.bForward = bForward;
    CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if( pControl == NULL ) {  
        if( bForward ) {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.pFocus = bForward ? NULL : info1.pLast;
            info2.bForward = bForward;
            pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        }
        else {
            pControl = info1.pLast;
        }
    }
    if( pControl != NULL ) SetFocus(pControl);
    m_bFocusNeeded = FALSE;
    return TRUE;
}

BOOL CPaintManagerUI::AddNotifier(INotifyUI* pNotifier)
{
    ASSERT(m_aNotifiers.Find(pNotifier)<0);
    return m_aNotifiers.Add(pNotifier);
}

BOOL CPaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
{
    for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
        if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
            return m_aNotifiers.Remove(i);
        }
    }
    return FALSE;
}

BOOL CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
{
    ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
    return m_aPreMessageFilters.Add(pFilter);
}

BOOL CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
            return m_aPreMessageFilters.Remove(i);
        }
    }
    return FALSE;
}

BOOL CPaintManagerUI::AddMessageFilter(IMessageFilterUI* pFilter)
{
    ASSERT(m_aMessageFilters.Find(pFilter)<0);
    return m_aMessageFilters.Add(pFilter);
}

BOOL CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
{
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
            return m_aMessageFilters.Remove(i);
        }
    }
    return FALSE;
}

int CPaintManagerUI::GetPostPaintCount() const
{
    return m_aPostPaintControls.GetSize();
}

BOOL CPaintManagerUI::AddPostPaint(CControlUI* pControl)
{
    ASSERT(m_aPostPaintControls.Find(pControl) < 0);
    return m_aPostPaintControls.Add(pControl);
}

BOOL CPaintManagerUI::RemovePostPaint(CControlUI* pControl)
{
    for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
        if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
            return m_aPostPaintControls.Remove(i);
        }
    }
    return FALSE;
}

BOOL CPaintManagerUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
{
    RemovePostPaint(pControl);
    return m_aPostPaintControls.InsertAt(iIndex, pControl);
}

void CPaintManagerUI::AddDelayedCleanup(CControlUI* pControl)
{
    pControl->SetManager(this, NULL, FALSE);
    m_aDelayedCleanup.Add(pControl);
    ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
}

void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, BOOL bAsync /*= FALSE*/)
{
    TNotifyUI Msg;
    Msg.pSender = pControl;
    Msg.sType = pstrMessage;
    Msg.wParam = wParam;
    Msg.lParam = lParam;
    SendNotify(Msg, bAsync);
}

void CPaintManagerUI::SendNotify(TNotifyUI& Msg, BOOL bAsync /*= FALSE*/)
{
    Msg.ptMouse = m_ptLastMousePos;
    Msg.dwTimestamp = ::GetTickCount();
    if( !bAsync ) {
        // Send to all listeners
        if( Msg.pSender != NULL ) {
            if( Msg.pSender->OnNotify ) Msg.pSender->OnNotify(&Msg);
        }
        for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
        }
    }
    else {
        TNotifyUI *pMsg = new TNotifyUI;
        pMsg->pSender = Msg.pSender;
        pMsg->sType = Msg.sType;
        pMsg->wParam = Msg.wParam;
        pMsg->lParam = Msg.lParam;
        pMsg->ptMouse = Msg.ptMouse;
        pMsg->dwTimestamp = Msg.dwTimestamp;
        m_aAsyncNotify.Add(pMsg);
    }
}

BOOL CPaintManagerUI::UseParentResource(CPaintManagerUI* pm)
{
    if( pm == NULL ) {
        m_pParentResourcePM = NULL;
        return TRUE;
    }
    if( pm == this ) return FALSE;

    CPaintManagerUI* pParentPM = pm->GetParentResource();
    while( pParentPM ) {
        if( pParentPM == this ) return FALSE;
        pParentPM = pParentPM->GetParentResource();
    }
    m_pParentResourcePM = pm;
    return TRUE;
}

CPaintManagerUI* CPaintManagerUI::GetParentResource() const
{
    return m_pParentResourcePM;
}

DWORD CPaintManagerUI::GetDefaultDisabledColor() const
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultDisabledColor();
    return m_dwDefaultDisabledColor;
}

void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor)
{
    m_dwDefaultDisabledColor = dwColor;
}

DWORD CPaintManagerUI::GetDefaultFontColor() const
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontColor();
    return m_dwDefaultFontColor;
}

void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor)
{
    m_dwDefaultFontColor = dwColor;
}

DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkFontColor();
    return m_dwDefaultLinkFontColor;
}

void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor)
{
    m_dwDefaultLinkFontColor = dwColor;
}

DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkHoverFontColor();
    return m_dwDefaultLinkHoverFontColor;
}

void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor)
{
    m_dwDefaultLinkHoverFontColor = dwColor;
}

DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultSelectedBkColor();
    return m_dwDefaultSelectedBkColor;
}

void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor)
{
    m_dwDefaultSelectedBkColor = dwColor;
}

TFontInfo* CPaintManagerUI::GetDefaultFontInfo()
{
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontInfo();

    if( m_DefaultFontInfo.tm.tmHeight == 0 ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_DefaultFontInfo.hFont);
        ::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return &m_DefaultFontInfo;
}

void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -12;
	lf.lfWeight = 0;

    _tcscpy(lf.lfFaceName, pStrFontName);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return;

    ::DeleteObject(m_DefaultFontInfo.hFont);
    m_DefaultFontInfo.hFont = hFont;
   // m_DefaultFontInfo.sFontName = pStrFontName;
	StrCpy( m_DefaultFontInfo.sFontName,pStrFontName);
    m_DefaultFontInfo.iSize = nSize;
    m_DefaultFontInfo.bBold = bBold;
    m_DefaultFontInfo.bUnderline = bUnderline;
    m_DefaultFontInfo.bItalic = bItalic;
    ::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));
    if( m_hDcPaint ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
}

DWORD CPaintManagerUI::GetCustomFontCount() const
{
    return m_aCustomFonts.GetSize();
}

HFONT CPaintManagerUI::AddFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcscpy(lf.lfFaceName, pStrFontName);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
	lf.lfWeight = 0;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return NULL;

    TFontInfo* pFontInfo = new TFontInfo;
    if( !pFontInfo ) return FALSE;
    ::ZeroMemory(pFontInfo, sizeof(TFontInfo));
    pFontInfo->hFont = hFont;
	StrCpy(pFontInfo->sFontName,pStrFontName);
    pFontInfo->iSize = nSize;
    pFontInfo->bBold = bBold;
    pFontInfo->bUnderline = bUnderline;
    pFontInfo->bItalic = bItalic;
    if( m_hDcPaint ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    if( !m_aCustomFonts.Add(pFontInfo) ) {
        ::DeleteObject(hFont);
        delete pFontInfo;
        return NULL;
    }

    return hFont;
}

HFONT CPaintManagerUI::AddFontAt(int index, LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcscpy(lf.lfFaceName, pStrFontName);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
	lf.lfWeight = 0;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return NULL;

    TFontInfo* pFontInfo = new TFontInfo;
    if( !pFontInfo ) return FALSE;
    ::ZeroMemory(pFontInfo, sizeof(TFontInfo));
    pFontInfo->hFont = hFont;
	StrCpy(pFontInfo->sFontName,pStrFontName);
    pFontInfo->iSize = nSize;
    pFontInfo->bBold = bBold;
    pFontInfo->bUnderline = bUnderline;
    pFontInfo->bItalic = bItalic;
    if( m_hDcPaint ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    if( !m_aCustomFonts.InsertAt(index, pFontInfo) ) {
        ::DeleteObject(hFont);
        delete pFontInfo;
        return NULL;
    }

    return hFont;
}

HFONT CPaintManagerUI::GetFont(int index)
{
    if( index < 0 || index >= m_aCustomFonts.GetSize() ) return GetDefaultFontInfo()->hFont;
    TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
    return pFontInfo->hFont;
}

HFONT CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( StrCmpI(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
            pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
            return pFontInfo->hFont;
    }
    if( m_pParentResourcePM ) return m_pParentResourcePM->GetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
    return NULL;
}

BOOL CPaintManagerUI::FindFont(HFONT hFont)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( pFontInfo->hFont == hFont ) return TRUE;
    }
    if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(hFont);
    return FALSE;
}

BOOL CPaintManagerUI::FindFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( StrCmpI(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
            pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
            return TRUE;
    }
    if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
    return FALSE;
}

int CPaintManagerUI::GetFontIndex(HFONT hFont)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( pFontInfo->hFont == hFont ) return it;
    }
    return -1;
}

int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( StrCmpI(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
            pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
            return it;
    }
    return -1;
}

BOOL CPaintManagerUI::RemoveFont(HFONT hFont)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( pFontInfo->hFont == hFont ) {
            ::DeleteObject(pFontInfo->hFont);
            delete pFontInfo;
            return m_aCustomFonts.Remove(it);
        }
    }

    return FALSE;
}

BOOL CPaintManagerUI::RemoveFontAt(int index)
{
    if( index < 0 || index >= m_aCustomFonts.GetSize() ) return FALSE;
    TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
    ::DeleteObject(pFontInfo->hFont);
    delete pFontInfo;
    return m_aCustomFonts.Remove(index);
}

void CPaintManagerUI::RemoveAllFonts()
{
    TFontInfo* pFontInfo;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        ::DeleteObject(pFontInfo->hFont);
        delete pFontInfo;
    }
    m_aCustomFonts.Empty();
}

TFontInfo* CPaintManagerUI::GetFontInfo(int index)
{
    if( index < 0 || index >= m_aCustomFonts.GetSize() ) return GetDefaultFontInfo();
    TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
    if( pFontInfo->tm.tmHeight == 0 ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return pFontInfo;
}

TFontInfo* CPaintManagerUI::GetFontInfo(HFONT hFont)
{
    TFontInfo* pFontInfo = NULL;
    for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
        pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
        if( pFontInfo->hFont == hFont ) {
            if( pFontInfo->tm.tmHeight == 0 ) {
                HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
                ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
            return pFontInfo;
        }
    }

    if( m_pParentResourcePM ) return m_pParentResourcePM->GetFontInfo(hFont);
    return GetDefaultFontInfo();
}

const TImageInfo* CPaintManagerUI::GetImage(LPCTSTR bitmap)
{
    TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
    if( !data && m_pParentResourcePM ) return m_pParentResourcePM->GetImage(bitmap);
    else return data;
}

const TImageInfo* CPaintManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type , DWORD mask )
{
	TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
	if( !data ) {
		if( AddImage(bitmap, type, mask) ) {
			data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
		}
	}

	return data;
}

const TImageInfo* CPaintManagerUI::GetImageEx(UINT nID, LPCTSTR type, DWORD mask)
{
	CString strKey;
	strKey.Format(_T("%d%s"),nID,type);
    TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(strKey));
    if( !data ) {
        if( AddImage(strKey,nID, type, mask) ) {
            data = static_cast<TImageInfo*>(m_mImageHash.Find(strKey));
        }
    }

    return data;
}

const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap,UINT nID, LPCTSTR type, DWORD mask)
{
	TImageInfo* data = NULL;
	if( type != NULL ) {
			data = CRenderEngine::LoadImage(nID, type, mask);
	}
	else {
		return NULL;
	}

	if( !data ) return NULL;
	if( type != NULL )
	{
		StrCpy(data->sResType,type);
	}
	data->dwMask = mask;
	if( !m_mImageHash.Insert(bitmap, data) ) {
		::DeleteObject(data->hBitmap);
		delete data;
	}

	return data;
}

const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask)
{
    TImageInfo* data = NULL;
    if( type != NULL ) {
        if( isdigit(*bitmap) ) {
            LPTSTR pstr = NULL;
            int iIndex = _tcstol(bitmap, &pstr, 10);
            data = CRenderEngine::LoadImage(iIndex, type, mask);
        }
    }
    else {
        data = CRenderEngine::LoadImage(bitmap, NULL, mask);
    }

    if( !data ) return NULL;
    if( type != NULL )
	{
		StrCpy(data->sResType,type);
	}
    data->dwMask = mask;
    if( !m_mImageHash.Insert(bitmap, data) ) {
        ::DeleteObject(data->hBitmap);
        delete data;
    }

    return data;
}

const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, BOOL bAlpha)
{
    if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

    TImageInfo* data = new TImageInfo;
    data->hBitmap = hBitmap;
    data->nX = iWidth;
    data->nY = iHeight;
    data->alphaChannel = bAlpha;
    //data->sResType = _T("");
    data->dwMask = 0;
    if( !m_mImageHash.Insert(bitmap, data) ) {
        ::DeleteObject(data->hBitmap);
        delete data;
    }

    return data;
}

BOOL CPaintManagerUI::RemoveImage(LPCTSTR bitmap)
{
    const TImageInfo* data = GetImage(bitmap);
    if( !data ) return FALSE;

    ::DeleteObject(data->hBitmap);
    delete data;

    return m_mImageHash.Remove(bitmap);
}

void CPaintManagerUI::RemoveAllImages()
{
    TImageInfo* data;
    for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
        if(LPCTSTR key = m_mImageHash.GetAt(i)) {
            data = static_cast<TImageInfo*>(m_mImageHash.Find(key));
            ::DeleteObject(data->hBitmap);
            delete data;
        }
    }
}

void CPaintManagerUI::ReloadAllImages()
{
    BOOL bRedraw = FALSE;
    TImageInfo* data;
    TImageInfo* pNewData;
    for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
        if(LPCTSTR bitmap = m_mImageHash.GetAt(i)) {
            data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
            if( data != NULL ) {
				CString strType(data->sResType);
                if( !strType.IsEmpty() ) 
				{
                    if( isdigit(*bitmap) ) 
					{
                        LPTSTR pstr = NULL;
                        int iIndex = _tcstol(bitmap, &pstr, 10);
                        pNewData = CRenderEngine::LoadImage(iIndex, data->sResType, data->dwMask);
                    }
                }
                else {
                    pNewData = CRenderEngine::LoadImage(bitmap, NULL, data->dwMask);
                }
                if( pNewData == NULL ) continue;

                if( data->hBitmap != NULL ) ::DeleteObject(data->hBitmap);
                data->hBitmap = pNewData->hBitmap;
                data->nX = pNewData->nX;
                data->nY = pNewData->nY;
                data->alphaChannel = pNewData->alphaChannel;

                delete pNewData;
                bRedraw = TRUE;
            }
        }
    }
    if( bRedraw && m_pRoot ) m_pRoot->Invalidate();
}

void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList)
{
	CString* pDefaultAttr = new CString(pStrControlAttrList);
	if (pDefaultAttr != NULL)
	{
		if (m_DefaultAttrHash.Find(pStrControlName) == NULL)
			m_DefaultAttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr);
		else
			delete pDefaultAttr;
	}
}

LPCTSTR CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
{
    CString* pDefaultAttr = static_cast<CString*>(m_DefaultAttrHash.Find(pStrControlName));
    if( !pDefaultAttr && m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultAttributeList(pStrControlName);
    
    if( pDefaultAttr ) return pDefaultAttr->GetString();
    else return NULL;
}

BOOL CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName)
{
    CString* pDefaultAttr = static_cast<CString*>(m_DefaultAttrHash.Find(pStrControlName));
    if( !pDefaultAttr ) return FALSE;

    delete pDefaultAttr;
    return m_DefaultAttrHash.Remove(pStrControlName);
}

const CStringPtrMap& CPaintManagerUI::GetDefaultAttribultes() const
{
	return m_DefaultAttrHash;
}

void CPaintManagerUI::RemoveAllDefaultAttributeList()
{
	CString* pDefaultAttr;
	for( int i = 0; i< m_DefaultAttrHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_DefaultAttrHash.GetAt(i)) {
			pDefaultAttr = static_cast<CString*>(m_DefaultAttrHash.Find(key));
			delete pDefaultAttr;
		}
	}
}

CControlUI* CPaintManagerUI::GetRoot() const
{
    ASSERT(m_pRoot);
    return m_pRoot;
}

CControlUI* CPaintManagerUI::FindControl(POINT pt) const
{
    ASSERT(m_pRoot);
    return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CPaintManagerUI::FindControl(LPCTSTR pstrName) const
{
    ASSERT(m_pRoot);
    return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
}

CControlUI* CPaintManagerUI::FindControl(UINT nID) const
{
	ASSERT(m_pRoot);
	return m_pRoot->FindControl(__FindControlsFromID, &nID, UIFIND_ALL);
}

CControlUI* CPaintManagerUI::FindSubControlByPoint(CControlUI* pParent, POINT pt) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CPaintManagerUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
}

CControlUI* CPaintManagerUI::FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Resize(iIndex + 1);
    return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
}

CStdPtrArray* CPaintManagerUI::FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Empty();
    pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
    return &m_aFoundControls;
}

CStdPtrArray* CPaintManagerUI::GetSubControlsByClass()
{
    return &m_aFoundControls;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
{
    CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(pData);
    const CString& sName = pThis->GetName();
    if( sName.IsEmpty() ) return NULL;
    // Add this control to the hash list
    pManager->m_mNameHash.Set(sName, pThis);
    return NULL; // Attempt to add all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
{
    int* pnCount = static_cast<int*>(pData);
    (*pnCount)++;
    return NULL;  // Count all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
{
    LPPOINT pPoint = static_cast<LPPOINT>(pData);
    return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
{
    FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
    if( pInfo->pFocus == pThis ) {
        if( pInfo->bForward ) pInfo->bNextIsIt = TRUE;
        return pInfo->bForward ? NULL : pInfo->pLast;
    }
    if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
    pInfo->pLast = pThis;
    if( pInfo->bNextIsIt ) return pThis;
    if( pInfo->pFocus == NULL ) return pThis;
    return NULL;  // Examine all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
{
    if( !pThis->IsVisible() ) return NULL; 
    FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
    if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = TRUE;
    if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL ) return NULL;   // Labels never get focus!
    return pFS->bPickNext ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromUpdate(CControlUI* pThis, LPVOID pData)
{
    return pThis->IsUpdateNeeded() ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
    const CString& sName = pThis->GetName();
    if( sName.IsEmpty() ) return NULL;
    return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    CStdPtrArray* pFoundControls = pThis->GetManager()->GetSubControlsByClass();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
        int iIndex = -1;
        while( pFoundControls->GetAt(++iIndex) != NULL ) ;
        if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
    }
    if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
    return NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
        pThis->GetManager()->GetSubControlsByClass()->Add((LPVOID)pThis);
    return NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromID(CControlUI* pThis, LPVOID pData)
{
	LPUINT nID = static_cast<LPUINT>(pData);
	UINT nThisID = pThis->GetID();
	return  (*nID == nThisID) ? pThis : NULL;
}

} // namespace DuiLib
