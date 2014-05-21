// zhaigj.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "zhaigj.h"
#include "Dlgzhaigj.h"
#include "WndShadow.h"

#define MAX_LOADSTRING 100

// Global Variables:
CDlgzhaigj dlgzhaigj;
CWndShadow m_wndShadow;

HINSTANCE hInst;								// current instance

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	MyRegisterClass(hInstance);

	CWndShadow::Initialize(hInstance);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZHAIGJ));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceDll(hInstance);
// 	CUIMessageBox::SetBKImage(IDB_PNG_DLG_BK);
// 	CUIMessageBox::SetBtnTextColor(RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0));
// 	CUIMessageBox::SetCloseBtnPos(2,2);
// 	CUIMessageBox::SetCloseBtnSize(27,25);
// 	CUIMessageBox::SetCommBtnSize(100,30);
// 	CUIMessageBox::SetLeftImage(IDB_PNG_DLG_BTN);
// 	CUIMessageBox::SetMidImage(IDB_PNG_DLG_BTN);
// 	CUIMessageBox::SetRightImage(IDB_PNG_DLG_BTN);
// 	CUIMessageBox::SetCloseBtnImage(IDB_PNG_DLG_CLOSE);
// 	CUIMessageBox::SetTitleID(IDB_PNG_TITLE);
// 	CUIMessageBox::SetInfoTextColor(RGB(0,0,0));
	CUIMessageBox::SetTitleTextColor(RGB(0,0,0));
	return 0;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = dlgzhaigj.Create(NULL,_T("dmzhaigj"),WS_VISIBLE|WS_POPUP,/*WS_EX_TOPMOST*/0);
   m_wndShadow.SetSize(0);
   m_wndShadow.Create(hWnd);
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

