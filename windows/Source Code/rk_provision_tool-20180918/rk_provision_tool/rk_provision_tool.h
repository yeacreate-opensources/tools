
// rk_provision_tool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Crk_provision_toolApp:
// See rk_provision_tool.cpp for the implementation of this class
//

class Crk_provision_toolApp : public CWinAppEx
{
public:
	Crk_provision_toolApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Crk_provision_toolApp theApp;