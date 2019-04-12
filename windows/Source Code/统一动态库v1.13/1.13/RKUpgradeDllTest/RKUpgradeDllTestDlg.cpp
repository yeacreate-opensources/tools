// RKUpgradeDllTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RKUpgradeDllTest.h"
#include "RKUpgradeDllTestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
UINT ThreadRun(LPVOID lpParam)
{
	BOOL bRet;
	CRKUpgradeDllTestDlg *pDlg = (CRKUpgradeDllTestDlg *)lpParam;
	bRet = pDlg->RunProc();
	if (bRet)
	{
		MessageBox(pDlg->m_hWnd,_T("Upgrade ok."),NULL,MB_OK);
	}
	else
		MessageBox(pDlg->m_hWnd,_T("Upgrade failed!"),NULL,MB_OK|MB_ICONERROR);
	return 0;
}
UINT ThreadWriteMac(LPVOID lpParam)
{
	BOOL bRet;
	CRKUpgradeDllTestDlg *pDlg = (CRKUpgradeDllTestDlg *)lpParam;
	bRet = pDlg->WriteMacProc();
	if (bRet)
	{
		MessageBox(pDlg->m_hWnd,_T("WriteMac ok."),NULL,MB_OK);
	}
	else
		MessageBox(pDlg->m_hWnd,_T("WriteMac failed!"),NULL,MB_OK|MB_ICONERROR);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRKUpgradeDllTestDlg dialog

CRKUpgradeDllTestDlg::CRKUpgradeDllTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRKUpgradeDllTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRKUpgradeDllTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CRKUpgradeDllTestDlg::~CRKUpgradeDllTestDlg()
{

}
void CRKUpgradeDllTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRKUpgradeDllTestDlg)
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_infoListBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRKUpgradeDllTestDlg, CDialog)
	//{{AFX_MSG_MAP(CRKUpgradeDllTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FW, &CRKUpgradeDllTestDlg::OnBnClickedButtonOpenFw)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_LOADER, &CRKUpgradeDllTestDlg::OnBnClickedButtonOpenLoader)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE, &CRKUpgradeDllTestDlg::OnBnClickedButtonUpgrade)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_MAC, &CRKUpgradeDllTestDlg::OnBnClickedButtonWriteMac)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRKUpgradeDllTestDlg message handlers

BOOL CRKUpgradeDllTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_bRun = FALSE;
	GetModuleFileName(NULL, m_modulePath.GetBuffer(MAX_PATH), MAX_PATH);
	m_modulePath.ReleaseBuffer();
	m_modulePath = m_modulePath.Left(m_modulePath.ReverseFind(_T('\\'))+1);
	m_LogPath = m_modulePath + _T("Log\\");

	if (!PathFileExists(m_LogPath))
	{
		CreateDirectory(m_LogPath,NULL);
	}
	BOOL bRet;
	m_dllObject = new CUpgradeLibrary(m_modulePath+_T("RKUpgrade.dll"),bRet);
	if (!bRet)
	{
		if (m_dllObject)
		{
			delete m_dllObject;
			m_dllObject = NULL;
		}
		MessageBox(_T("Loading rkupgrade.dll failed!"),NULL,MB_OK|MB_ICONERROR);
	}
	else
	{
		INIT_DEV_INFO dev_info;
		INIT_LOG_INFO log_info;
		INIT_CALLBACK_INFO callback_info;
		memset(&dev_info,0,sizeof(INIT_DEV_INFO));
		memset(&callback_info,0,sizeof(INIT_CALLBACK_INFO));
		dev_info.uiRockMscTimeout = dev_info.uiRockusbTimeout = 30;//30s
		/*custom device need set vid and pid
		dev_info.usRockUvcVid = ;
		dev_info.usRockUvcPid = ;
		*/
		log_info.bLogEnable = TRUE;
		log_info.lpszLogPathName = (LPTSTR)(LPCTSTR)m_LogPath;
		m_dllObject->m_bInitOK = m_dllObject->initialize(dev_info,log_info,callback_info);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRKUpgradeDllTestDlg::OnPaint() 
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
HCURSOR CRKUpgradeDllTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRKUpgradeDllTestDlg::OnBnClickedButtonOpenFw()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg( TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("Firmware(*.img)|*.img|All Files(*.*)|*.*||"));
	// Initializes m_ofn structure 
	fileDlg.m_ofn.lpstrTitle = _T("Open firmware");

	if ( fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_FW,fileDlg.GetPathName());
	}
}

void CRKUpgradeDllTestDlg::OnBnClickedButtonOpenLoader()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg( TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("Loader(*.bin)|*.bin|All Files(*.*)|*.*||"));
	// Initializes m_ofn structure 
	fileDlg.m_ofn.lpstrTitle = _T("Open loader");

	if ( fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LOADER,fileDlg.GetPathName());
	}
}

void CRKUpgradeDllTestDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bRun)
	{
		return;
	}
	if (m_dllObject)
	{
		if (m_dllObject->m_bInitOK)
		{
			m_dllObject->deinitialize();
		}
		delete m_dllObject;
	}
	CDialog::OnClose();
}

void CRKUpgradeDllTestDlg::OnBnClickedButtonUpgrade()
{
	// TODO: Add your control notification handler code here

	GetDlgItemText(IDC_EDIT_FW,m_strFw);
	GetDlgItemText(IDC_EDIT_LOADER,m_strLoader);
	if (m_strFw.IsEmpty())
	{
		MessageBox(_T("Firmware is empty!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	if (m_strLoader.IsEmpty())
	{
		MessageBox(_T("Loader is empty!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	if (!m_dllObject)
	{
		MessageBox(_T("RKUpgrade did not be loaded!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	if (!m_dllObject->m_bInitOK)
	{
		MessageBox(_T("Loader did not initialize!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	m_infoListBox.ResetContent();
	m_bRun = TRUE;
	DisableCtrl();
	AfxBeginThread(ThreadRun,(LPVOID)this);
	
}
VOID CRKUpgradeDllTestDlg::EnableCtrl()
{
	GetDlgItem(IDC_BUTTON_UPGRADE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OPEN_FW)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OPEN_LOADER)->EnableWindow(TRUE);
}
VOID CRKUpgradeDllTestDlg::DisableCtrl()
{
	GetDlgItem(IDC_BUTTON_UPGRADE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN_FW)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN_LOADER)->EnableWindow(FALSE);
}
BOOL CRKUpgradeDllTestDlg::RunProc()
{
	BOOL bSuccess=FALSE;
	//1.scan device
	int nDev,i,ret;
	PSTRUCT_DEVICE_DESC pDevs=NULL;
	nDev = m_dllObject->scan_device(&pDevs);
	if (nDev<=0)
	{
		m_infoListBox.AddString(_T("No found device!"));
		goto Exit_Run;
	}
	for (i=0;i<nDev;i++)
	{
		if (pDevs[i].emUsbType==RKUSB_MASKROM)
		{
			break;
		}
		/*
		if (pDevs[i].emUsbType==RKUSB_UVC)
		{
		  switch uvc into maskrom
		}
		*/
	}
	if (i>=nDev)
	{
		m_infoListBox.AddString(_T("No found device to upgrade!"));
		goto Exit_Run;
	}
	//2.download boot
	m_infoListBox.AddString(_T("Download Boot..."));
	ret = m_dllObject->download_boot(pDevs[i].szLinkName,(LPTSTR)(LPCTSTR)m_strLoader);
	if (ret!=0)
	{
		m_infoListBox.AddString(_T("Download Boot failed!"));
		goto Exit_Run;
	}
	m_infoListBox.AddString(_T("Download Boot ok"));
	//3.wait for device reconnect
	Sleep(1000);//1s
	//4.write firmware to flash
	if (!DownloadImage(pDevs[i].szLinkName,(LPTSTR)(LPCTSTR)m_strFw))
	{
		m_infoListBox.AddString(_T("Download firmware failed!"));
		goto Exit_Run;
	}
	bSuccess = TRUE;
Exit_Run:
	m_bRun = FALSE;
	EnableCtrl();
	return bSuccess;
}
BOOL CRKUpgradeDllTestDlg::WriteMacProc()
{
	BOOL bSuccess=FALSE;
	//1.scan device
	int nDev,i,macLen=6;
	BYTE mac[] = {0x11,0x22,0x33,0x44,0x55,0x66};
	PSTRUCT_DEVICE_DESC pDevs=NULL;
	nDev = m_dllObject->scan_device(&pDevs);
	if (nDev<=0)
	{
		m_infoListBox.AddString(_T("No found device!"));
		goto Exit_WriteMac;
	}
	for (i=0;i<nDev;i++)
	{
		if (pDevs[i].emUsbType==RKUSB_LOADER)
		{
			break;
		}
		/*
		if (pDevs[i].emUsbType==RKUSB_UVC)
		{
		  switch uvc into maskrom
		}
		*/
	}
	if (i>=nDev)
	{
		m_infoListBox.AddString(_T("No found device to WriteMac!"));
		goto Exit_WriteMac;
	}
	//2.download boot
	m_infoListBox.AddString(_T("Writing Mac..."));
	bSuccess = m_dllObject->write_mac(mac,macLen,0);
	if (!bSuccess)
	{
		m_infoListBox.AddString(_T("WriteMac failed!"));
		goto Exit_WriteMac;
	}
	m_infoListBox.AddString(_T("WriteMac ok"));
	
Exit_WriteMac:
	m_bRun = FALSE;
	EnableCtrl();
	return bSuccess;
}
#define SECTOR_SIZE 512
#define MAX_READWRITE_SECTORS 32
BOOL CRKUpgradeDllTestDlg::DownloadImage(TCHAR *szDevPath,TCHAR *szImage,unsigned int uiOffset)
{
	int iRet;
	BOOL bSuccess=FALSE,bNewLine=TRUE,bRet;
	CFile file;
	DWORD dwMaxReadWriteBytes ;
	PBYTE pBuf=NULL ;
	PBYTE pRead=NULL ;
	UINT  uiBegin,uiRead,uiTransferSec;
	ULONGLONG dwTotalWritten,dwTotalRead;
	DWORD dwTransferBytes,dwProgress,dwLastProgress;
	ULONGLONG dwFWSize;
	CString strFile,strPromptText,strFormatText;

	file.m_hFile = INVALID_HANDLE_VALUE;

	dwMaxReadWriteBytes = MAX_READWRITE_SECTORS * SECTOR_SIZE;
	pBuf = new BYTE[dwMaxReadWriteBytes];
	pRead = new BYTE[dwMaxReadWriteBytes];

	bRet = file.Open(szImage,CFile::typeBinary|CFile::modeRead);
	if( !bRet )
	{
		goto Exit_DownloadImage;
	}
	dwFWSize = file.GetLength();
	file.SeekToBegin();
	dwTotalWritten = 0;
	dwLastProgress = dwProgress = 0;
	uiBegin = uiOffset;
	strFormatText = _T("write firmware...%d%%");
	while(dwTotalWritten<dwFWSize) 
	{
		memset(pBuf, 0, dwMaxReadWriteBytes);
		if ((dwFWSize-dwTotalWritten)>=dwMaxReadWriteBytes)
		{
			dwTransferBytes = dwMaxReadWriteBytes;
			uiTransferSec = MAX_READWRITE_SECTORS;
		}
		else
		{
			dwTransferBytes = dwFWSize - dwTotalWritten;
			uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
		}
		uiRead = file.Read(pBuf,dwTransferBytes);
		if (uiRead!=dwTransferBytes)
		{
			goto Exit_DownloadImage;
		}
		iRet = m_dllObject->write_lba(szDevPath,uiBegin,uiTransferSec,pBuf);
		if( 0 == iRet )
		{
			dwTotalWritten += dwTransferBytes;
			uiBegin += uiTransferSec;
			dwProgress = (dwTotalWritten/1024)*100/(dwFWSize/1024);

			if (dwProgress!=dwLastProgress)
			{
				dwLastProgress = dwProgress;
				strPromptText.Format(strFormatText,dwProgress);
				if (bNewLine)
				{
					m_infoListBox.AddString(strPromptText);
					bNewLine = FALSE;
				}
				else
				{
					m_infoListBox.DeleteString(m_infoListBox.GetCount()-1);
					m_infoListBox.AddString(strPromptText);
				}
			}
		}
		else
		{
			goto Exit_DownloadImage;
		}
	}
	bNewLine=TRUE; 
	dwLastProgress = dwProgress = 0;
	uiBegin = uiOffset+0x40;
	strFormatText = _T("check firmware...%d%%");
	//skip 0x40 protection sectors 
	file.Seek(0x40*SECTOR_SIZE,CFile::begin);
	dwTotalRead = 0x40*SECTOR_SIZE;

	while(dwTotalRead<dwFWSize) 
	{
		memset(pBuf, 0, dwMaxReadWriteBytes);
		memset(pRead, 0, dwMaxReadWriteBytes);
		if ((dwFWSize-dwTotalRead)>=dwMaxReadWriteBytes)
		{
			dwTransferBytes = dwMaxReadWriteBytes;
			uiTransferSec = MAX_READWRITE_SECTORS;
		}
		else
		{
			dwTransferBytes = dwFWSize - dwTotalRead;
			uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
		}
		uiRead = file.Read(pBuf,dwTransferBytes);
		if (uiRead!=dwTransferBytes)
		{
			goto Exit_DownloadImage;
		}
		iRet = m_dllObject->read_lba(szDevPath,uiBegin,uiTransferSec,pRead);
		if( 0 == iRet )
		{
			dwTotalRead += dwTransferBytes;
			uiBegin += uiTransferSec;
		}
		else
		{
			goto Exit_DownloadImage;
		}

		if( 0!=memcmp(pBuf, pRead, dwTransferBytes) )
		{
			goto Exit_DownloadImage;
		}

		dwProgress = (dwTotalRead/1024)*100/(dwFWSize/1024);
	
		if (dwProgress!=dwLastProgress)
		{
			dwLastProgress = dwProgress;
			strPromptText.Format(strFormatText,dwProgress);
			if (bNewLine)
			{
				m_infoListBox.AddString(strPromptText);
				bNewLine = FALSE;
			}
			else
			{
				m_infoListBox.DeleteString(m_infoListBox.GetCount()-1);
				m_infoListBox.AddString(strPromptText);
			}
		}
	}

	bSuccess = TRUE;

Exit_DownloadImage:

	if (file.m_hFile!=INVALID_HANDLE_VALUE)
		file.Close();
	if (pBuf)
		delete []pBuf;
	if (pRead)
		delete []pRead;

	return bSuccess;
}
void CRKUpgradeDllTestDlg::OnBnClickedButtonWriteMac()
{
	// TODO: Add your control notification handler code here
	if (!m_dllObject)
	{
		MessageBox(_T("RKUpgrade did not be loaded!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	if (!m_dllObject->m_bInitOK)
	{
		MessageBox(_T("Loader did not initialize!"),NULL,MB_OK|MB_ICONERROR);
		return;
	}
	m_infoListBox.ResetContent();
	m_bRun = TRUE;
	DisableCtrl();
	AfxBeginThread(ThreadWriteMac,(LPVOID)this);
}
