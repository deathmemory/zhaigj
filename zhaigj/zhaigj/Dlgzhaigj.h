#pragma once

class CDlgzhaigj : public CSkinDialog
{
public:
	CDlgzhaigj(void);
	~CDlgzhaigj(void);
	// abstract functions
	LPCTSTR	GetWindowClassName() const {	return _T("dmzhaigj");	}
	BOOL	Initialization();
	void	Notify(TNotifyUI& msg);

	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	virtual LRESULT OnSystemCmd(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
};

