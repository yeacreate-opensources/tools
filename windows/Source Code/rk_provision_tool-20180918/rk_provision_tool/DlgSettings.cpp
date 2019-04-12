// DlgSettings.cpp : implementation file
//

#include "stdafx.h"
#include "rk_provision_tool.h"
#include "DlgSettings.h"


// CDlgSettings dialog

IMPLEMENT_DYNAMIC(CDlgSettings, CDialog)

CDlgSettings::CDlgSettings(CIniSettingBase &Config, CIniLocalLan &LocalLang,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSettings::IDD, pParent),m_Configs(Config),m_LocalLan(LocalLang),
	  m_DlgItemCmn(Config, LocalLang)
{

}

CDlgSettings::~CDlgSettings()
{
}

void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ITEM, m_TabItem);
	DDX_Control(pDX, IDC_CHECK_FORCE_MASKROM, m_btnForceMaskrom);
	DDX_Control(pDX, IDC_EDIT_MASKROM, m_CeditLoader);
}


BEGIN_MESSAGE_MAP(CDlgSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSettings::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_ITEM, &CDlgSettings::OnTcnSelchangingTabItem)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ITEM, &CDlgSettings::OnTcnSelchangeTabItem)
	ON_BN_CLICKED(IDC_CHECK_FORCE_MASKROM, &CDlgSettings::OnBnClickedCheckForceMaskrom)
	ON_BN_CLICKED(IDC_CHECK_COMPAT, &CDlgSettings::OnBnClickedCheckCompat)
	ON_BN_CLICKED(IDC_CHECK_REBOOT, &CDlgSettings::OnBnClickedCheckReboot)
	ON_BN_CLICKED(IDCANCEL, &CDlgSettings::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_LOCK, &CDlgSettings::OnBnClickedCheckLock)
END_MESSAGE_MAP()


// CDlgSettings message handlers
BOOL CDlgSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rc;
	m_TabItem.InsertItem(0,_T("SN"));
	m_TabItem.InsertItem(1,_T("WIFI MAC"));
	m_TabItem.InsertItem(2,_T("LAN MAC"));
	m_TabItem.InsertItem(3,_T("BT MAC"));
	m_TabItem.InsertItem(4,_T("IMEI"));

	m_LocalLan.TreeControls(m_hWnd, m_Configs.bDebug ? TRUE : FALSE, this->IDD, false);
	//m_DlgItemCmn.SetConfigs();
	m_DlgItemCmn.Create(IDD_ITEM_CMN, &m_TabItem);
	m_TabItem.GetClientRect(&rc);
	m_DlgItemCmn.MoveWindow(0, 20,rc.right - rc.left ,rc.bottom - rc.top - 20, TRUE);
	m_DlgItemCmn.ShowWindow(SW_SHOW);
	OnUpdateCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	int index = m_TabItem.GetCurSel();

	if(m_DlgItemCmn.SendMessage(WM_ITEM_PRESWITCH, 1, index)) {
		if(m_DlgItemCmn.SendMessage(WM_SETTING_OK, 0, 0)){
			OnSaveCtrl();
			OnOK();
		}
	}
}

void CDlgSettings::OnUpdateCtrl()
{
	((CButton *)GetDlgItem(IDC_CHECK_FORCE_MASKROM))->SetCheck(m_Configs.bForceMskrom ? TRUE :FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_COMPAT))->SetCheck(m_Configs.bCompatRw ? TRUE :FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_REBOOT))->SetCheck(m_Configs.bRebootDevice ? TRUE :FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_RPMB))->SetCheck(m_Configs.bRPMB ? TRUE :FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_LOCK))->SetCheck(m_Configs.bLockConfig ? TRUE :FALSE);
	m_CeditLoader.SetWindowText(m_Configs.strLoaderPath.c_str());

}

void CDlgSettings::OnSaveCtrl()
{
	CString strTemp;

	/* force maskrom*/
	if (BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_FORCE_MASKROM))->GetCheck()&BST_CHECKED)) {
		m_Configs.bForceMskrom = TRUE;
	} else {
		m_Configs.bForceMskrom = FALSE;
	}
	if (BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_COMPAT))->GetCheck()&BST_CHECKED)) {
		m_Configs.bCompatRw = TRUE;
	} else {
		m_Configs.bCompatRw = FALSE;
	}
	if (BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_REBOOT))->GetCheck()&BST_CHECKED)) {
		m_Configs.bRebootDevice = TRUE;
	} else {
		m_Configs.bRebootDevice = FALSE;
	}
	if (BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_RPMB))->GetCheck()&BST_CHECKED)) {
		m_Configs.bRPMB = TRUE;
	} else {
		m_Configs.bRPMB = FALSE;
	}
	if (BST_CHECKED == (((CButton *)GetDlgItem(IDC_CHECK_LOCK))->GetCheck()&BST_CHECKED)) {
		m_Configs.bLockConfig = TRUE;
	} else {
		m_Configs.bLockConfig = FALSE;
	}
	m_CeditLoader.GetWindowText(strTemp);
	m_Configs.strLoaderPath = strTemp.GetString();

}

void CDlgSettings::OnTcnSelchangingTabItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_TabItem.GetCurSel();

	if(m_DlgItemCmn.SendMessage(WM_ITEM_PRESWITCH, 1, index)) {
		*pResult = 0;
	} else {
		*pResult = TRUE;
	}
}

void CDlgSettings::OnTcnSelchangeTabItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_TabItem.GetCurSel();

	m_DlgItemCmn.SendMessage(WM_ITEM_SWITCH, 1, index);
	*pResult = 0;
}

BOOL CDlgSettings::OnUpdateTab(int iCurSel)
{
	return TRUE;
}
void CDlgSettings::OnBnClickedCheckForceMaskrom()
{
	// TODO: Add your control notification handler code here
	CString strLoaderFileName;
    TCHAR szFilters[] =  _T("binary\0*.bin\0");
    CFileDialog OpenDlg(TRUE);

	if(BST_CHECKED == (m_btnForceMaskrom.GetCheck()&BST_CHECKED)) {
		m_btnForceMaskrom.SetCheck(FALSE);
		/*GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(_T(""));*/
		/* m_Configs.bSetLoader = FALSE;*/
	} else {
		OpenDlg.m_ofn.lpstrTitle = _T("Open File");
		OpenDlg.m_ofn.lpstrFilter = szFilters;
		if (IDOK == OpenDlg.DoModal()) {
			strLoaderFileName = OpenDlg.GetPathName();
			m_CeditLoader.SetWindowText(strLoaderFileName);
			m_btnForceMaskrom.SetCheck(TRUE);
			m_Configs.bSetLoader = TRUE;
			m_Configs.strLoaderPath = strLoaderFileName.GetString();
		} else {
			/* ((CButton *)GetDlgItem(IDC_CHECK_LOADER))->SetCheck(FALSE); */
			/* GetDlgItem(IDC_EDIT_LOADER)->SetWindowText(_T("")); */
			/* m_Configs.bSetLoader = FALSE; */
		}
	}
}

void CDlgSettings::OnBnClickedCheckCompat()
{
	// TODO: Add your control notification handler code here
}

void CDlgSettings::OnBnClickedCheckReboot()
{
	// TODO: Add your control notification handler code here
}

void CDlgSettings::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgSettings::OnBnClickedCheckLock()
{
	// TODO: Add your control notification handler code here
}
