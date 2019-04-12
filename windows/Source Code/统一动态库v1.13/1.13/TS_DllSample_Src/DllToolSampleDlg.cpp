// DllToolSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DllToolSample.h"
#include "DllToolSampleDlg.h"
cmLog *g_pLogObject=NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
UINT ThreadScanDevice(LPVOID lpParam)
{
	CDllToolSampleDlg* pMainDlg = (CDllToolSampleDlg*)lpParam;
	pMainDlg->ScanDeviceProc();
	return 0;
}
UINT ThreadWriteData(LPVOID lpParam)
{
	CDllToolSampleDlg* pMainDlg = (CDllToolSampleDlg*)lpParam;
	pMainDlg->WriteDataProc();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CDllToolSampleDlg dialog

CDllToolSampleDlg::CDllToolSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDllToolSampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDllToolSampleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDllToolSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDllToolSampleDlg)
	DDX_Control(pDX, IDC_STATIC_DEVICE, m_labelDevice);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDllToolSampleDlg, CDialog)
	//{{AFX_MSG_MAP(CDllToolSampleDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_SN_WRITE, OnBtnSnWrite)
	ON_BN_CLICKED(IDC_BTN_IMEI_WRITE, OnBtnImeiWrite)
	ON_BN_CLICKED(IDC_BTN_MAC_WRITE, OnBtnMacWrite)
	ON_BN_CLICKED(IDC_BTN_BT_WRITE, OnBtnBtWrite)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_BROWSE, OnBtnCustomBrowse)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_WRITE, OnBtnCustomWrite)
	ON_BN_CLICKED(IDC_BTN_ALL_WRITE, OnBtnAllWrite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllToolSampleDlg message handlers

BOOL CDllToolSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_strModulePath = cmPath::GetModulePath();
	m_strLogPath = m_strModulePath + _T("Log\\");
	

	if ( !cmFile::IsExisted(m_strLogPath) )
	{
		CreateDirectory(m_strLogPath,NULL);
	}
	
	BOOL bRet;
	g_pLogObject = new cmLog(m_strLogPath,bRet);
	if (bRet)
	{
		g_pLogObject->LogFileName = _T("AppLog");
	}
	else
	{
		if (g_pLogObject)
		{
			delete g_pLogObject;
			g_pLogObject = NULL;
		}
	}

	m_bUpgradeDllInitOK = FALSE;
	m_bTerminated = FALSE;
	m_pLangObject = NULL;
	m_pScanEventObject = NULL;
	m_pScanThreadObject = NULL;
	m_curAction = EM_ACTION_NONE;

	if (LoadConfig())
	{
		m_pLangObject = new cmMultiLanguage(m_LangPath,m_langProp,g_pLogObject,bRet);
		if (bRet)
		{
			//set window interface
			m_pLangObject->CurrentLang = m_curLang;
			STRUCT_DIALOG_PROPERTY prop;
			prop.hwnd = m_hWnd;
			prop.id = IDD_DLLTOOLSAMPLE_DIALOG;
			m_pLangObject->AddDialogProp(prop);

			bRet = m_pLangObject->SetLanguageString();

			//Initialize RKupgrade dll
			INIT_DEV_INFO InitDevInfo;
			INIT_LOG_INFO InitLogInfo;
			INIT_CALLBACK_INFO InitCallbackInfo;
			
			InitDevInfo.bScan4FsUsb = m_bSupportFullUsb;
			InitDevInfo.emSupportDevice = m_emDeviceType;
			InitDevInfo.uiRockMscTimeout = m_mscTimeout;
			InitDevInfo.uiRockusbTimeout = m_rockusbTimeout;
			InitDevInfo.usRockMscPid = m_mscPid;
			InitDevInfo.usRockMscVid = m_mscVid;
			InitDevInfo.usRockusbPid = 0;
			InitDevInfo.usRockusbVid = 0;
			
			InitLogInfo.bLogEnable = m_bLogOn;
			InitLogInfo.lpszLogPathName = (LPTSTR)(LPCTSTR)m_strLogPath;
			
			InitCallbackInfo.pProgressPromptProc = NULL;//you can set it to ProgressPromptProc for showing upgrade info;
			InitCallbackInfo.pUpgradeStepPromptProc = NULL;//you can set it to UpgradeStepPromptProc for showing progress info;
			
			m_bUpgradeDllInitOK = RK_Initialize(InitDevInfo, InitLogInfo, InitCallbackInfo);
			if (!m_bUpgradeDllInitOK)
			{
				MessageBox(_T("Initialize RKUpgrade dll failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
			}
			else
			{
				m_pScanThreadObject = AfxBeginThread(ThreadScanDevice,(LPVOID)this);
			}
		}
		else
		{
			MessageBox(_T("Loading Multi Language failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		}
		//init sn edit
		bRet = m_font.CreateFont(-20,8,0,0,10,FALSE,FALSE,0,  
			ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("Arial"));
		
		GetDlgItem(IDC_EDT_SN)->SetFont(&m_font);
		GetDlgItem(IDC_EDT_MAC)->SetFont(&m_font);
		GetDlgItem(IDC_EDT_BT)->SetFont(&m_font);
		GetDlgItem(IDC_EDT_IMEI)->SetFont(&m_font);
		GetDlgItem(IDC_EDT_CUSTOM_FILE)->SetFont(&m_font);
		GetDlgItem(IDC_EDT_CUSTOM_OFFSET)->SetFont(&m_font);
	}
	else
	{
		MessageBox(_T("Loading config file failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
	}

	m_labelDevice.SetParent(this);
	m_labelDevice.SetFontSize(15).SetFontBold(TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDllToolSampleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDllToolSampleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CDllToolSampleDlg::LoadConfig()
{
	CString strConfigPath;
	strConfigPath = m_strModulePath + _T("config.ini");
	if (!cmFile::IsExisted(strConfigPath))
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->PathFileExists failed"));
		}
		return FALSE;
	}
	bool bRet;
	bRet = m_iniConfig.Load((LPTSTR)(LPCTSTR)strConfigPath);
	if (!bRet)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->Load file failed"));
		}
		return FALSE;
	}
	cmIniSection *pSection;
	TCHAR tzLangSection[]=_T("Language");
	pSection = m_iniConfig.GetSection(tzLangSection);
	if (!pSection)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetSection Language failed"));
		}
		return FALSE;
	}
 	CString strValue;
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("Kinds")).c_str() );
	int langKinds;
	langKinds = cmNumString::StrToInt32(strValue);
	if (langKinds<=0)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetKeyValue kinds failed"));
		}
		return FALSE;
	}

	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("Selected")).c_str() );
	int curLang;
	curLang = cmNumString::StrToInt32(strValue);
	if (curLang<=0)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetKeyValue Selected failed"));
		}
		return FALSE;
	}
	m_curLang = curLang-1;
	
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("LangPath")).c_str() );
	m_LangPath = cmPath::FixPath(m_strModulePath + strValue);

	if (!cmFile::IsExisted(m_LangPath))
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->PathFileExists LangPath failed"));
		}
		return FALSE;
	}
	int i;
	CString strKey;
	STRUCT_LANG_PROPERTY langProp;
	for (i=0;i<langKinds;i++)
	{
		strKey.Format(_T("Lang%dFile"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		if (!cmFile::IsExisted(m_LangPath+strValue))
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("LoadConfig-->PathFileExists %s failed"),strValue);
			}
			return FALSE;
		}
		langProp.strFileName = strValue;
	
		strKey.Format(_T("Lang%dFontName"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		langProp.strFontName = strValue;

		strKey.Format(_T("Lang%dFontSize"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		langProp.uiFontSize = cmNumString::StrToInt32(strValue);
		if (langProp.uiFontSize<=0)
		{
			langProp.uiFontSize = 9;
		}
		m_langProp.push_back(langProp);
	}
	TCHAR tzSystemSection[]=_T("System");
	pSection = m_iniConfig.GetSection(tzSystemSection);
	if (!pSection)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetSection System failed"));
		}
		return FALSE;
	}
	
	int nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("SUPPORT_DEVICE")).c_str() );	
	nValue = cmNumString::StrToULong(strValue,0);

	if (nValue<=0)
	{
		m_emDeviceType = 0;
	}
	else
		m_emDeviceType = nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MSC_VID")).c_str() );	
	nValue = cmNumString::StrToULong(strValue,0);

	if (nValue<=0)
	{
		m_mscVid = 0;
	}
	else
		m_mscVid = nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MSC_PID")).c_str() );	
	nValue = cmNumString::StrToULong(strValue,0);

	if (nValue<=0)
	{
		m_mscPid = 0;
	}
	else
		m_mscPid = nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("IS_SUPPORT_FULL_USB")).c_str() );	
	if (strValue.CompareNoCase(_T("1"))==0)
	{
		m_bSupportFullUsb = TRUE;
	}
	else
		m_bSupportFullUsb = FALSE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("IS_LOG_ON")).c_str() );	
	if (strValue.CompareNoCase(_T("1"))==0)
	{
		m_bLogOn = TRUE;
	}
	else
		m_bLogOn = FALSE;


	m_mscTimeout = cmNumString::StrToInt32(pSection->GetKeyValue(_T("MSC_TIMEOUT")).c_str());
	m_rockusbTimeout = cmNumString::StrToInt32(pSection->GetKeyValue(_T("ROCKUSB_TIMEOUT")).c_str());

	if (m_mscTimeout<=0)
	{
		m_mscTimeout = 20;
	}
	if (m_rockusbTimeout<=0)
	{
		m_rockusbTimeout = 20;
	}

	return TRUE;
}
void CDllToolSampleDlg::ScanDeviceProc()
{
	PSTRUCT_DEVICE_DESC pDevs=NULL;
	m_pScanEventObject = new CEvent(FALSE,TRUE);
	m_pScanEventObject->ResetEvent();
	m_nDeviceCount = 0;
	CString strOneDevice,strFormat;
	strFormat = GetLocalString(_T("INFO_ONE_DEVICE"));
	while (!m_bTerminated)
	{
		m_csScanLock.Lock();
		m_nDeviceCount=RK_ScanDevice(&pDevs);
		m_csScanLock.Unlock();
		m_bExistMsc = FALSE;
		m_bExistAdb = FALSE;
		if (m_nDeviceCount==0)
		{
			m_labelDevice.SetText(GetLocalString(_T("INFO_NO_DEVICE")));
		}
		else if (m_nDeviceCount==1)
		{
			if (pDevs[0].emUsbType==0x04)
			{
				m_bExistMsc = TRUE;
				strOneDevice.Format(strFormat,_T("MSC"));
				m_labelDevice.SetText(strOneDevice);
			}
			else if (pDevs[0].emUsbType==0x8)
			{
				m_bExistAdb = TRUE;
				strOneDevice.Format(strFormat,_T("ADB"));
				m_labelDevice.SetText(strOneDevice);
			}
			else if (pDevs[0].emUsbType==0x10)
			{
				strOneDevice.Format(strFormat,_T("MTP"));
				m_labelDevice.SetText(strOneDevice);
			}
			else if (pDevs[0].emUsbType==0x20)
			{
				strOneDevice.Format(strFormat,_T("UVC"));
				m_labelDevice.SetText(strOneDevice);
			}
			else if (pDevs[0].emUsbType==0x1)
			{
				strOneDevice.Format(strFormat,_T("MASKROM"));
				m_labelDevice.SetText(strOneDevice);
			}
			else
			{
				strOneDevice.Format(strFormat,_T("LOADER"));
				m_labelDevice.SetText(strOneDevice);
			}
		}
		else if (m_nDeviceCount>1)
		{
			m_labelDevice.SetText(GetLocalString(_T("INFO_MANY_DEVICE")));
		}
		Sleep(200);
	}
	
	m_pScanEventObject->SetEvent();
}
CString CDllToolSampleDlg::GetLocalString(CString strKey)
{
	BOOL bRet;
	CString strValue ;
	bRet = m_pLangObject->GetString(_T("Prompt"),strKey,strValue);
	if (!bRet)
	{
		strValue = _T("");
	}
	return strValue;
}

void CDllToolSampleDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (m_curAction!=EM_ACTION_NONE)
	{
		MessageBox(GetLocalString(_T("ERROR_ACTION_RUN")),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	
	m_bTerminated = TRUE;
	if (m_pScanThreadObject)
	{
		MSG msg;
		DWORD dwRet;
		while (TRUE)
		{
			dwRet = MsgWaitForMultipleObjects(1, &m_pScanEventObject->m_hObject,FALSE, 10000, QS_ALLINPUT);
			if(WAIT_OBJECT_0 ==dwRet )
			{
				break;
			}
			else if( (WAIT_OBJECT_0+1)==dwRet )
			{
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				
			}
			else if (WAIT_TIMEOUT==dwRet)
			{
				TerminateThread(m_pScanThreadObject->m_hThread,0);
				break;
			}
		}//end while
		m_pScanThreadObject = NULL;
		delete m_pScanEventObject;
		m_pScanEventObject = NULL;
	}

	if (m_pLangObject)
	{
		delete m_pLangObject;
		m_pLangObject = NULL;
	}
	if (g_pLogObject)
	{
		delete g_pLogObject;
		g_pLogObject = NULL;
	}
	if (m_bUpgradeDllInitOK)
	{
		RK_Uninitialize();
	}
	CDialog::OnClose();
}
VOID CDllToolSampleDlg::EnableCtrl()
{
	GetDlgItem(IDC_CHK_SN)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_MAC)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_BT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_IMEI)->EnableWindow(TRUE);

	GetDlgItem(IDC_BTN_SN_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_MAC_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_BT_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_IMEI_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CUSTOM_BROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CUSTOM_WRITE)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_EDT_SN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDT_MAC)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDT_BT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDT_IMEI)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDT_CUSTOM_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDT_CUSTOM_OFFSET)->EnableWindow(TRUE);
}

VOID CDllToolSampleDlg::DisableCtrl()
{
	GetDlgItem(IDC_CHK_SN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_MAC)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_BT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_IMEI)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_BTN_SN_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MAC_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_BT_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_IMEI_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CUSTOM_BROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CUSTOM_WRITE)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_EDT_SN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_MAC)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_BT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_IMEI)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_CUSTOM_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_CUSTOM_OFFSET)->EnableWindow(FALSE);
}

BOOL CDllToolSampleDlg::WriteDataProc()
{
	CString strImei,strError;
	BOOL bRet,bSuccess=FALSE;
	int nCount=0;
	m_csScanLock.Lock();
	if (m_nDeviceCount==0)
	{
		strError = GetLocalString(_T("ERROR_NO_DEVICE"));
		goto Exit_WriteData;
	}
	if (m_nDeviceCount>1)
	{
		strError = GetLocalString(_T("ERROR_MANY_DEVICE"));
		goto Exit_WriteData;
	}
	
	if (m_bExistMsc)
	{
		if (!RK_SwitchToRockusb())
		{
			strError = GetLocalString(_T("ERROR_SWITCH_DEVICE"));
			goto Exit_WriteData;
		}
	}
	/*else if (m_bExistAdb)
	{
		m_csScanLock.Unlock();
		bLock = FALSE;
		if (!RK_AdbToRockusb())
		{
			strError = GetLocalString(_T("ERROR_SWITCH_DEVICE"));
			goto Exit_WriteData;
		}
		m_csScanLock.Lock();
		bLock = TRUE;
	}*/
	if (m_curAction==EM_ACTION_WRITE_SN)
	{
		bRet = RK_WriteSN(m_sn,strlen((char *)m_sn));
	}
	else if (m_curAction==EM_ACTION_WRITE_MAC)
	{
		bRet = RK_WriteMAC(m_mac);
	}
	else if (m_curAction==EM_ACTION_WRITE_BT)
	{
		bRet = RK_WriteBT(m_bt);
	}
	else if (m_curAction==EM_ACTION_WRITE_IMEI)
	{
		bRet = RK_WriteIMEI(m_imei);
	}
	else if (m_curAction==EM_ACTION_WRITE_ALL)
	{
		bRet = RK_WriteAllInfo(m_all);
	}
	else
	{
		bRet = RK_WriteCustomData(m_custom+m_uiCustomOffset,m_uiCustomOffset,m_uiCustomLen);
	}
	if (!bRet)
	{
		strError = GetLocalString(_T("ERROR_WRITEDATA_FAIL"));
		goto Exit_WriteData;
	}

	bSuccess = TRUE;
Exit_WriteData:

	m_csScanLock.Unlock();

	EnableCtrl();
	m_curAction = EM_ACTION_NONE;
	if (bSuccess)
	{
		MessageBox(GetLocalString(_T("INFO_WRITEDATA_OK")));
	}
	else
	{
		MessageBox(strError,NULL,MB_ICONERROR|MB_OK);	
	}
	return bSuccess;
}

void CDllToolSampleDlg::OnBtnSnWrite() 
{
	// TODO: Add your control notification handler code here
	
	if (!GetSnData())
	{
		return;
	}
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_SN;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
	
}

void CDllToolSampleDlg::OnBtnImeiWrite() 
{
	// TODO: Add your control notification handler code here
	if (!GetImeiData())
	{
		return;
	}
	
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_IMEI;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
}

void CDllToolSampleDlg::OnBtnMacWrite() 
{
	// TODO: Add your control notification handler code here
	if (!GetMacData())
	{
		return;
	}
	
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_MAC;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
}
BOOL CDllToolSampleDlg::HexStrToBytes(CString strHex,PBYTE pBuf,int &nBufLen)
{
	CString strValue;
	int nStrLen,i,nCount;
	if (nBufLen<=0)
	{
		return FALSE;
	}
	nCount=i=0;nStrLen=strHex.GetLength();
	while((i+2)<=nStrLen)
	{
		strValue = strHex.Mid(i,2);
		pBuf[nCount] = cmNumString::StrToULong(strValue,16);
		nCount++;
		i+=2;
		if (nCount>=nBufLen)
		{
			break;
		}
	}
	nBufLen = nCount;
	return TRUE;
}

void CDllToolSampleDlg::OnBtnBtWrite() 
{
	// TODO: Add your control notification handler code here
	
	if (!GetBtData())
	{
		return;
	}
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_BT;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
}
BOOL CDllToolSampleDlg::GetSnData()
{
	CString strSn;
	PBYTE pSn=NULL;
	int nSnLen;
	GetDlgItemText(IDC_EDT_SN,strSn);
	if (strSn.IsEmpty())
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_SN")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (!cmStrCode::UnicodeToAnsi((LPSTR &)pSn,nSnLen,strSn))
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_SN")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (nSnLen>30)
	{
		pSn[30] = 0;
	}
	memset(m_sn,0,sizeof(m_sn));
	strcpy((char *)m_sn,(char *)pSn);
	delete []pSn;
	pSn = NULL;
	return TRUE;
}
BOOL CDllToolSampleDlg::GetMacData()
{
	CString strMac;
	int nMacLen;
	GetDlgItemText(IDC_EDT_MAC,strMac);
	if (strMac.IsEmpty())
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_MAC")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	strMac.Remove(_T('-'));
	if (strMac.GetLength()!=12)
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_MAC")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (!cmNumString::IsHexString(strMac))
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_MAC")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	
	memset(m_mac,0,sizeof(m_mac));
	nMacLen = 6;
	HexStrToBytes(strMac,m_mac,nMacLen);
	return TRUE;
}
BOOL CDllToolSampleDlg::GetBtData()
{
	CString strBT;
	int nBTLen;
	GetDlgItemText(IDC_EDT_BT,strBT);
	if (strBT.IsEmpty())
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_BT")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	strBT.Remove(_T('-'));
	if (strBT.GetLength()!=12)
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_BT")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (!cmNumString::IsHexString(strBT))
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_BT")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	
	memset(m_bt,0,sizeof(m_bt));
	nBTLen = 6;
	HexStrToBytes(strBT,m_bt,nBTLen);
	return TRUE;
}
BOOL CDllToolSampleDlg::GetImeiData()
{
	CString strImei;
	PBYTE pImei=NULL;
	int nImeiLen;
	GetDlgItemText(IDC_EDT_IMEI,strImei);
	if (strImei.IsEmpty())
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_IMEI")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (strImei.GetLength()!=15)
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_IMEI")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if (!cmStrCode::UnicodeToAnsi((LPSTR &)pImei,nImeiLen,strImei))
	{
		MessageBox(GetLocalString(_T("ERROR_INVALID_IMEI")),NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	
	memset(m_imei,0,sizeof(m_imei));
	memcpy(m_imei,pImei,15);
	delete []pImei;
	pImei = NULL;
	return TRUE;
}

void CDllToolSampleDlg::OnBtnCustomBrowse() 
{
	// TODO: Add your control notification handler code here
	BOOL bRet;
	CString strFile,strFilter;
	strFilter = _T("Custom File(*.bin)|*.bin|All File(*.*)|*.*||");
	bRet = cmCommonDlg::OpenDialog(strFile,strFilter);
	if (bRet)
	{
		SetDlgItemText(IDC_EDT_CUSTOM_FILE,strFile);
	}
}

void CDllToolSampleDlg::OnBtnCustomWrite() 
{
	// TODO: Add your control notification handler code here
	CString strFile,strOffset;
	CFile file;
	BOOL bRet;
	GetDlgItemText(IDC_EDT_CUSTOM_FILE,strFile);
	GetDlgItemText(IDC_EDT_CUSTOM_OFFSET,strOffset);
	if (!cmFile::IsExisted(strFile))
	{
		MessageBox(GetLocalString(_T("ERROR_CUSTOM_FILE")),NULL,MB_ICONERROR|MB_OK);
		return;
	}
	m_uiCustomOffset = cmNumString::StrToULong(strOffset,0);
	if ((m_uiCustomOffset<0)||(m_uiCustomOffset>511))
	{
		MessageBox(GetLocalString(_T("ERROR_CUSTOM_OFFSET")),NULL,MB_ICONERROR|MB_OK);
		return;
	}
	bRet = file.Open(strFile,CFile::modeRead|CFile::typeBinary);
	if (!bRet)
	{
		MessageBox(GetLocalString(_T("ERROR_OPEN_CUSTOM_FILE")),NULL,MB_ICONERROR|MB_OK);
		return;
	}
	m_uiCustomLen = 512-m_uiCustomOffset;
	if (m_uiCustomLen>file.GetLength())
	{
		m_uiCustomLen = file.GetLength();
	}
	file.Read(m_custom+m_uiCustomOffset,m_uiCustomLen);
	file.Close();
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_CUSTOM;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
}

void CDllToolSampleDlg::OnBtnAllWrite() 
{
	// TODO: Add your control notification handler code here
	BOOL bCheck,bRet;
	m_all.pReserved = NULL;
	m_all.btReservedSize = 0;
	m_all.pUid = NULL;
	bCheck = ((CButton *)GetDlgItem(IDC_CHK_SN))->GetCheck();
	if (bCheck)
	{
		bRet = GetSnData();
		if (!bRet)
		{
			return;
		}
		m_all.pSn = m_sn;
		m_all.usSnSize = strlen((char *)m_sn);
	}
	else
	{
		m_all.pSn = NULL;
		m_all.usSnSize = 0;
	}
	bCheck = ((CButton *)GetDlgItem(IDC_CHK_MAC))->GetCheck();
	if (bCheck)
	{
		bRet = GetMacData();
		if (!bRet)
		{
			return;
		}
		m_all.pMac = m_mac;
		m_all.btMacSize = 6;
	}
	else
	{
		m_all.pMac = NULL;
	}
	bCheck = ((CButton *)GetDlgItem(IDC_CHK_BT))->GetCheck();
	if (bCheck)
	{
		bRet = GetBtData();
		if (!bRet)
		{
			return;
		}
		m_all.pBt = m_bt;
		m_all.btBlueToothSize = 6;
	}
	else
	{
		m_all.pBt = NULL;
	}
	bCheck = ((CButton *)GetDlgItem(IDC_CHK_IMEI))->GetCheck();
	if (bCheck)
	{
		bRet = GetImeiData();
		if (!bRet)
		{
			return;
		}
		m_all.pImei = m_imei;
		m_all.btImeiSize = 15;
	}
	else
	{
		m_all.pImei = NULL;
	}
	
	if ((!m_all.pSn)&&(!m_all.pMac)&&(!m_all.pBt)&&(!m_all.pImei))
	{
		return;
	}
	DisableCtrl();
	m_curAction = EM_ACTION_WRITE_ALL;
	AfxBeginThread(ThreadWriteData,(LPVOID)this);
}

BOOL CDllToolSampleDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE )
	{
		return TRUE;
	}
	if ( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
