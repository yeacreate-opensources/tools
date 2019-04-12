
// rk_provision_toolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "rk_provision_tool.h"
#include "rk_provision_toolDlg.h"
#include "DlgSettings.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CONTROLMAP editcontrolmap[ITEM_WVCNT] = {
	{IDC_EDIT_SN},{IDC_EDIT_WIFI},{IDC_EDIT_LAN},{IDC_EDIT_BT},{IDC_EDIT_IMEI}
};

static CONTROLMAP checkcontrolmap[ITEM_WVCNT] = {
	{IDC_CHECK_SN},{IDC_CHECK_WIFI},{IDC_CHECK_LAN},{IDC_CHECK_BT},{IDC_CHECK_IMEI}
};
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Crk_provision_toolDlg dialog




Crk_provision_toolDlg::Crk_provision_toolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Crk_provision_toolDlg::IDD, pParent), m_pWriteThread(NULL), m_pReadThread(NULL), m_pScanThread(NULL),
	m_bRunning(FALSE), m_bStarted(FALSE), m_bUserStop(FALSE), m_CurCtrlID(-1), m_CurOp(ID_OP_NO), pLog(NULL)
{
	memset(&m_ItemInfo, 0, sizeof(m_ItemInfo));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Crk_provision_toolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SN, m_CBtnEnableSn);
	DDX_Control(pDX, IDC_CHECK_WIFI, m_CBtnEnableWifi);
	DDX_Control(pDX, IDC_CHECK_LAN, m_CBtnEnableLan);
	DDX_Control(pDX, IDC_CHECK_BT, m_CBtnEnableBt);
	DDX_Control(pDX, IDC_CHECK_IMEI, m_CBtnEnableImei);
	DDX_Control(pDX, IDC_STATIC_DEVICES, m_lblDevice);
	DDX_Control(pDX, IDC_PICTURE_DEVICE, m_PicDevice);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listInfo);
}

BEGIN_MESSAGE_MAP(Crk_provision_toolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &Crk_provision_toolDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_READ, &Crk_provision_toolDlg::OnBnClickedButtonRead)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_UPDATE_MSG,&Crk_provision_toolDlg::OnHandleUpdateMsg)
	ON_COMMAND(ID_MENU_ABOUT, &Crk_provision_toolDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_SETTINGS, &Crk_provision_toolDlg::OnMenuSettings)
	ON_BN_CLICKED(IDC_CHECK_SN, &Crk_provision_toolDlg::OnBnClickedCheckSn)
	ON_BN_CLICKED(IDC_CHECK_WIFI, &Crk_provision_toolDlg::OnBnClickedCheckWifi)
	ON_BN_CLICKED(IDC_CHECK_LAN, &Crk_provision_toolDlg::OnBnClickedCheckLan)
	ON_BN_CLICKED(IDC_CHECK_BT, &Crk_provision_toolDlg::OnBnClickedCheckBt)
	ON_BN_CLICKED(IDC_CHECK_IMEI, &Crk_provision_toolDlg::OnBnClickedCheckImei)
	ON_BN_CLICKED(IDC_CHECK_SINGLE, &Crk_provision_toolDlg::OnBnClickedCheckSingle)
	ON_EN_SETFOCUS(IDC_EDIT_SN, &Crk_provision_toolDlg::OnEnSetfocusEditSn)
	ON_EN_SETFOCUS(IDC_EDIT_WIFI, &Crk_provision_toolDlg::OnEnSetfocusEditWifi)
	ON_EN_SETFOCUS(IDC_EDIT_BT, &Crk_provision_toolDlg::OnEnSetfocusEditBt)
	ON_EN_SETFOCUS(IDC_EDIT_IMEI, &Crk_provision_toolDlg::OnEnSetfocusEditImei)
	ON_EN_SETFOCUS(IDC_EDIT_LAN, &Crk_provision_toolDlg::OnEnSetfocusEditLan)
	ON_BN_CLICKED(IDC_CHECK_LOADER, &Crk_provision_toolDlg::OnBnClickedCheckLoader)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &Crk_provision_toolDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_BROW_LOADER, &Crk_provision_toolDlg::OnBnClickedButtonBrowLoader)
	ON_COMMAND(ID_MENU_LOG, &Crk_provision_toolDlg::OnMenuLog)
	ON_BN_CLICKED(IDC_BUTTON_REBOOT, &Crk_provision_toolDlg::OnBnClickedButtonReboot)
END_MESSAGE_MAP()


UINT ThreadScanDevice(LPVOID lpParam)
{
	Crk_provision_toolDlg *pMainDlg = (Crk_provision_toolDlg*)lpParam;
	pMainDlg->ScanDeviceProc();
	return 0;
}

UINT ThreadWrite(LPVOID lpParam)
{
	Crk_provision_toolDlg *pMainDlg = (Crk_provision_toolDlg*)lpParam;
	pMainDlg->WriteProc();
	return 0;
}

UINT ThreadRead(LPVOID lpParam)
{
	Crk_provision_toolDlg *pMainDlg = (Crk_provision_toolDlg*)lpParam;
	pMainDlg->ReadProc();
	return 0;
}

// Crk_provision_toolDlg message handlers

BOOL Crk_provision_toolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	DWORD dwRet;
	INIT_DEV_INFO InitDevInfo;
	INIT_LOG_INFO InitLogInfo;
	INIT_CALLBACK_INFO InitCallbackInfo;
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	m_bRedLedLight = TRUE;
	m_hGreenLedBitmap = m_hRedLedBitmap = NULL;
	m_hRedLedBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_RED));
	m_hGreenLedBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_GREEN));
	dwRet = GetModuleFileName(NULL, m_strModulePath.GetBuffer(MAX_PATH), MAX_PATH);
	m_strModulePath.ReleaseBuffer();
	if (dwRet > 0) {
		m_strModulePath = m_strModulePath.Left(m_strModulePath.ReverseFind(_T('\\'))+1);
	}

	if(LoadConfig()) {
		wsprintf(m_szLogPath, (LPTSTR)(LPCTSTR)(m_strModulePath + _T("log\\")));
		dwRet = GetFileAttributes(m_szLogPath);
		if (INVALID_FILE_ATTRIBUTES != dwRet && 0 != (dwRet & FILE_ATTRIBUTE_DIRECTORY)) {
		} else {
			CreateDirectory(m_szLogPath, NULL);
		}

		if ( m_Configs.nLogLevel < CLogger::DEBUG_OFF ) {
			pLog = CLogger::StartLog((m_szLogPath + CLogger::TimeStr(true, true)).c_str(), (CLogger::DEBUG_LEVEL) m_Configs.nLogLevel);
		}
		LDEGMSGW((CLogger::DEBUG_INFO ,TEXT("%s starting...\r\n"), m_LocalLan.GetLanStr(_T("LANG:IDS_TEXT_APPNAME")).c_str()));
		/* UpgradeDll init */
		InitDevInfo.bScan4FsUsb = FALSE;
		InitDevInfo.emSupportDevice = 0;
		InitDevInfo.uiRockMscTimeout = 30;
		InitDevInfo.uiRockusbTimeout = 30;
		InitDevInfo.usRockMscPid = 0;
		InitDevInfo.usRockMscVid = 0;
		InitDevInfo.usRockusbPid = 0;
		InitDevInfo.usRockusbVid = 0;
		InitLogInfo.bLogEnable = TRUE;
		InitLogInfo.lpszLogPathName = m_szLogPath;
		InitCallbackInfo.pProgressPromptProc = NULL;//you can set it to ProgressPromptProc for showing upgrade info;
		InitCallbackInfo.pUpgradeStepPromptProc = NULL;//you can set it to UpgradeStepPromptProc for showing progress info;
		m_bUpgradeDllInitOK = RK_Initialize(InitDevInfo, InitLogInfo, InitCallbackInfo);
		if (!m_bUpgradeDllInitOK) {
			MessageBox(_T("Initialize RKUpgrade dll failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		} else {
			m_pScanThread = AfxBeginThread(ThreadScanDevice,(LPVOID)this);
		}
	} else {
		AfxMessageBox(_T("Loader Config fail"));
		exit(0);
	}
	OnUpdateCtrl();
	SetControlsFocus();
#if 0
	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);
#endif
	return FALSE;  // return TRUE  unless you set the focus to a control
}

void Crk_provision_toolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Crk_provision_toolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CPaintDC dc(this); // device context for painting
		HBITMAP hLedBitmap;
		if (m_bRedLedLight) {
			hLedBitmap = m_hRedLedBitmap;
		} else {
			hLedBitmap = m_hGreenLedBitmap;
		}
		if (hLedBitmap) {
			int nDstWidth,nDstHeight;
			RECT dstClientRect;
			CDC ImageDC;
			HGDIOBJ hOldGdiObject;
			BITMAP bmpStruct;
			BOOL bRet;

			CPaintDC dc(GetDlgItem(IDC_PICTURE_DEVICE));
			ImageDC.CreateCompatibleDC(&dc);
			hOldGdiObject = ImageDC.SelectObject(hLedBitmap);
			dc.GetWindow()->GetClientRect(&dstClientRect);
			nDstHeight = dstClientRect.bottom - dstClientRect.top;
			nDstWidth = dstClientRect.right-dstClientRect.left;
			GetObject(hLedBitmap,sizeof(BITMAP),&bmpStruct);

			bRet = TransparentBlt(dc.m_hDC, 0, 0, nDstWidth, nDstHeight, ImageDC.m_hDC, 0, 0, bmpStruct.bmWidth, bmpStruct.bmHeight, RGB(255,255,255));
			ImageDC.SelectObject(hOldGdiObject);
		}


		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Crk_provision_toolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Crk_provision_toolDlg::OnStartReadLocked(BOOL bDoSingle)
{
	CString strPromt;
	CWinThread *pThread = NULL;
	int idx;

	for(idx = 0; idx < ITEM_WVCNT; idx++) {
		if(m_Configs.iinfo[idx].bEnable) {
			break;
		}
	}

	if (ITEM_WVCNT == idx) {
		MessageBox(GetLocalString(_T("IDS_ERROR_NO_READ")).c_str(),_T("Error"),MB_OK|MB_ICONERROR);
		goto OnStartReadExit;
	}

	if (m_Configs.bSetLoader) {
		GetDlgItemText(IDC_EDIT_LOADER, m_strLoader);
		if (m_strLoader.IsEmpty()) {
			strPromt = GetLocalString(_T("IDS_LOADER_NOSEL_FAIL")).c_str();
			goto OnStartReadExit;
		}
	}

	if(!m_Configs.bDebug) {/* if debug, no test devices exist or not */
		m_csScanLock.Lock();
		if (m_nDeviceCount < 1) {
			if (bDoSingle) {
				strPromt = GetLocalString(_T("IDS_ERROR_NO_USB_WRITE")).c_str();
			}
			m_csScanLock.Unlock();
			goto OnStartReadExit;
		}
		m_csScanLock.Unlock();
	}

	m_pReadThread = pThread = AfxBeginThread(ThreadRead, (LPVOID)this);
	if (!pThread)
		goto OnStartReadExit;
	return TRUE;
OnStartReadExit:
	if(!strPromt.IsEmpty() && bDoSingle ) {
		MessageBox(strPromt, GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(), MB_OK|MB_ICONERROR);
	}
	if (bDoSingle) {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_FAIL);
	}
	return FALSE;
}

BOOL Crk_provision_toolDlg::CheckItemValid(int id)
{
	CString strPromt;

#if 0
	if( 0 == m_Configs.iinfo[id].iMode) {
		GetDlgItemText(editcontrolmap[id].id, m_strCurItemValue[id] );
	} else if (1 == m_Configs.iinfo[id].iMode) {/* Auto */
		if (m_Configs.iinfo[id].nRemainCount > 0) {
			m_strCurItemValue[id] = m_Configs.iinfo[id].strItemCurrent.c_str();
		} else {
			return FALSE;
		}
	} else if(2 == m_Configs.iinfo[id].iMode) { /* file */
	}
#endif
	return TRUE;

}

void Crk_provision_toolDlg::FreeBuffer(PDLGITEMINFO pItemInfo)
{
	if (pItemInfo->pData) {
		delete[] pItemInfo->pData;
		pItemInfo->pData= NULL;
	}
	pItemInfo->size = 0;
}

BOOL Crk_provision_toolDlg::AllocBuffer(PDLGITEMINFO pItemInfo, int iItemSize)
{
	if (pItemInfo->pData && pItemInfo->size != iItemSize) {
		delete[] pItemInfo->pData;
		pItemInfo->pData = NULL;
		pItemInfo->size = 0;
	}
	if (!pItemInfo->pData) {
		pItemInfo->pData = new BYTE[iItemSize];
	}
	if (!pItemInfo->pData) {
		return FALSE;
	}
	pItemInfo->size = iItemSize;
	return TRUE;
}

CString Crk_provision_toolDlg::SetItemString(int id, const BYTE *pBuf, int len)
{
	int iLen, i;
	CString Str;
	TCHAR *pszTemp;

	if(_tcsstr(index_id_map[id].name, _T("MAC"))) { /* mac */
		iLen = len*2 + 1;
		pszTemp = Str.GetBuffer(iLen);
		for(i = 0; i < len; i ++ ) {
			pszTemp[2*i] = byte_to_tchar((pBuf[i]>>4)&0xf);
			pszTemp[2*i + 1] = byte_to_tchar((pBuf[i])&0xf);
		}
		pszTemp[i*2] = 0;
		Str.ReleaseBuffer();
	} else if (_tcsstr(index_id_map[id].name, _T("IMEI"))) {
		/* imei is 15 length, sometimes it doesn't null character terminate
		 * We need fix it
		 */
		iLen = ('\0'== (const CHAR )pBuf[len]) ? 0 : 1;
		iLen = CCharToWChar(0, NULL, (const CHAR *)pBuf, len) + iLen;
		pszTemp = Str.GetBuffer(iLen);
		memset(pszTemp, 0, sizeof(TCHAR)*iLen);
		CCharToWChar(pszTemp, iLen, (const CHAR *)pBuf, len);
		Str.ReleaseBuffer();
	} else if (_tcsstr(index_id_map[id].name, _T("SN"))) { /* string convert */
		/* the same as imei
		 */
		iLen = ('\0'== (const CHAR )pBuf[len]) ? 0 : 1;
		iLen = CCharToWChar(0, NULL, (const CHAR *)pBuf, len) + iLen;
		pszTemp = Str.GetBuffer(iLen);
		memset(pszTemp, 0, sizeof(TCHAR)*iLen);
		CCharToWChar(pszTemp, iLen, (const CHAR *)pBuf, len);
		Str.ReleaseBuffer();
	}
	return Str;
}

/* for read, set item to user
 * save read buffer to m_ItemInfo
 */
BOOL Crk_provision_toolDlg::SetItem(int id, BYTE *pBuf, int len)
{
	PDLGITEMINFO pItemInfo = &m_ItemInfo[id];

	if(!AllocBuffer(pItemInfo, len)){
		return FALSE;
	}
	memcpy(pItemInfo->pData, pBuf, len);
	return TRUE;
}

/* for wrtie, get item from config or file
 * then save to m_ItemInfo;
 */
BOOL Crk_provision_toolDlg::GetItem(int id)
{
	/* CString strPromt; **/
	PDLGITEMINFO pItemInfo = &m_ItemInfo[id];
	__int64 int64Imei;
	const TCHAR *pStr;
	CString Str;
	BYTE mac;
	int iItemSize;
	int i;

	if( 0 == m_Configs.iinfo[id].iMode) {
		GetDlgItemText(editcontrolmap[id].id, Str);
		if(Str.IsEmpty()) {
			return FALSE;
		}
	} else if (1 == m_Configs.iinfo[id].iMode) {/* Auto */
		if (m_Configs.iinfo[id].nRemainCount > 0) {
			Str = m_Configs.iinfo[id].strItemCurrent.c_str();
		} else {
			return FALSE;
		}
	} else if(2 == m_Configs.iinfo[id].iMode) { /* file */
		/* get item from file */
	}

	/* convert cstring to byte string */
	if(_tcsstr(index_id_map[id].name, _T("MAC"))) { /* mac */
		if(!check_mac(Str.GetString(), 0)) {
			return FALSE;
		}
		iItemSize = MAC_LEN;
		if (!AllocBuffer(pItemInfo, iItemSize)) {
			return FALSE;
		}
		pStr = Str.GetString();
		for(i = 0; i < iItemSize; i ++) {
			mac = (tchar_to_byte(pStr[2*i]) << 4) | (tchar_to_byte(pStr[2*i + 1]));
			pItemInfo->pData[i]  = mac;
		}
	} else if (_tcsstr(index_id_map[id].name, _T("IMEI"))){
		if(!check_imei(Str.GetString(), 1)) {/* imei 14 bits long */
			return FALSE;
		}
		iItemSize = IMEI_LEN; /* imei write as a string */
		if (!AllocBuffer(pItemInfo, iItemSize)) {
			return FALSE;
		}
		pStr = Str.GetString();
		iItemSize = WCharToChar((CHAR *)pItemInfo->pData, iItemSize, Str.GetString());
		if (IMEI_LEN != iItemSize) { /* including the terminating null character. */
			return FALSE;
		}
		int64Imei = _ttoi64(pStr);
		pItemInfo->pData[iItemSize - 1] = calc_imei_sp(int64Imei) + TEXT('0');
	} else if (_tcsstr(index_id_map[id].name, _T("SN"))){ /* string convert */
		if(!check_sn(Str.GetString(), 0))
			return FALSE;
		iItemSize = WCharToChar(NULL, 0, Str.GetString());
		if (iItemSize <= 0)
			return FALSE;
		if (!AllocBuffer(pItemInfo, iItemSize)) {
			return FALSE;
		}
		iItemSize = WCharToChar((CHAR *)pItemInfo->pData, iItemSize, Str.GetString());
		if (iItemSize != pItemInfo->size) {
			return FALSE;
		}
	}
	return TRUE;

}

BOOL Crk_provision_toolDlg::OnStartWriteLocked(BOOL bDoSingle)
{
	CString strPromt;
	CWinThread *pThread = NULL;
	int idx;

	for(idx = 0; idx < ITEM_WVCNT; idx++) {
		if(m_Configs.iinfo[idx].bEnable) {
			break;
		}
	}

	if(ITEM_WVCNT == idx) {
		MessageBox(GetLocalString(_T("IDS_ERROR_NO_WRITE")).c_str(),_T("Error"),MB_OK|MB_ICONERROR);
		goto OnStartWriteExit;
	}

	if (m_Configs.bSetLoader) {
		GetDlgItemText(IDC_EDIT_LOADER, m_strLoader);
		if (m_strLoader.IsEmpty()) {
			strPromt = GetLocalString(_T("IDS_LOADER_NOSEL_FAIL")).c_str();
			goto OnStartWriteExit;
		}
	}

	/* check item enabled */
	for (idx = 0; idx < ITEM_WVCNT; idx++) {
		if(m_Configs.iinfo[idx].bEnable) {
			if(!GetItem(idx)) {
				strPromt.Format(GetLocalString(_T("IDS_GET_SS_FAIL")).c_str(), index_id_map[idx].name);
				goto OnStartWriteExit;
			}
		}
	}

	/* show enabled item value to control */
	for (idx = 0; idx < ITEM_WVCNT; idx++) {
		if(m_Configs.iinfo[idx].bEnable && 0 != m_Configs.iinfo[idx].iMode ) {
			GetDlgItem(editcontrolmap[idx].id)->SetWindowText(SetItemString(idx, m_ItemInfo[idx].pData, m_ItemInfo[idx].size));
		}
	}

	if (!m_Configs.bDebug) {/* if debug, no test devices exist or not */
		m_csScanLock.Lock();
		if (m_nDeviceCount < 1) {
			if (bDoSingle) {
				strPromt = GetLocalString(_T("IDS_ERROR_NO_USB_WRITE")).c_str();
			}
			m_csScanLock.Unlock();
			goto OnStartWriteExit;
		}
		m_csScanLock.Unlock();
	}

	m_pWriteThread = pThread = AfxBeginThread(ThreadWrite, (LPVOID)this);
	if(!pThread)
		goto OnStartWriteExit;
	return TRUE;
OnStartWriteExit:
	if ((!strPromt.IsEmpty()) && bDoSingle ) {
		MessageBox(strPromt, GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(), MB_OK|MB_ICONERROR);
		//AddPrompt(strPromt,LIST_ERR);
	}
	if (bDoSingle) {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_FAIL);
	}
	return FALSE;
}

/*RW:
 * 0 read operate
 * 1 write operate
 */
void Crk_provision_toolDlg::OnOperateChg(int RW, int status)
{
	int i;
	CButton *pBtn = (CButton *)GetDlgItem( ID_OP_WR == RW ? IDC_BUTTON_READ : IDC_BUTTON_WRITE);

	if (1 == status) { /* busy ,lock all buttons */
		m_CurOp = RW;
		if (!m_Configs.bLockConfig) {
			for (i = 0; i < ITEM_WVCNT; i++) {
				GetDlgItem(checkcontrolmap[i].id)->EnableWindow(FALSE);
			}
		}
		GetDlgItem(IDC_CHECK_SINGLE)->EnableWindow(FALSE);
		pBtn->EnableWindow(FALSE);
	} else { /* 0 == status, idle ,free all buttons */
		m_CurOp = ID_OP_NO;
		if (!m_Configs.bLockConfig) {
			for (i = 0; i < ITEM_WVCNT; i++) {
				GetDlgItem(checkcontrolmap[i].id)->EnableWindow(TRUE);
			}
		}
		GetDlgItem(IDC_CHECK_SINGLE)->EnableWindow(TRUE);
		pBtn->EnableWindow(TRUE);
	}
}

void Crk_provision_toolDlg::OnBnClickedButtonWrite()
{
	m_criSection.Lock();
	if (m_Configs.bSingle) {
		if (m_bRunning) {
			m_bUserStop = TRUE;
			SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_STOPING_BUTTON")).c_str());
		} else {
			if (OnStartWriteLocked(TRUE)) {
				SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
				m_bRunning = TRUE;
				OnOperateChg(ID_OP_WR,1);
			}
		}
	} else {
		if( m_bStarted ) {
			if (m_bRunning) { /* If running, just set user stop flag */
				m_bUserStop = TRUE;
				SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_STOPING_BUTTON")).c_str());
			} else {
				m_bStarted = FALSE;
				SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_WRITE_BUTTON")).c_str());
				OnOperateChg(ID_OP_WR,0);
			}
		} else {
			SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
			m_bStarted = TRUE;
			OnOperateChg(ID_OP_WR,1);
			if(OnStartWriteLocked(FALSE)) {
				m_bRunning = TRUE;
			}
		}
	}
	m_criSection.Unlock();
}

void Crk_provision_toolDlg::ScanDeviceProc()
{
	DWORD dwEvent;
	HANDLE lpHandles[2] = {m_ScanEvent.m_hObject, m_ScanExitEvent.m_hObject};
	bool bDeviceChanged = FALSE;

	PSTRUCT_DEVICE_DESC pDevs = NULL;
	UINT nDeviceCount = 0;

	while (1) {
		dwEvent = ::WaitForMultipleObjects(2, lpHandles, FALSE, 200);
	    switch (dwEvent) {
		case WAIT_OBJECT_0 + 1:
			goto ScanDeviceProc_exit;
		case WAIT_OBJECT_0 + 0:
		case WAIT_TIMEOUT:
			break;
		default: 
			break;
		}
		/* do scan device and update device list */
		nDeviceCount = 0;
		bDeviceChanged = FALSE;
		m_csScanLock.Lock();
		nDeviceCount = RK_ScanDevice(&pDevs);
		if (m_nDeviceCount != nDeviceCount) {
			m_nDeviceCount = nDeviceCount;
			bDeviceChanged = TRUE;
		}
		m_csScanLock.Unlock();

		if (m_nDeviceCount == 0) {
			m_bRedLedLight = TRUE;
			m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_ON")).c_str();
		} else if (m_nDeviceCount == 1) {
			m_bRedLedLight = FALSE;
			if (pDevs[0].emUsbType == 0x04) {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_MSC")).c_str();
			} else if (pDevs[0].emUsbType == 0x8) {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_ADB")).c_str();
			} else if (pDevs[0].emUsbType == 0x10) {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_MTP")).c_str();
			} else if (pDevs[0].emUsbType == 0x20) {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_UVC")).c_str();
			} else if (pDevs[0].emUsbType == 0x1) {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_MASKROM")).c_str();
				m_MaksDeviceEvent.SetEvent();
			} else {
				m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_LOADER")).c_str();
				m_LoaderDeviceEvent.SetEvent();
			}
		} else if (m_nDeviceCount > 1) {
			m_bRedLedLight = FALSE;
			m_strDevice = GetLocalString(_T("IDS_INFO_DEVICE_MANY")).c_str();
		}
		if (bDeviceChanged) {
			PostMessage(WM_UPDATE_MSG, UPDATE_WINDOW, 0);
			if (m_nDeviceCount > 0) {
				PostMessage(WM_UPDATE_MSG, DEVICE_CHG, TRUE);
			} else {
				PostMessage(WM_UPDATE_MSG, DEVICE_CHG, FALSE);
			}
		}
	}
ScanDeviceProc_exit:
	m_ScanExitNotify.SetEvent();
}

void Crk_provision_toolDlg::AddPrompt(CString strPrompt,int flag)
{
	PSTRUCT_LIST_LINE pLine = NULL;
	SYSTEMTIME curTime;

	GetLocalTime( &curTime );
	pLine = new STRUCT_LIST_LINE;
	if (!pLine) {
		return;
	}
	wsprintf(pLine->pszLineText, _T("%02d:%02d:%02d %03d\t%s "),
			 curTime.wHour, curTime.wMinute, curTime.wSecond,
			 curTime.wMilliseconds, (LPCTSTR)strPrompt);
	pLine->flag = flag;
	if (0 == PostMessage(WM_UPDATE_MSG, UPDATE_LIST, (LPARAM)pLine)) {
		delete pLine;
	}
	return;
}

CLogger *Crk_provision_toolDlg::get_log()
{
    return pLog;
}

BOOL Crk_provision_toolDlg::log_enable()
{
	return m_Configs.nLogLevel != CLogger::DEBUG_OFF;
}

BOOL Crk_provision_toolDlg::GetAdbSerialno(CString strDevPath, CString &strSerialno)
{
	BOOL bSuccess=FALSE;
	CString strAdbWinApiDll = m_strModulePath + _T("AdbWinApi.dll");
	TCHAR tzSerial[MAX_PATH];
	unsigned long ulSerialSize = MAX_PATH;
	HMODULE hAdbwinapi = NULL;
	ADBAPIHANDLE hAdbHandle = NULL;
	AdbCloseHandle pAdbCloseHandle = NULL;
	AdbCreateInterfaceByName pAdbCreateInterfaceByName = NULL;
	AdbGetSerialNumber pAdbGetSerialNumber = NULL;
	strSerialno = _T("");

	hAdbwinapi = LoadLibrary(strAdbWinApiDll);
	if (!hAdbwinapi) {
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->LoadLibrary AdbWinApi.dll failed!\r\n"));
		goto Exit_GetAdbSerialno;
	}
	pAdbCloseHandle = (AdbCloseHandle)GetProcAddress( hAdbwinapi, "AdbCloseHandle");
	if (!pAdbCloseHandle) {
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->Get AdbCloseHandle address failed!\r\n"));
	}
	pAdbCreateInterfaceByName = (AdbCreateInterfaceByName)GetProcAddress( hAdbwinapi, "AdbCreateInterfaceByName");
	if (!pAdbCreateInterfaceByName)
	{
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->Get AdbCreateInterfaceByName address failed!"));
		goto Exit_GetAdbSerialno;
	}
	pAdbGetSerialNumber = (AdbGetSerialNumber)GetProcAddress( hAdbwinapi, "AdbGetSerialNumber");
	if (!pAdbGetSerialNumber) {
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->Get AdbGetSerialNumber address failed!"));
		goto Exit_GetAdbSerialno;
	}

	hAdbHandle = pAdbCreateInterfaceByName((LPTSTR)(LPCTSTR)strDevPath);
	if (!hAdbHandle) {
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->AdbCreateInterfaceByName failed,err=%d,dev=%s!", GetLastError(), strDevPath));
		goto Exit_GetAdbSerialno;
	}
	memset(tzSerial,0,sizeof(TCHAR)*MAX_PATH);
	if (!pAdbGetSerialNumber(hAdbHandle,(void *)tzSerial, &ulSerialSize, false)) {
		LDEGMSG((CLogger::DEBUG_INFO,"ERROR:GetAdbSerialno->AdbGetSerialNumber failed!"));
		goto Exit_GetAdbSerialno;
	}
	strSerialno = tzSerial;
	bSuccess = TRUE;
Exit_GetAdbSerialno:
	if (hAdbHandle) {
		pAdbCloseHandle(hAdbHandle);
		hAdbHandle = NULL;
	}
	if (hAdbwinapi) {
		FreeLibrary(hAdbwinapi);
		hAdbwinapi = NULL;
	}
	return bSuccess;
}

BOOL Crk_provision_toolDlg::ReadProc()
{
	BOOL bRet;
	DWORD dwTotalTick;
	CString strPrompt;
	CString strItem;
	int readlen;
	BYTE bItemBufer[128];
	BOOL bSuccess = FALSE;
	DWORD mask = 0;
	BOOL bNeedLoader = FALSE;
	int adb_retry = 3;

	dwTotalTick = GetTickCount();
	if (m_listInfo.GetCount()>0) {
		PostMessage(WM_UPDATE_MSG, UPDATE_LIST, LIST_EMPTY);
	}
	AddPrompt(GetLocalString(_T("IDS_INFO_START_READ")).c_str(), LIST_INFO);
retry:
	if (m_bUserStop)
		goto ReadProc_Exit;
	if (m_strDevice.Find(_T("ADB")) >= 0) {
		m_LoaderDeviceEvent.ResetEvent();
		AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_LOADER")).c_str(), LIST_INFO);
		if (!AdbToRockusb()) {
			AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_LOADER")).c_str(), LIST_INFO);
		}
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_LoaderDeviceEvent.m_hObject, 10*1000)) {
			AddPrompt(GetLocalString(_T("IDS_REBOOT_SUCCESS")).c_str(), LIST_INFO);
		} else {
			AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_LOADER")).c_str(), LIST_ERR);
			adb_retry --;
			if(adb_retry) {
				AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_LOADER_RETRY")).c_str(), LIST_INFO);
				goto retry;
			}
			goto ReadProc_Exit;
		}
	}
	if (m_bUserStop)
		goto ReadProc_Exit;
	if ( m_Configs.bForceMskrom ) {
		if (m_strDevice.Find(_T("MASKROM")) == -1) {
			AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_MASKROM")).c_str(), LIST_INFO);
			LDEGMSGW((CLogger::DEBUG_ERROR,_T("Current device is %s, reset to maskrom..."), (LPCTSTR)m_strDevice));
			m_MaksDeviceEvent.ResetEvent();
			RK_ResetRockusb(3);
			if(WAIT_OBJECT_0 == ::WaitForSingleObject(m_MaksDeviceEvent.m_hObject, 10*1000)) {
				AddPrompt(GetLocalString(_T("IDS_REBOOT_SUCCESS")).c_str(), LIST_INFO);
			} else {
				AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_MASKROM")).c_str(), LIST_ERR);
				goto ReadProc_Exit;
			}
		}
		bNeedLoader = TRUE;
	} else {
		if (m_strDevice.Find(_T("MASKROM")) >= 0) {
			bNeedLoader = TRUE;
		}

	}
	if(m_Configs.bDebug) {
		bNeedLoader = FALSE;
	}

	/* download loader */
	if (m_bUserStop)
		goto ReadProc_Exit;
	if (bNeedLoader) {
		if(m_strLoader.IsEmpty()) {
			strPrompt.Format(GetLocalString(_T("IDS_ERROR_LOADER_EMPTY")).c_str());
			AddPrompt(strPrompt, LIST_ERR);
			LDEGMSGW((CLogger::DEBUG_ERROR, _T("Error:none loader selected")));
			goto ReadProc_Exit;
		}
		AddPrompt(GetLocalString(_T("IDS_SET_LOADER")).c_str(), LIST_INFO);
		bRet = RK_SetFirmware((LPTSTR)(LPCTSTR)m_strLoader);
		if (!bRet) {
			strPrompt.Format(GetLocalString(_T("IDS_SET_LOADER_FAIL")).c_str());
			AddPrompt(strPrompt,LIST_ERR);
			LDEGMSGW((CLogger::DEBUG_ERROR, _T("Error:RK_SetFirmware failed.  Loader path:%s"), (LPCTSTR)m_strLoader));
			goto ReadProc_Exit;
		}
		AddPrompt(GetLocalString(_T("IDS_SET_LOADER_SUCCESS")).c_str(),LIST_INFO);

		AddPrompt(GetLocalString(_T("IDS_DOWN_BOOT")).c_str(), LIST_INFO);
		bRet = RK_DownloadBoot();
		if (!bRet) {
			strPrompt.Format(GetLocalString(_T("IDS_DOWN_BOOT_FAIL")).c_str());
			AddPrompt(strPrompt,LIST_ERR);
			LDEGMSG((CLogger::DEBUG_ERROR,"Error:RK_DownloadBoot failed."));
			goto ReadProc_Exit;
		}
		AddPrompt(GetLocalString(_T("IDS_DOWN_BOOT_SUCCESS")).c_str(),LIST_INFO);
	}
	for (int i = 0; i < ITEM_WVCNT; i ++ ) {
		if (m_bUserStop)
			goto ReadProc_Exit;
		if (m_Configs.iinfo[i].bEnable) {
			strPrompt.Format(GetLocalString(_T("IDS_READ_SS")).c_str(), index_id_map[i].name);
			AddPrompt(strPrompt,LIST_INFO);
			memset(bItemBufer, 0 ,sizeof(bItemBufer));
			if (m_Configs.bDebug) {
				readlen = DebugReadItem(i, bItemBufer, sizeof(bItemBufer));
			} else {
				if (m_Configs.bCompatRw) {
					readlen = CompatReadItem(i, bItemBufer, sizeof(bItemBufer));
				} else {
					if (bNeedLoader) {
						if (m_Configs.bRPMB) {
							readlen = ReadVendorRpmbItem(0, i, bItemBufer, sizeof(bItemBufer));
						} else {
							readlen = ReadItem(i, bItemBufer, sizeof(bItemBufer));
						}
					} else {
						if (m_Configs.bRPMB) {
							readlen = ReadVendorRpmbItem(0, i, bItemBufer, sizeof(bItemBufer));
						} else {
							readlen = ReadItem(i, bItemBufer, sizeof(bItemBufer));
						}
					}
				}
			}
			if (readlen > 0) {
				strItem = SetItemString(i, bItemBufer, readlen);
				SetItem(i, bItemBufer, readlen);
				mask |= 1 << i;
				strPrompt.Format(GetLocalString(_T("IDS_READ_SS_SS_PASS")).c_str(),
								 index_id_map[i].name, strItem.GetString());
				AddPrompt(strPrompt, LIST_INFO);
				LDEGMSGW((CLogger::DEBUG_INFO, _T("Read %s:%s"),
						 index_id_map[i].name,  strItem.GetString()));
			} else {
				strPrompt.Format(GetLocalString(_T("IDS_READ_SS_FAIL")).c_str(), index_id_map[i].name);
				AddPrompt(strPrompt,LIST_ERR);
				LDEGMSG((CLogger::DEBUG_ERROR, "Read %d failed.", index_id_map[i].id));
				goto ReadProc_Exit;
			}
		}
	}
	/* wrting end */
	if (m_bUserStop)
		goto ReadProc_Exit;
	if (m_Configs.bRebootDevice) {
		AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT")).c_str(),LIST_INFO);
		if (!RK_ResetRockusb()) {
			AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT_FAIL")).c_str(),LIST_ERR);
			goto ReadProc_Exit;
		}
		Sleep(1000);//sleep for device offline
		AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT_PASS")).c_str(),LIST_INFO);
	}
	bSuccess = TRUE;

ReadProc_Exit:
	/* every is ok, saveing result and do contact with user */
	if (!m_bUserStop) {
		ShowReadResultOnPass(bSuccess, mask);
	}
	if (m_bUserStop) {
		strPrompt.Format(GetLocalString(_T("IDS_INFO_USER_ABORT")).c_str());
		AddPrompt(strPrompt, LIST_ERR);
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_EMPTY);
	}
	if (bSuccess) {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_PASS);
	} else {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_FAIL);
	}
	strPrompt.Format(GetLocalString(_T("IDS_INFO_TIME_ELAPSE")).c_str(), (GetTickCount() - dwTotalTick )/1000,(GetTickCount() - dwTotalTick )%1000);
	AddPrompt(strPrompt, LIST_TIME);

	/* clear state */
	m_criSection.Lock();
	if ( m_bUserStop || m_Configs.bSingle) { /* update button status */
		/* here means we need exit read/write status
		 */
		PostMessage(WM_UPDATE_MSG, UPDATE_TEST_EXIT, ID_OP_RD);
	}
	if (m_bUserStop && (!m_Configs.bSingle)) {
		m_bStarted = FALSE;
	}
	m_pWriteThread = NULL;
	m_bRunning = FALSE;
	m_bUserStop = FALSE;
	m_criSection.Unlock();
	return bSuccess;
}

BOOL Crk_provision_toolDlg::WriteProc()
{
	BOOL bRet;
	int iRet;
	DWORD dwTotalTick;
	CString strPrompt;
	CString strItem;
	BOOL bSuccess = FALSE;
	DWORD mask = 0;
	BOOL bNeedLoader = FALSE;
	int adb_retry = 3;

	dwTotalTick = GetTickCount();
	if (m_listInfo.GetCount()>0) {
		PostMessage(WM_UPDATE_MSG, UPDATE_LIST, LIST_EMPTY);
	}
	AddPrompt(GetLocalString(_T("IDS_INFO_START_WRITE")).c_str(), LIST_INFO);
retry:
	if (m_bUserStop)
		goto WriteProc_Exit;
	if (m_strDevice.Find(_T("ADB")) >= 0) {
		m_LoaderDeviceEvent.ResetEvent();
		AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_LOADER")).c_str(), LIST_INFO);
		if (!AdbToRockusb()) {
			AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_LOADER")).c_str(),LIST_INFO);
		}
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_LoaderDeviceEvent.m_hObject, 10*1000)) {
			AddPrompt(GetLocalString(_T("IDS_REBOOT_SUCCESS")).c_str(), LIST_INFO);
		} else {
			AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_LOADER")).c_str(), LIST_ERR);
			adb_retry --;
			if (adb_retry) {
				AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_LOADER_RETRY")).c_str(), LIST_INFO);
				goto retry;
			}
			goto WriteProc_Exit;
		}
	}
	if (m_bUserStop)
		goto WriteProc_Exit;
	if ( m_Configs.bForceMskrom ) {
		if (m_strDevice.Find(_T("MASKROM")) == -1) {
			AddPrompt(GetLocalString(_T("IDS_REBOOT_TO_MASKROM")).c_str(), LIST_INFO);
			LDEGMSGW((CLogger::DEBUG_ERROR,_T("Current device is %s, reset to maskrom..."), (LPCTSTR)m_strDevice));
			m_MaksDeviceEvent.ResetEvent();
			RK_ResetRockusb(3);
			if(WAIT_OBJECT_0 == ::WaitForSingleObject(m_MaksDeviceEvent.m_hObject, 10*1000)) {
				AddPrompt(GetLocalString(_T("IDS_REBOOT_SUCCESS")).c_str(), LIST_INFO);
			} else {
				AddPrompt(GetLocalString(_T("IDS_ERROR_REBOOT_MASKROM")).c_str(), LIST_ERR);
				goto WriteProc_Exit;
			}
		}
		bNeedLoader = TRUE;
	} else {
		if (m_strDevice.Find(_T("MASKROM")) >= 0) {
			bNeedLoader = TRUE;
		}

	}
	if (m_Configs.bDebug)
		bNeedLoader = FALSE;
	if (m_bUserStop)
		goto WriteProc_Exit;
	if ( bNeedLoader) {
		AddPrompt(GetLocalString(_T("IDS_SET_LOADER")).c_str(),LIST_INFO);
		bRet = RK_SetFirmware((LPTSTR)(LPCTSTR)m_strLoader);
		if (!bRet) {
			strPrompt.Format(GetLocalString(_T("IDS_SET_LOADER_FAIL")).c_str());
			AddPrompt(strPrompt, LIST_ERR);
			LDEGMSGW((CLogger::DEBUG_ERROR,_T("Error:RK_SetFirmware failed.  Loader path:%s"),(LPCTSTR)m_strLoader));
			goto WriteProc_Exit;
		}
		AddPrompt(GetLocalString(_T("IDS_SET_LOADER_SUCCESS")).c_str(),LIST_INFO);

		AddPrompt(GetLocalString(_T("IDS_DOWN_BOOT")).c_str(), LIST_INFO);
		bRet = RK_DownloadBoot();
		if (!bRet) {
			strPrompt.Format(GetLocalString(_T("IDS_DOWN_BOOT_FAIL")).c_str());
			AddPrompt(strPrompt, LIST_ERR);
			LDEGMSG((CLogger::DEBUG_ERROR,"Error:RK_DownloadBoot failed."));
			goto WriteProc_Exit;
		}
		AddPrompt(GetLocalString(_T("IDS_DOWN_BOOT_SUCCESS")).c_str(),LIST_INFO);
	}

	/* wrting items now */
	for (int i = 0; i < ITEM_WVCNT; i ++ ) {
		if (m_bUserStop)
			goto WriteProc_Exit;
		if (m_Configs.iinfo[i].bEnable) {
			strItem = SetItemString(i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
			strPrompt.Format(GetLocalString(_T("IDS_WRITE_SS_SS")).c_str(), 
							 index_id_map[i].name, strItem.GetString());
			AddPrompt(strPrompt, LIST_INFO);
			LDEGMSGW((CLogger::DEBUG_INFO, _T("writing %s with %s"),
					 index_id_map[i].name, strItem.GetString()));
			if(m_Configs.bDebug) {
				iRet = DebugWriteItem(i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
			} else {
				if(m_Configs.bCompatRw) {
					iRet = CompatWriteItem(i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
				} else {
					if(bNeedLoader) {
						if (m_Configs.bRPMB) {
							iRet = WriteVendorRpmbItem(0, i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
						} else {
							iRet = WriteItem(i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
						}
					} else {
						if (m_Configs.bRPMB) {
							iRet = WriteVendorRpmbItem(0, i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
						} else {
							iRet = WriteItem(i, m_ItemInfo[i].pData, m_ItemInfo[i].size);
						}
					}
				}
			}
			if (iRet) {
				mask |= 1<< i;
				strPrompt.Format(GetLocalString(_T("IDS_WRITE_SS_PASS")).c_str(), index_id_map[i].name);
				AddPrompt(strPrompt,LIST_INFO);
				LDEGMSG((CLogger::DEBUG_INFO, "writing %d successfully!!!", index_id_map[i].id));
			} else {
				strPrompt.Format(GetLocalString(_T("IDS_WRITE_SS_FAIL")).c_str(), index_id_map[i].name);
				AddPrompt(strPrompt,LIST_ERR);
				LDEGMSG((CLogger::DEBUG_INFO, "writing %d fail!!!", index_id_map[i].id));
				goto WriteProc_Exit;
			}
		}
	}

	/* wrting end */
	if (m_bUserStop)
		goto WriteProc_Exit;
	if (m_Configs.bRebootDevice) {
		AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT")).c_str(),LIST_INFO);
		if (!RK_ResetRockusb()) {
			AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT_FAIL")).c_str(),LIST_ERR);
			goto WriteProc_Exit;
		}
		Sleep(1000);//sleep for device offline
		AddPrompt(GetLocalString(_T("IDS_INFO_DEVICE_REBOOT_PASS")).c_str(),LIST_INFO);
	}
	bSuccess = TRUE;
WriteProc_Exit:
	/* every is ok, saveing result and do contact with user */
	if (!m_bUserStop) {
		SaveWriteResultOnPass(bSuccess, mask);
	}
	if (m_bUserStop) {
		strPrompt.Format(GetLocalString(_T("IDS_INFO_USER_ABORT")).c_str());
		AddPrompt(strPrompt, LIST_ERR);
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_EMPTY);
	}
	if (bSuccess) {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_PASS);
	} else {
		PostMessage(WM_UPDATE_MSG, UPDATE_PROMPT, PROMPT_FAIL);
	}
	strPrompt.Format(GetLocalString(_T("IDS_INFO_TIME_ELAPSE")).c_str(), (GetTickCount() - dwTotalTick )/1000,(GetTickCount() - dwTotalTick )%1000);
	AddPrompt(strPrompt, LIST_TIME);

	m_criSection.Lock();
	/* clear state */
	if ( m_bUserStop || m_Configs.bSingle) { /* update button status */
		/* here means we need exit read/write status
		 */
		PostMessage(WM_UPDATE_MSG, UPDATE_TEST_EXIT, ID_OP_WR);
	}
	if (m_bUserStop && (!m_Configs.bSingle)) {
		m_bStarted = FALSE;
	}
	m_pWriteThread = NULL;
	m_bRunning = FALSE;
	m_bUserStop = FALSE;
	m_criSection.Unlock();
	return bSuccess;
}

BOOL Crk_provision_toolDlg::ShowReadResultOnPass(BOOL bPass, DWORD mask)
{
	int i;
	item_info *piinfo;

	m_Configs.iReadRunCount ++;
	if(bPass)
		m_Configs.iReadSuccess ++;

	for (i = 0; i < ITEM_WVCNT; i ++ ) {
		piinfo = &m_Configs.iinfo[i];
		if (piinfo->bEnable && (mask & 1 << i)) { /* enable and need inc */
			PostMessage(WM_UPDATE_MSG, SHOW_RESULT, i);
		}
	}
	return TRUE;
}

BOOL Crk_provision_toolDlg::SaveWriteResultOnPass(BOOL bPass, DWORD mask)
{
	int i;
	item_info *piinfo;
	TCHAR szTmp[128];
	CString strTmp;

	m_Configs.iRunCount ++;
	if(bPass)
		m_Configs.iSuccess ++;

	for(i = 0; i < ITEM_WVCNT; i ++ ) {
		piinfo = &m_Configs.iinfo[i];
		if (piinfo->bEnable && (mask & 1 << i)) { /* enable and need inc */
			if (1 == piinfo->iMode) { /* auto increase mode */
				if (piinfo->nRemainCount != 0&& piinfo->nRemainCount != -1) {
					piinfo->nRemainCount--;
				}
				wsprintf(szTmp, piinfo->strItemCurrent.c_str());
				TrimAndInc(szTmp, piinfo->strPrefix.c_str(), piinfo->strSuffix.c_str(), 1);
				piinfo->strItemCurrent = szTmp;
			} else if (2 == piinfo->iMode) {/* read from file */

			}
		}
	}
	return TRUE;
}

void Crk_provision_toolDlg::OnBnClickedButtonRead()
{
	m_criSection.Lock();
	if (m_Configs.bSingle) {
		if (m_bRunning) {
			m_bUserStop = TRUE;
			SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_STOPING_BUTTON")).c_str());
		} else {
			if (OnStartReadLocked(TRUE)) {
				SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
				m_bRunning = TRUE;
				OnOperateChg(ID_OP_RD,1);
			}
		}
	} else {
		if( m_bStarted ) {
			if (m_bRunning) { /* If running, just set user stop flag */
				m_bUserStop = TRUE;
				SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_STOPING_BUTTON")).c_str());
			} else {
				SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_READ_BUTTON")).c_str());
				OnOperateChg(ID_OP_RD,0);
				m_bStarted = FALSE;
			}
		} else {
			SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
			m_bStarted = TRUE;
			OnOperateChg(ID_OP_RD,1);
			if(OnStartReadLocked(FALSE)) {
				m_bRunning = TRUE;
			}
		}
	}
	m_criSection.Unlock();
}

void Crk_provision_toolDlg::OnClose()
{
	int i;

	if(m_bRunning) {
		MessageBox(_T("Running now,please wait!"), NULL, MB_OK|MB_ICONERROR);
		return;
	}
	if(m_bStarted) {
		MessageBox(_T("Please stop first!"), NULL, MB_OK|MB_ICONERROR);
		return;
	}
	for(i = 0; i < ITEM_WVCNT; i++) {
		FreeBuffer(&m_ItemInfo[i]);
	}
	m_ScanExitEvent.SetEvent();
	if (m_pScanThread) {
		MSG msg;
		DWORD dwRet;
		while (TRUE) {
			dwRet = MsgWaitForMultipleObjects(1, &m_ScanExitNotify.m_hObject, FALSE, 10000, QS_ALLINPUT);
			if(WAIT_OBJECT_0 == dwRet ) {
				break;
			} else if ((WAIT_OBJECT_0+1) == dwRet) {
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			} else if (WAIT_TIMEOUT == dwRet) {
				TerminateThread(m_pScanThread->m_hThread,0);
				break;
			}
		}/* end while */
		m_pScanThread = NULL;
	}
	RK_Uninitialize();
    if(pLog) {
        delete pLog;
        pLog = NULL;
    }
    if (m_hRedLedBitmap) {
		DeleteObject(m_hRedLedBitmap);
		m_hRedLedBitmap = NULL;
    }
    if (m_hGreenLedBitmap) {
		DeleteObject(m_hGreenLedBitmap);
		m_hGreenLedBitmap = NULL;
    }
	m_Configs.SaveToolSetting(std::wstring(_T("")));
	CDialog::OnClose();
}

void Crk_provision_toolDlg::OnMenuAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void Crk_provision_toolDlg::OnMenuSettings()
{
	// TODO: Add your command handler code here
	CDlgSettings SettingsDlg(m_Configs, m_LocalLan);
	//ConfigModeDlg.UpdateInterface();
	m_criSection.Lock();
	if (m_bStarted || m_bRunning) {
		AfxMessageBox(GetLocalString(_T("IDS_ERROR_SETTING_RUNNING")).c_str());
		m_criSection.Unlock();
		return;
	}
	m_criSection.Unlock();
	if (IDOK == SettingsDlg.DoModal()) {
		m_Configs.SaveToolSetting(std::wstring(TEXT("")));
		OnUpdateCtrl();
	}
	OnClearCtrl(TRUE);
}

VOID Crk_provision_toolDlg::WalkMenu(CMenu *pMenu,CString strMainKeyPart)
{
	CString strKey;
	int     id ;
	UINT    i;
	if(NULL == pMenu) return ;
	for (i=0;i < pMenu->GetMenuItemCount();i++) {
		strKey.Format(_T("%s_%d"),strMainKeyPart,i);
		id = pMenu->GetMenuItemID(i);
		if (0 == id) { 
			/*If nPos corresponds to a SEPARATOR menu item, the return value is 0. **/
		} else if (-1 == id) { 
			/*If the specified item is a pop-up menu (as opposed to an item within the pop-up menu), the return value is ¨C1 **/
			pMenu->ModifyMenu(i, MF_BYPOSITION, i, m_LocalLan.GetStr((LPCTSTR)strKey).c_str());
			WalkMenu(pMenu->GetSubMenu(i),strKey);
		} else {
			pMenu->ModifyMenu(id, MF_BYCOMMAND, id,m_LocalLan.GetStr((LPCTSTR)strKey).c_str());
		}
	}
}

BOOL Crk_provision_toolDlg::LoadConfig()
{
	CString strConfigPath;
	strConfigPath = m_strModulePath + _T("config.ini");
	bool bLoadConfig = m_Configs.LoadToolSetting(strConfigPath.GetString());

	if (!bLoadConfig) {
		return FALSE;
	}
	if(m_Configs.nCurLan >= 0 && m_Configs.nCurLan < m_Configs.lan_info_cnt) {
		strConfigPath = m_strModulePath + m_Configs.strLanPath.c_str() + 
						m_Configs.lan_info[m_Configs.nCurLan].LangFile.c_str();
		bLoadConfig = m_LocalLan.LoadToolSetting(strConfigPath.GetString());
		if (!bLoadConfig) {
			return FALSE;
		}
		m_LocalLan.TreeControls(m_hWnd, m_Configs.bDebug ? TRUE : FALSE, this->IDD, true);
		if(!m_Configs.bDebug) {
			WalkMenu(GetMenu(),TEXT("MENU"));
		}
	}
	/* m_LocalLan.TreeControls(m_hWnd,m_Configs.bDebug?TRUE:FALSE,this->IDD,true); */

	return TRUE;
}

void Crk_provision_toolDlg::OnBnClickedCheckSn()
{
	// TODO: Add your control notification handler code here
	OnBnClickedItem(ITEM_SN);
}

std::wstring Crk_provision_toolDlg::GetLocalString(std::wstring strKey)
{
	return m_LocalLan.GetLanStr(strKey);
}

void Crk_provision_toolDlg::OnBnClickedCheckWifi()
{
	// TODO: Add your control notification handler code here
	OnBnClickedItem(ITEM_WIFI);
}

void Crk_provision_toolDlg::OnBnClickedCheckLan()
{
	// TODO: Add your control notification handler code here
	OnBnClickedItem(ITEM_LAN);
}

void Crk_provision_toolDlg::OnBnClickedItem(int id)
{
	CEdit *pEditCtrl;

	if(id < 0 || id > ITEM_WVCNT)
		return;
	pEditCtrl = (CEdit *)GetDlgItem(editcontrolmap[id].id);
	if(BST_CHECKED == (((CButton *)GetDlgItem(checkcontrolmap[id].id))->GetCheck()&BST_CHECKED)) {
		pEditCtrl->EnableWindow(TRUE);
		/*if( 0 == m_Configs.iinfo[i].iMode) {
			pEditCtrl->SetReadOnly(0);
		} else {
			pEditCtrl->SetReadOnly(1);
		}*/
		m_Configs.iinfo[id].bEnable = TRUE;
	} else {
		pEditCtrl->EnableWindow(FALSE);
		m_Configs.iinfo[id].bEnable = FALSE;
	}

}
void Crk_provision_toolDlg::OnBnClickedCheckBt()
{
	// TODO: Add your control notification handler code here
	OnBnClickedItem(ITEM_BT);
}

void Crk_provision_toolDlg::OnBnClickedCheckImei()
{
	// TODO: Add your control notification handler code here
	OnBnClickedItem(ITEM_IMEI);
}

void Crk_provision_toolDlg::OnClearCtrl(BOOL bAll)
{
	CEdit *pEditCtrl;
	int i;

	if(bAll) {
		for(i = 0; i < ITEM_WVCNT; i ++) {
			pEditCtrl = (CEdit *)GetDlgItem(editcontrolmap[i].id);
			pEditCtrl->SetWindowText(_T(""));
		}
	} else {
		for(i = 0; i < ITEM_WVCNT; i ++) {
			if(m_Configs.iinfo[i].bEnable) {
				pEditCtrl = (CEdit *)GetDlgItem(editcontrolmap[i].id);
				pEditCtrl->SetWindowText(_T(""));
			}
		}
	}
	SetControlsFocus();
	return;
}

void Crk_provision_toolDlg::OnLockCtrl()
{
	int i;

	for(i = 0; i < ITEM_WVCNT; i ++ ) {
		((CButton *)GetDlgItem(checkcontrolmap[i].id))->EnableWindow(m_Configs.bLockConfig ? FALSE : TRUE);
	}

}
void Crk_provision_toolDlg::OnUpdateCtrl()
{
	int i = 0;
	CEdit *pEditCtrl;

	for(i = 0; i < ITEM_WVCNT; i ++) {
		if(m_Configs.iinfo[i].bEnable) {
			((CButton *)GetDlgItem(checkcontrolmap[i].id))->SetCheck(BST_CHECKED);
			pEditCtrl = (CEdit *)GetDlgItem(editcontrolmap[i].id);
			pEditCtrl->EnableWindow(TRUE);
			pEditCtrl->SetReadOnly(0 == m_Configs.iinfo[i].iMode ? FALSE : TRUE);
		} else {
			((CButton *)GetDlgItem(checkcontrolmap[i].id))->SetCheck(BST_UNCHECKED);
			pEditCtrl = (CEdit *)GetDlgItem(editcontrolmap[i].id);
			pEditCtrl->EnableWindow(FALSE);
			pEditCtrl->SetReadOnly(0 == m_Configs.iinfo[i].iMode ? FALSE : TRUE);
		}
	}
	((CButton *)GetDlgItem(IDC_CHECK_SINGLE))->SetCheck(m_Configs.bSingle ? BST_CHECKED : BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(m_Configs.bSetLoader ? BST_CHECKED : BST_UNCHECKED);
	((CEdit *)GetDlgItem(IDC_EDIT_LOADER))->SetWindowText(m_Configs.strLoaderPath.c_str());
	OnLockCtrl();

}
void Crk_provision_toolDlg::OnBnClickedCheckSingle()
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_SINGLE))->GetCheck()&BST_CHECKED)){
		m_Configs.bSingle = TRUE;
	} else {
		m_Configs.bSingle = FALSE;
	}
}

LRESULT Crk_provision_toolDlg::OnHandleUpdateMsg(WPARAM wParam,LPARAM lParam)
{
	switch(wParam) {
	case UPDATE_WINDOW:
		m_lblDevice.SetWindowText(m_strDevice);
		RedrawWindow();
		break;
	case DEVICE_CHG:
		if(lParam)
			OnDeviceArrival();
		break;
	case SHOW_RESULT:
		if ( lParam < ITEM_WVCNT && lParam >= 0){
			GetDlgItem(editcontrolmap[lParam].id)->SetWindowText(
				SetItemString(lParam, m_ItemInfo[lParam].pData,
				m_ItemInfo[lParam].size));
		}
		break;
	case UPDATE_TEST_EXIT:
		if (ID_OP_WR == lParam) {
			SetDlgItemText(IDC_BUTTON_WRITE, GetLocalString(_T("IDS_TEXT_WRITE_BUTTON")).c_str());
			OnOperateChg(ID_OP_WR,0);
		} else {
			SetDlgItemText(IDC_BUTTON_READ, GetLocalString(_T("IDS_TEXT_READ_BUTTON")).c_str());
			OnOperateChg(ID_OP_RD,0);
		}
		break;
	case UPDATE_LIST:
		if (lParam == LIST_EMPTY) {
			m_listInfo.ResetContent();
		} else {
			PSTRUCT_LIST_LINE pLine = (PSTRUCT_LIST_LINE)lParam;
			if (pLine->flag == LIST_INFO) {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Black,pLine->pszLineText); 
			} else if (pLine->flag == LIST_TIME) {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Purple,pLine->pszLineText);
			} else if (pLine->flag == LIST_WARN){ 
				m_listInfo.AddLine(CXListBox::Black,CXListBox::Yellow,pLine->pszLineText);
			} else {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Red,pLine->pszLineText);
			}
			//m_listInfo.SetCurSel(m_listInfo.GetCount()-1);
			delete pLine;
		}
		break;
	case UPDATE_PROMPT:
		if (lParam == PROMPT_PASS) {
			SetControlsFocus();
		}
		break;
	}
	return 0;
}

BOOL Crk_provision_toolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ) {
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN ) {
		if (IsLastInputControlId(m_CurCtrlID)) {
			m_criSection.Lock();
			if ((!m_Configs.bSingle) && m_bStarted ) { /* auto and started */
				if (ID_OP_RD == m_CurOp) {
					if(OnStartReadLocked(FALSE)) {
						m_bRunning = TRUE;
					}
				} else if (ID_OP_WR == m_CurOp){
					if(OnStartWriteLocked(FALSE)) {
						m_bRunning = TRUE;
					}
				}
			}
			m_criSection.Unlock();
			return TRUE;
		}
		SetControlsFocus(FALSE);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);

}

BOOL Crk_provision_toolDlg::IsLastInputControlId(int id)
{
	int init_id;
	INT last_id = -2;

	for(init_id = 0; init_id < ITEM_WVCNT; init_id++ ) {
		if( 0 == m_Configs.iinfo[init_id].iMode && m_Configs.iinfo[init_id].bEnable) {
			last_id = init_id;
		}
	}
	if(id == last_id)
		return TRUE;
	return FALSE;
}

int Crk_provision_toolDlg::ControlsIdToItemId(int ctrl_id)
{
	for(int i = 0; i < ITEM_WVCNT; i ++) {
		if(ctrl_id == editcontrolmap[i].id)
			return i;
	}
	return -1;

}

/* bFirst: just focus the next id
 *
 */
VOID Crk_provision_toolDlg::SetControlsFocus(BOOL bFirst)
{
	int id;

	if(bFirst) {
		id = -1;
	} else {
		id = IsInputControlId(m_CurCtrlID) ? m_CurCtrlID : - 1;
	}
	id = NextFocusControlId(id, TRUE);
	if (id >= 0 && id < ITEM_WVCNT) {
		GetDlgItem(editcontrolmap[id].id)->SetFocus();
	}
}

BOOL Crk_provision_toolDlg::IsInputControlId(INT Id)
{

	if(Id < ITEM_WVCNT && Id >= 0 ) {
		return TRUE;
	}
	return FALSE;
}

INT Crk_provision_toolDlg::NextFocusControlId(INT startId, BOOL loop)
{
	INT id = -1;
	int init_id;
	item_info *piinfo;

	if(startId < 0 || startId >= ITEM_WVCNT)
		startId =  - 1;
	for(init_id = startId + 1; init_id < ITEM_WVCNT; init_id++ ) {
		piinfo = &m_Configs.iinfo[init_id];
		if( 0 == piinfo->iMode && piinfo->bEnable) {
			id = init_id;
			break;
		}
	}
	if(id == -1 && loop) {
		return NextFocusControlId(id, FALSE);
	}
	return id;
}
void Crk_provision_toolDlg::OnEnSetfocusEditSn()
{
	// TODO: Add your control notification handler code here
	m_CurCtrlID = ITEM_SN;
}

void Crk_provision_toolDlg::OnEnSetfocusEditWifi()
{
	// TODO: Add your control notification handler code here
	m_CurCtrlID = ITEM_WIFI;
}

void Crk_provision_toolDlg::OnEnSetfocusEditBt()
{
	// TODO: Add your control notification handler code here
	m_CurCtrlID = ITEM_BT;
}

void Crk_provision_toolDlg::OnEnSetfocusEditImei()
{
	// TODO: Add your control notification handler code here
	m_CurCtrlID = ITEM_IMEI;
}

void Crk_provision_toolDlg::OnEnSetfocusEditLan()
{
	// TODO: Add your control notification handler code here
	m_CurCtrlID = ITEM_LAN;
}

int Crk_provision_toolDlg::ReadItem(int id, BYTE *buf, int len)
{
	BOOL bRet;
	USHORT nBufferSize;
	int id_in_device = index_id_map[id].id;

	nBufferSize = len;
	bRet = RK_ReadProvisioningData(id_in_device, buf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_ReadProvisioningData(%d) failed.", id_in_device));
		return -1;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "ReadItem(%d), len=%d", id_in_device, nBufferSize));
	return nBufferSize;
}

int Crk_provision_toolDlg::WriteItem(int id, BYTE *buf, int len)
{
	BOOL bRet;
	PBYTE pReadBuf = NULL;
	USHORT nBufferSize = len;
	int id_in_device = index_id_map[id].id;

	if( !buf || (0 >= len)) {
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteItem(%d) buf=0x%08x, len=%d", id_in_device, buf, len));
		return FALSE;
	}
	bRet = RK_WriteProvisioningData(id_in_device, buf,len);		
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_WriteProvisioningData(%d) failed.", id_in_device));
		return FALSE;
	}
	pReadBuf = new BYTE[nBufferSize];
	if(!pReadBuf) {
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteItem(%d) alloc read buffer failed.", id_in_device));
		return FALSE;
	}
	bRet = RK_ReadProvisioningData(id_in_device, pReadBuf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_ReadProvisioningData(%d) failed.", id_in_device));
		return FALSE;
	}
	if (memcmp(pReadBuf, buf, nBufferSize) != 0) {
		if(pReadBuf)
			delete[] pReadBuf;
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteItem(%d) compare failed.", id_in_device));
		return FALSE;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "WriteItem(%d), len=%d", id_in_device, nBufferSize));
	if(pReadBuf)
		delete[] pReadBuf;
	return TRUE;
}

int Crk_provision_toolDlg::ReadVendorRpmbItem(BYTE dest, int id, BYTE *buf, int len)
{
	BOOL bRet;
	USHORT nBufferSize;
	int id_in_device = index_id_map[id].id;

	nBufferSize = len;
	bRet = RK_ReadVendorRpmbData(id_in_device, dest, buf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_ReadVendorRpmbData(%d) failed.", id_in_device));
		return -1;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "ReadVendorRpmbItem(%d), len=%d", id_in_device, nBufferSize));
	return nBufferSize;
}

int Crk_provision_toolDlg::WriteVendorRpmbItem(BYTE dest, int id, BYTE *buf, int len)
{
	BOOL bRet;
	PBYTE pReadBuf = NULL;
	USHORT nBufferSize = len;
	int id_in_device = index_id_map[id].id;

	if( !buf || (0 >= len)) {
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteVendorRpmbItem(%d) buf=0x%08x, len=%d", id_in_device, buf, len));
		return FALSE;
	}	
	bRet = RK_WriteVendorRpmbData(id_in_device, dest, buf,len);		
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_WriteVendorRpmbData(%d) failed.", id_in_device));
		return FALSE;
	}
	pReadBuf = new BYTE[nBufferSize];
	if(!pReadBuf) {
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteVendorRpmbItem(%d) alloc read buffer failed.", id_in_device));
		return FALSE;
	}
	bRet = RK_ReadVendorRpmbData(id_in_device, dest, pReadBuf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "RK_ReadVendorRpmbData(%d) failed.", id_in_device));
		return FALSE;
	}

	if (memcmp(pReadBuf, buf, nBufferSize) != 0) {
		if(pReadBuf)
			delete[] pReadBuf;
		LDEGMSG((CLogger::DEBUG_ERROR, "WriteVendorRpmbItem(%d) compare failed.", id_in_device));
		return FALSE;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "WriteVendorRpmbItem(%d), len=%d", id_in_device, nBufferSize));
	if(pReadBuf)
		delete[] pReadBuf;
	return TRUE;
}

int Crk_provision_toolDlg::CompatRead(int id, PBYTE pBuf, INT& nLen, DWORD dwLayer)
{
	if(1 == id) {
		return RK_ReadSN(pBuf, nLen, dwLayer);
	} else if (2 == id) {
		return RK_ReadWifi(pBuf, nLen, dwLayer);
	} else if (3 == id) {
		return RK_ReadMAC(pBuf, nLen, dwLayer);
	} else if (4 == id) {
		return RK_ReadBT(pBuf, nLen, dwLayer);
	} else if (5 == id) {
		return RK_ReadIMEI(pBuf, nLen, dwLayer);
	}
	return -1;
}

int Crk_provision_toolDlg::CompatWrite(int id, PBYTE pBuf, INT& nLen, DWORD dwLayer)
{
	if(1 == id) {
		return RK_WriteSN(pBuf, nLen, dwLayer);
	} else if (2 == id) {
		return RK_WriteWifi(pBuf, nLen, dwLayer);
	} else if (3 == id) {
		return RK_WriteMAC(pBuf, nLen, dwLayer);
	} else if (4 == id) {
		return RK_WriteBT(pBuf, nLen, dwLayer);
	} else if (5 == id) {
		return RK_WriteIMEI(pBuf, nLen, dwLayer);
	}
	return -1;

}

int Crk_provision_toolDlg::CompatReadItem(int id, BYTE *buf, int len)
{
	BOOL bRet;
	INT nBufferSize = len;
	int id_in_device = index_id_map[id].id;

	bRet = CompatRead(id_in_device, buf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "CompatRead(%d) failed.", id_in_device));
		return -1;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "CompatReadItem(%d), len=%d", id_in_device, nBufferSize));
	return nBufferSize;
}

int Crk_provision_toolDlg::CompatWriteItem(int id, BYTE *buf, int len)
{
	BOOL bRet;
	PBYTE pReadBuf = NULL;
	INT nBufferSize = len;
	int id_in_device = index_id_map[id].id;

	if( !buf || (0 >= len)) {
		LDEGMSG((CLogger::DEBUG_ERROR, "CompatWriteItem(%d) buf=0x%08x, len=%d", id_in_device, buf, len));
		return FALSE;
	}
	bRet = CompatWrite(id_in_device, buf, len);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR, "CompatWrite(%d) failed.", id_in_device));
		return FALSE;
	}
	pReadBuf = new BYTE[nBufferSize];
	if(!pReadBuf) {
		LDEGMSG((CLogger::DEBUG_ERROR, "CompatWriteItem(%d) alloc read buffer failed.", id_in_device));
		return FALSE;
	}
	bRet = CompatRead(id_in_device, pReadBuf, nBufferSize);
	if (!bRet) {
		LDEGMSG((CLogger::DEBUG_ERROR,"CompatRead(%d) failed.", id_in_device));
		return FALSE;
	}
	if (memcmp(pReadBuf, buf, nBufferSize) != 0) {
		if(pReadBuf)
			delete[] pReadBuf;
		LDEGMSG((CLogger::DEBUG_ERROR,"CompatWriteItem(%d) compare failed.", id_in_device));
		return FALSE;
	}
	LDEGMSG((CLogger::DEBUG_INFO, "CompatWriteItem(%d), len=%d", id_in_device, nBufferSize));
	if(pReadBuf)
		delete[] pReadBuf;
	return TRUE;
}

int Crk_provision_toolDlg::DebugReadItem(int id, BYTE *buf, int len)
{
	BYTE *resp_sn = (BYTE *)"rockchip201809022311";
	BYTE resp_mac[6] = {0x0A, 0x0B, 0x11, 0x22, 0x33, 0x44};
	BYTE *resp_imei = (BYTE *)"868938037160878";
	BYTE *resp;
	USHORT nBufferSize;
	int id_in_device = index_id_map[id].id;

	LDEGMSGW((CLogger::DEBUG_INFO,_T("DebugReadItem %s:%d"), index_id_map[id].name,
			 id_in_device));
	if(_tcsstr(index_id_map[id].name, _T("MAC"))) { /* mac */
		resp = resp_mac;
		nBufferSize = 6;
	} else if (_tcsstr(index_id_map[id].name, _T("IMEI"))){
		resp = resp_imei;
		nBufferSize = strlen((char *)resp);
	} else if (_tcsstr(index_id_map[id].name, _T("SN"))){ /* string convert */
		resp = resp_sn;
		nBufferSize = strlen((char *)resp);
	}
	if(nBufferSize > len)
		nBufferSize = len;
	memcpy(buf, resp, nBufferSize);
	Sleep(100*2);
	return nBufferSize;
}

int Crk_provision_toolDlg::DebugWriteItem(int id, BYTE *buf, int len)
{
	TCHAR *pStr;
	USHORT usStrLen;
	int i;
	int id_in_device = index_id_map[id].id;

	LDEGMSGW((CLogger::DEBUG_INFO, _T("DebugWriteItem %s:%d"), index_id_map[id].name,
			 id_in_device));
	if(_tcsstr(index_id_map[id].name, _T("MAC"))) { /* mac */
		usStrLen = len*2 + 1;
		pStr = new TCHAR[usStrLen];
		for(i = 0; i < len; i ++ ) {
			pStr[2*i] = byte_to_tchar((buf[i]>>4)&0xf);
			pStr[2*i + 1] = byte_to_tchar((buf[i])&0xf);
		}
		pStr[2*i] = 0;
	} else if (_tcsstr(index_id_map[id].name, _T("IMEI"))) { /* string convert */
		usStrLen = CharToWChar(0, NULL, (const CHAR *)buf);
		pStr = new TCHAR[usStrLen];
		CharToWChar(pStr, usStrLen, (const CHAR *)buf);
	} else if (_tcsstr(index_id_map[id].name, _T("SN"))) {
		usStrLen = CharToWChar(0, NULL, (const CHAR *)buf);
		pStr = new TCHAR[usStrLen];
		CharToWChar(pStr, usStrLen, (const CHAR *)buf);
	}
	if(pStr) {
		LDEGMSGW((CLogger::DEBUG_INFO,_T("DebugWriteItem %s:%s"),
				 index_id_map[id].name, pStr));
		delete[] pStr;
	}
	Sleep(100*2);
	return TRUE;
}


void Crk_provision_toolDlg::OnBnClickedCheckLoader()
{
	// TODO: Add your control notification handler code here
	CString strLoaderFileName;
    TCHAR szFilters[] =  _T("binary\0*.bin\0");
    CFileDialog OpenDlg(TRUE);

	if(BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_LOADER))->GetCheck()&BST_CHECKED)) {
		((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(FALSE);
		/*GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(_T(""));*/
		m_Configs.bSetLoader = FALSE;
	} else {
		OpenDlg.m_ofn.lpstrTitle = _T("Open File");
		OpenDlg.m_ofn.lpstrFilter = szFilters;
		if (IDOK == OpenDlg.DoModal()) {
			strLoaderFileName = OpenDlg.GetPathName();
			GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(strLoaderFileName);
			((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(TRUE);
			m_Configs.bSetLoader = TRUE;
			m_Configs.strLoaderPath = strLoaderFileName.GetString();
		} else {
			/* ((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(FALSE); */
			/* GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(_T("")); */
			/* m_Configs.bSetLoader = FALSE; */
		}
	}
}

void Crk_provision_toolDlg::OnDeviceArrival()
{
	m_criSection.Lock();
	if(!m_bRunning) {
		OnClearCtrl(FALSE);
		if ((!m_Configs.bSingle) && m_bStarted ) { /* auto and started */
			if (ID_OP_RD == m_CurOp) {
				if(OnStartReadLocked(FALSE)) {
					m_bRunning = TRUE;
				}
			} else if (ID_OP_WR == m_CurOp){
				if(OnStartWriteLocked(FALSE)) {
					m_bRunning = TRUE;
				}
			}
		}
	} else {
		
	}
	m_criSection.Unlock();

}

void Crk_provision_toolDlg::OnDeviceRemoval()
{


}

BOOL Crk_provision_toolDlg::AdbToRockusb()
{
	CSpawn ShellSpawn;
	wchar_t strCmd[256];

	swprintf(strCmd, nof(strCmd), TEXT("adb.exe reboot loader"));
	if (ShellSpawn.Exe(strCmd, 50000, true)) {
	} else {
		return FALSE;
	}
	return TRUE;
}


void Crk_provision_toolDlg::OnBnClickedButtonTest()
{
#define DEBUG_CASE 1
#if 1 == DEBUG_CASE
	// TODO: Add your control notification handler code here
	TCHAR szTemp[128];
	std::wstring str = _T("LANSHH1100223344SHUNHUA");
	std::wstring prestr = _T("LANSHH");
	std::wstring sufstr = _T("SHUNHUA");
	std::wstring r;
	BOOL bR;

	r = MID(str, prestr, sufstr);
	wsprintf(szTemp, _T("%s"), str.c_str());
	bR = TrimAndInc(szTemp, prestr.c_str(), sufstr.c_str(), 1);
#elif 2 == DEBUG_CASE

	unsigned int mac[6];
	int i = sscanf("0a0b11223344", "%2x%2x%2x%2x%2x%2x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	int s = i;

#elif 3 == DEBUG_CASE
	CString strSerialno;
	GetAdbSerialno(m_strLinkName, strSerialno);
	AfxMessageBox(strSerialno);
#elif 4 == DEBUG_CASE
		TCHAR szMsg[256];
		TCHAR *pStr = _T("123456789");
		int len = 18;
		char cMsg[256];
		int clen = 256;

		int iLen = CWCharToChar(0, NULL, pStr, len);
		wsprintf(szMsg, _T("%d"), iLen);
		AfxMessageBox(szMsg);
		iLen = CWCharToChar(cMsg, clen, pStr, len);
		wsprintf(szMsg, _T("%d"), iLen);
		AfxMessageBox(szMsg);
#endif


}


void Crk_provision_toolDlg::OnBnClickedButtonBrowLoader()
{
	// TODO: Add your control notification handler code here
	CString strLoaderFileName;
    TCHAR szFilters[] =  _T("binary\0*.bin\0");
    CFileDialog OpenDlg(TRUE);

	OpenDlg.m_ofn.lpstrTitle = _T("Open File");
	OpenDlg.m_ofn.lpstrFilter = szFilters;
	if (IDOK == OpenDlg.DoModal()) {
		strLoaderFileName = OpenDlg.GetPathName();
		GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(strLoaderFileName);
		((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(TRUE);
		m_Configs.bSetLoader = TRUE;
		m_Configs.strLoaderPath = strLoaderFileName.GetString();
	}
}

void Crk_provision_toolDlg::OnMenuLog()
{
	// TODO: Add your command handler code here
	ShellExecute(NULL, _T("open"), m_szLogPath, NULL, NULL, SW_SHOWNORMAL);
}

void Crk_provision_toolDlg::OnBnClickedButtonReboot()
{
	// TODO: Add your control notification handler code here
	RK_ResetRockusb();
}
