// DllToolSample.h : main header file for the DLLTOOLSAMPLE application
//

#if !defined(AFX_DLLTOOLSAMPLE_H__64BF1538_8DD9_4D74_8E0D_F8AC264DCA9E__INCLUDED_)
#define AFX_DLLTOOLSAMPLE_H__64BF1538_8DD9_4D74_8E0D_F8AC264DCA9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllToolSampleApp:
// See DllToolSample.cpp for the implementation of this class
//

class CDllToolSampleApp : public CWinApp
{
public:
	CDllToolSampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllToolSampleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDllToolSampleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLTOOLSAMPLE_H__64BF1538_8DD9_4D74_8E0D_F8AC264DCA9E__INCLUDED_)
