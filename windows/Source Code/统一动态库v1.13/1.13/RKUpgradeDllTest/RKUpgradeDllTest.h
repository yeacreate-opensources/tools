// RKUpgradeDllTest.h : main header file for the RKUPGRADEDLLTEST application
//

#if !defined(AFX_RKUPGRADEDLLTEST_H__4CA8EB91_6344_4853_95CE_7433127C6121__INCLUDED_)
#define AFX_RKUPGRADEDLLTEST_H__4CA8EB91_6344_4853_95CE_7433127C6121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRKUpgradeDllTestApp:
// See RKUpgradeDllTest.cpp for the implementation of this class
//

class CRKUpgradeDllTestApp : public CWinApp
{
public:
	CRKUpgradeDllTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRKUpgradeDllTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRKUpgradeDllTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RKUPGRADEDLLTEST_H__4CA8EB91_6344_4853_95CE_7433127C6121__INCLUDED_)
