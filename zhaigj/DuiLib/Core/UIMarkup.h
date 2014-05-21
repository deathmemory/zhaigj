#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

namespace DuiLib {

enum
{
    XMLFILE_ENCODING_UTF8 = 0,
    XMLFILE_ENCODING_UNICODE = 1,
    XMLFILE_ENCODING_ASNI = 2,
};

class CMarkup;
class CMarkupNode;


class UILIB_API CMarkup
{
    friend class CMarkupNode;
public:
    CMarkup(LPCTSTR pstrXML = NULL);
    ~CMarkup();

    BOOL Load(LPCTSTR pstrXML);
    BOOL LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
    BOOL LoadFromFile(LPCTSTR pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
    void Release();
    BOOL IsValid() const;

    void SetPreserveWhitespace(BOOL bPreserve = TRUE);
    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;

    CMarkupNode GetRoot();

private:
    typedef struct tagXMLELEMENT
    {
        ULONG iStart;
        ULONG iChild;
        ULONG iNext;
        ULONG iParent;
        ULONG iData;
    } XMLELEMENT;

    LPTSTR m_pstrXML;
    XMLELEMENT* m_pElements;
    ULONG m_nElements;
    ULONG m_nReservedElements;
    TCHAR m_szErrorMsg[100];
    TCHAR m_szErrorXML[50];
    BOOL m_bPreserveWhitespace;

private:
    BOOL _Parse();
    BOOL _Parse(LPTSTR& pstrText, ULONG iParent);
    XMLELEMENT* _ReserveElement();
    inline void _SkipWhitespace(LPTSTR& pstr) const;
    inline void _SkipWhitespace(LPCTSTR& pstr) const;
    inline void _SkipIdentifier(LPTSTR& pstr) const;
    inline void _SkipIdentifier(LPCTSTR& pstr) const;
    BOOL _ParseData(LPTSTR& pstrText, LPTSTR& pstrData, char cEnd);
    void _ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest);
    BOOL _ParseAttributes(LPTSTR& pstrText);
    BOOL _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);
};


class UILIB_API CMarkupNode
{
    friend class CMarkup;
private:
    CMarkupNode();
    CMarkupNode(CMarkup* pOwner, int iPos);

public:
    BOOL IsValid() const;

    CMarkupNode GetParent();
    CMarkupNode GetSibling();
    CMarkupNode GetChild();
    CMarkupNode GetChild(LPCTSTR pstrName);

    BOOL HasSiblings() const;
    BOOL HasChildren() const;
    LPCTSTR GetName() const;
    LPCTSTR GetValue() const;

    BOOL HasAttributes();
    BOOL HasAttribute(LPCTSTR pstrName);
    int GetAttributeCount();
    LPCTSTR GetAttributeName(int iIndex);
    LPCTSTR GetAttributeValue(int iIndex);
    LPCTSTR GetAttributeValue(LPCTSTR pstrName);
    BOOL GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax);
    BOOL GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax);

private:
    void _MapAttributes();

    enum { MAX_XML_ATTRIBUTES = 64 };

    typedef struct
    {
        ULONG iName;
        ULONG iValue;
    } XMLATTRIBUTE;

    int m_iPos;
    int m_nAttributes;
    XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
    CMarkup* m_pOwner;
};

} // namespace DuiLib

#endif // __UIMARKUP_H__
