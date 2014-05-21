#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once
#include <richedit.h>
#include <richole.h>
#include <textserv.h>
namespace DuiLib {

class CTxtWinHost;

class UILIB_API CRichEditUI : public CContainerUI, public IMessageFilterUI
{
public:
    CRichEditUI();
    ~CRichEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    BOOL IsWantTab();
    void SetWantTab(BOOL bWantTab = TRUE);
    BOOL IsWantReturn();
    void SetWantReturn(BOOL bWantReturn = TRUE);
    BOOL IsWantCtrlReturn();
    void SetWantCtrlReturn(BOOL bWantCtrlReturn = TRUE);
    BOOL IsRich();
    void SetRich(BOOL bRich = TRUE);
    BOOL IsReadOnly();
    void SetReadOnly(BOOL bReadOnly = TRUE);
    BOOL GetWordWrap();
    void SetWordWrap(BOOL bWordWrap = TRUE);
    int GetFont();
    void SetFont(int index);
    void SetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic);
    LONG GetWinStyle();
    void SetWinStyle(LONG lStyle);

	LONG GetEditStyle();
	void SetEditStyle(LONG lStyle);

    DWORD GetTextColor();
    void SetTextColor(DWORD dwTextColor);
    int GetLimitText();
    void SetLimitText(int iChars);
    long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
    CDuiString GetText() const;
    void SetText(LPCTSTR pstrText);
    BOOL GetModify() const;
    void SetModify(BOOL bModified = TRUE) const;
    void GetSel(CHARRANGE &cr) const;
    void GetSel(long& nStartChar, long& nEndChar) const;
    int SetSel(CHARRANGE &cr);
    int SetSel(long nStartChar, long nEndChar);
    void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo);
    void ReplaceSelW(LPCWSTR lpszNewText, BOOL bCanUndo = FALSE);
    CDuiString GetSelText() const;
    int SetSelAll();
    int SetSelNone();
    WORD GetSelectionType() const;
    BOOL GetZoom(int& nNum, int& nDen) const;
    BOOL SetZoom(int nNum, int nDen);
    BOOL SetZoomOff();
    BOOL GetAutoURLDetect() const;
    BOOL SetAutoURLDetect(BOOL bAutoDetect = TRUE);
    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);
    CDuiString GetTextRange(long nStartChar, long nEndChar) const;
    void HideSelection(BOOL bHide = TRUE, BOOL bChangeStyle = FALSE);
    void ScrollCaret();
    int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE);
    int AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE);
    DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
    DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
    BOOL SetWordCharFormat(CHARFORMAT2 &cf);
    DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    BOOL SetParaFormat(PARAFORMAT2 &pf);
    BOOL Redo();
    BOOL Undo();
    void Clear();
    void Copy();
    void Cut();
    void Paste();
    int GetLineCount() const;
    CDuiString GetLine(int nIndex, int nMaxLength) const;
    int LineIndex(int nLine = -1) const;
    int LineLength(int nLine = -1) const;
    BOOL LineScroll(int nLines, int nChars = 0);
	CPoint GetCharPos(long lChar) const;
    long LineFromChar(long nIndex) const;
    CPoint PosFromChar(UINT nChar) const;
    int CharFromPos(CPoint pt) const;
    void EmptyUndoBuffer();
    UINT SetUndoLimit(UINT nLimit);
    long StreamIn(int nFormat, EDITSTREAM &es);
    long StreamOut(int nFormat, EDITSTREAM &es);

	void SetBidioptions(BIDIOPTIONS * bidi);

	BIDIOPTIONS GetBidioptions();

	void SetCanPaste();

	void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0);

	BOOL SetStartIndent(int nSize);

	BOOL SetLineSpace(int nSpace);

	ITextServices * GetTextServices();

	void SetSelFontFormat(TFontInfo * pFont,COLORREF col );

    void DoInit();
    // 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
    // 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	BOOL SetDropAcceptFile(BOOL bAccept);
    virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
    IDropTarget* GetTxDropTarget();
    virtual BOOL OnTxViewChanged();
    virtual void OnTxNotify(DWORD iNotify, void *pv);

    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(LPMSG uMsg, BOOL& bHandled);

	ITextHost* GetTextHost();

	void ScrollLine(int nLine);

protected:
    CTxtWinHost* m_pTwh;
    BOOL m_bVScrollBarFixing;
    BOOL m_bWantTab;
    BOOL m_bWantReturn;
    BOOL m_bWantCtrlReturn;
    BOOL m_bRich;
    BOOL m_bReadOnly;
    BOOL m_bWordWrap;
    DWORD m_dwTextColor;
    int m_iFont;
    int m_iLimitText;
    LONG m_lTwhStyle;
	BOOL m_bInited;
};

} // namespace DuiLib

#endif // __UIRICHEDIT_H__
