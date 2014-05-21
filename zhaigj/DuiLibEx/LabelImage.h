#pragma once
#include "Gif.h"
#define  ID_TIMER_FLASH 2000
class CLabelImage :
	public CControlUI
{
public:
	CLabelImage(void);
	~CLabelImage(void);
	
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;
	
	void DoEvent(TEventUI& event);

	void SetImage(LPCTSTR strPath);
	void SetImage(UINT    nResID,LPCTSTR strType = _T("PNG"));
	
	void Play();

	void Stop();

	void PaintStatusImage(HDC hDC);

	void InitImage(CHAR * pBuff ,DWORD dSize);

	CString		m_strImagePath;

	UINT		m_uResID;

	CString		m_strImageType;

	CGIF		m_PlayGif;

	TImageInfo *m_pCurImage;

	Bitmap *		m_pImage;

	UINT		m_nCurFrame;
};
