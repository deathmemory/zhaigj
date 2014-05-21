#pragma once


namespace DuiLib
{
	class CHwndUI:public CLabelUI
	{
	public:
		CHwndUI(void);
		~CHwndUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
		HWND GetUIHwnd();
		void SetUIHwnd(HWND hWnd);
		void SetPos(RECT rc);
		void SetVisible(BOOL bVisible /* = TRUE */ );
	private:

		HWND m_UIHwnd;
	};
};