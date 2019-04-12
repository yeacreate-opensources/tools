// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AC901EEB_208C_4E43_AAA9_7EC9DCD607EA__INCLUDED_)
#define AFX_STDAFX_H__AC901EEB_208C_4E43_AAA9_7EC9DCD607EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>
#include <math.h>
#include "RKUpgradeDll.h"
#ifdef _DEBUG
#pragma comment(lib,"RKUpgrade_Debug.lib")
#else
#pragma comment(lib,"RKUpgrade.lib")
#endif

#include "cmBase.h"
#ifdef _DEBUG
#pragma comment(lib,"cmMfcLib_vs2008_Debug.lib")
#else
#pragma comment(lib,"cmMfcLib_vs2008.lib")
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AC901EEB_208C_4E43_AAA9_7EC9DCD607EA__INCLUDED_)
