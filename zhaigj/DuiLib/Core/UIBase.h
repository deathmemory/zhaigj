#ifndef __UIBASE_H__
#define __UIBASE_H__


#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

#define UI_WNDSTYLE_CONTAINER		(0)
#define UI_WNDSTYLE_FRAME			(WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD			(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG			(WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_SKINDLG			(WS_VISIBLE | WS_CAPTION| WS_POPUP | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |WS_MINIMIZE|WS_MAXIMIZE|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME)
#define UI_WNDSTYLE_SKINDLG_CAP		(WS_VISIBLE | WS_CAPTION| WS_POPUP | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |WS_OVERLAPPED|WS_THICKFRAME|WS_MAXIMIZE|WS_MINIMIZEBOX)
#define UI_WNDSTYLE_OVERLAPPED		(UI_WNDSTYLE_SKINDLG | WS_OVERLAPPED)

#define UI_WNDSTYLE_EX_FRAME		(WS_EX_WINDOWEDGE|WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_APPWINDOW)
#define UI_WNDSTYLE_EX_DIALOG		(WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


#define _INVALID_RESOURCE_ID		(0)
#define _INVALID_COLOR               RGB(1,1,1)

#define ARGB(a,r,g,b)         (((DWORD) (b) <<  0) |((DWORD) (g) << 8) |((DWORD) (r) <<   16) |((DWORD) (a) << 24))
#define GetAValue(argb)      (LOBYTE((argb)>>24))
/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#ifndef DUITRACE
#define DUITRACE DUI__Trace
#endif
#define DUITRACEMSG DUI__TraceMsg
#else
#ifndef DUITRACE
#define DUITRACE
#endif
#define DUITRACEMSG _T("")
#endif

typedef enum
{
	BUTTON_TYPE_NORMAL = 0,
	BUTTON_TYPE_CHECK,
	BUTTON_TYPE_CHECKBOX,
	BUTTON_TYPE_RADIOBOX,
}BUTTON_TYPE;

void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...);
LPCTSTR UILIB_API DUI__TraceMsg(UINT uMsg);

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CNotifyPump
{
public:
	BOOL AddVirtualWnd(CDuiString strName,CNotifyPump* pObject);
	BOOL RemoveVirtualWnd(CDuiString strName);
	void NotifyPump(TNotifyUI& msg);
	BOOL LoopDispatch(TNotifyUI& msg);
	DUI_DECLARE_MESSAGE_MAP()
private:
	CStdStringPtrMap m_VirtualWndMap;
};

class UILIB_API CWindowWnd
{
public:
    CWindowWnd();

    HWND GetHWND() const;
    operator HWND() const;

    BOOL RegisterWindowClass();
    BOOL RegisterSuperclass();

    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
    HWND CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);
    HWND Subclass(HWND hWnd);
    void Unsubclass();
    void ShowWindow(BOOL bShow = TRUE, BOOL bTakeFocus = TRUE);
   // UINT ShowModal(HWND hParentWindow = ::GetActiveWindow());
    void Close(UINT nRet = IDOK);
    void CenterWindow(HWND hCen = NULL);	// 居中，支持扩展屏幕
    void SetIcon(UINT nRes);

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    void ResizeClient(int cx = -1, int cy = -1);
	virtual LPCTSTR GetWindowClassName() const = 0;
protected:
   
    virtual LPCTSTR GetSuperClassName() const;
    virtual UINT GetClassStyle() const;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnFinalMessage(HWND hWnd);

    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    HWND m_hWnd;
    WNDPROC m_OldWndProc;
    BOOL m_bSubclassed;
};

} // namespace DuiLib

#endif // __UIBASE_H__
