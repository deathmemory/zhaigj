#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl;

template< class T >
class CSafeRelease
{
public:
    CSafeRelease(T* p) : m_p(p) { };
    ~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
    T* Detach() { T* t = m_p; m_p = NULL; return t; };
    T* m_p;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
{
    friend CActiveXCtrl;
public:
    CActiveXUI();
    virtual ~CActiveXUI();

    LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

    HWND GetHostWindow() const;

    BOOL IsDelayCreate() const;
    void SetDelayCreate(BOOL bDelayCreate = TRUE);

    BOOL CreateControl(const CLSID clsid);
    BOOL CreateControl(LPCTSTR pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID* ppRet);
	CLSID GetClisd() const;
    CString GetModuleName() const;
    void SetModuleName(LPCTSTR pstrText);

    void SetVisible(BOOL bVisible = TRUE);
    void SetInternVisible(BOOL bVisible = TRUE);
    void SetPos(RECT rc);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(LPMSG lpMsg, BOOL& bHandled);

protected:
    void ReleaseControl();
    BOOL DoCreateControl();

protected:
    CLSID m_clsid;
    CString m_sModuleName;
    BOOL m_bCreated;
    BOOL m_bDelayCreate;
    IOleObject* m_pUnk;
    CActiveXCtrl* m_pControl;
    HWND m_hwndHost;
};

} // namespace DuiLib

#endif // __UIACTIVEX_H__
