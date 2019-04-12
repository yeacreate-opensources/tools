// DlgItemCmn.cpp : implementation file
//

#include "stdafx.h"
#include "rk_provision_tool.h"
#include "DlgItemCmn.h"


// CDlgItemCmn dialog
IMPLEMENT_DYNAMIC(CDlgItemCmn, CDialog)

CDlgItemCmn::CDlgItemCmn(CIniSettingBase &Config, CIniLocalLan &LocalLang, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItemCmn::IDD, pParent), m_Configs(Config), m_LocalLan(LocalLang)
{
	memset(&m_bHaveUpdateToCtrl, 0,sizeof(m_bHaveUpdateToCtrl));
}

CDlgItemCmn::~CDlgItemCmn()
{
}

void CDlgItemCmn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_CCombType);
	DDX_Control(pDX, IDC_STATIC_AUDO, m_CGrpAuto);
	DDX_Control(pDX, IDC_STATIC_FILE, m_CGrpFile);
	DDX_Control(pDX, IDC_CHECK_ENABLE, m_CBtnEnable);
}

BOOL CDlgItemCmn::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CCombType.InsertString(0, GetLocalString(_T("IDS_MANUAL")).c_str());
	m_CCombType.InsertString(1, GetLocalString(_T("IDS_AUTO")).c_str());
	m_CCombType.InsertString(2, GetLocalString(_T("IDS_FILE")).c_str());
	m_LocalLan.TreeControls(m_hWnd, m_Configs.bDebug ? TRUE : FALSE, this->IDD, false);
	m_CurItemSel = 0;
	OnItemSwitch(m_CurItemSel);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDlgItemCmn, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDlgItemCmn::OnCbnSelchangeComboType)
	ON_MESSAGE(WM_ITEM_SWITCH, &CDlgItemCmn::OnItemSwitchMessage)
	ON_MESSAGE(WM_SETTING_OK, &CDlgItemCmn::OnSettingOkMessage) 
	ON_MESSAGE(WM_ITEM_PRESWITCH, &CDlgItemCmn::OnItemPreSwitchMessage) 
	
	ON_EN_SETFOCUS(IDC_EDIT_ID, &CDlgItemCmn::OnEnSetfocusEditId)
	ON_EN_CHANGE(IDC_EDIT_ID, &CDlgItemCmn::OnEnChangeEditId)
	ON_BN_CLICKED(IDC_CHECK_ENABLE, &CDlgItemCmn::OnBnClickedCheckEnable)


END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgItemCmn)

END_DHTML_EVENT_MAP()



// CDlgItemCmn message handlers

void CDlgItemCmn::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
	item_info * piinfo = &m_iinfo[m_CurItemSel];
	int index = m_CCombType.GetCurSel();

	OnCtrlEnable(-1, index);
	piinfo->iMode = index;

}

BOOL CDlgItemCmn::OnSettingOk()
{
	int i;
	item_info * piinfo_left, *piinfo_right;

	for (i = 0; i < ITEM_WVCNT; i++) {
		if(m_bHaveUpdateToCtrl[i]) {
			piinfo_left =  &m_Configs.iinfo[i];
			piinfo_right = &m_iinfo[i];
			piinfo_left->bEnable = piinfo_right->bEnable;
			piinfo_left->iMode = piinfo_right->iMode;
			piinfo_left->strItemStart =  piinfo_right->strItemStart;
			piinfo_left->strPrefix =  piinfo_right->strPrefix;
			piinfo_left->strSuffix = piinfo_right->strSuffix;
			piinfo_left->strItemCurrent = piinfo_right->strItemCurrent;
			piinfo_left->nItemCount = piinfo_right->nItemCount;
			piinfo_left->nRemainCount = piinfo_right->nRemainCount;
			piinfo_left->bEnable = piinfo_right->bEnable;
		}
	}
	return TRUE;
}

BOOL CDlgItemCmn::CheckItemValid(int id, const TCHAR *szStr)
{
	if(_tcsstr(index_id_map[id].name, _T("MAC"))) {
		if(check_mac(szStr, 0)) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else if (_tcsstr(index_id_map[id].name, _T("IMEI"))){
		if(check_imei(szStr, 1)) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else if (_tcsstr(index_id_map[id].name, _T("SN"))){
		if(check_sn(szStr, 0)) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return TRUE;

}

BOOL CDlgItemCmn::OnCheckFileItem(int iCurSel)
{

	return TRUE;
}

BOOL CDlgItemCmn::OnCheckAutoItem(int iCurSel)
{
	CString strTmp, strPrompt;
	size_t len;
	item_info * piinfo;

	/* save result */
	piinfo = &m_iinfo[iCurSel];
	if(!piinfo->strItemStart.empty()) {
		if(!CheckItemValid(iCurSel, piinfo->strItemStart.c_str())) {
			strPrompt.Format(GetLocalString(_T("IDS_ERROR_SS_INVALID")).c_str(), index_id_map[iCurSel].name);
			AfxMessageBox(strPrompt);
			return FALSE;
		}
	} else {
		strPrompt.Format(GetLocalString(_T("IDS_ERROR_SS_INVALID")).c_str(), index_id_map[iCurSel].name);
		AfxMessageBox(strPrompt);
	}
	
	if(!piinfo->strPrefix.empty()) {
		if( 0 != piinfo->strItemStart.find(piinfo->strPrefix)){
			strPrompt.Format(GetLocalString(_T("IDS_ERROR_SS_PRE_INVALID")).c_str(), index_id_map[iCurSel].name);
			AfxMessageBox(strPrompt);
			return FALSE;
		}
	}

	if(!piinfo->strSuffix.empty()) {
		len = piinfo->strItemStart.length() - piinfo->strSuffix.length();
		if( len != piinfo->strItemStart.rfind(piinfo->strSuffix)){
			strPrompt.Format(GetLocalString(_T("IDS_ERROR_SS_SUF_INVALID")).c_str(), index_id_map[iCurSel].name);
			AfxMessageBox(strPrompt);
			return FALSE;
		}
	}

	if(piinfo->nItemCount < 0 && piinfo->nItemCount !=  0){
		strPrompt.Format(GetLocalString(_T("IDS_ERROR_SS_CNT_INVALID")).c_str(), index_id_map[iCurSel].name);
		AfxMessageBox(strPrompt);
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgItemCmn::OnItemPreSwitch(int iCurSel)
{
	CString strTmp, strPrompt;
	int index;
	item_info * piinfo;
	int iCount;
	BOOL bNeedAuto = FALSE;

	/* save result */
	piinfo = &m_iinfo[iCurSel];

	index = m_CCombType.GetCurSel();
	piinfo->iMode = index;

	if(BST_CHECKED == (m_CBtnEnable.GetCheck()&BST_CHECKED)) {
		piinfo->bEnable = TRUE;
	} else {
		piinfo->bEnable = FALSE;
	}
	/* auto increase */
	GetDlgItem(IDC_EDIT_START)->GetWindowText(strTmp);
	if(strTmp.Compare(piinfo->strItemStart.c_str())) {
		bNeedAuto = TRUE;
	}
	piinfo->strItemStart =  (LPCTSTR)strTmp;


	GetDlgItem(IDC_EDIT_PREFIX)->GetWindowText(strTmp);
	piinfo->strPrefix =  (LPCTSTR)strTmp;

	GetDlgItem(IDC_EDIT_SUFFIX)->GetWindowText(strTmp);
	piinfo->strSuffix = (LPCTSTR)strTmp;

	//GetDlgItem(IDC_EDIT_CURRENT)->GetWindowText(strTmp);
	//piinfo->strItemCurrent =  (LPCTSTR)strTmp;


	GetDlgItem(IDC_EDIT_COUNT)->GetWindowText(strTmp);
	iCount = _ttoi((LPCTSTR)strTmp);
	if(iCount != piinfo->nItemCount) {
		bNeedAuto = TRUE;
		piinfo->nItemCount = iCount;
	}

	//GetDlgItem(IDC_EDIT_LEFT)->GetWindowText(strTmp);
	//piinfo->nRemainCount = _ttoi((LPCTSTR)strTmp);

	if(piinfo->bEnable && 1 == piinfo->iMode) {
		if(!OnCheckAutoItem(iCurSel))
			return FALSE;
	}
	if(bNeedAuto) {
		piinfo->strItemCurrent = piinfo->strItemStart;
		piinfo->nRemainCount = iCount;
	}
	return TRUE;
}

std::wstring CDlgItemCmn::GetLocalString(std::wstring strKey)
{
	return m_LocalLan.GetLanStr(strKey);
}

BOOL CDlgItemCmn::OnItemSwitch(int iCurSel)
{
	TCHAR strTmp[32];
	item_info * piinfo;

	if(m_bHaveUpdateToCtrl[iCurSel]) {
		piinfo = &m_iinfo[iCurSel];
	} else {
		piinfo = &m_Configs.iinfo[iCurSel];
		m_bHaveUpdateToCtrl[iCurSel] = TRUE;
	}
	GetDlgItem(IDC_EDIT_START)->SetWindowText(piinfo->strItemStart.c_str());
	GetDlgItem(IDC_EDIT_PREFIX)->SetWindowText(piinfo->strPrefix.c_str());
	GetDlgItem(IDC_EDIT_SUFFIX)->SetWindowText(piinfo->strSuffix.c_str());
	GetDlgItem(IDC_EDIT_CURRENT)->SetWindowText(piinfo->strItemCurrent.c_str());
	wsprintf(strTmp, _T("%d"), piinfo->nItemCount);
	GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(strTmp);
	wsprintf(strTmp, _T("%d"), piinfo->nRemainCount);
	GetDlgItem(IDC_EDIT_LEFT)->SetWindowText(strTmp);
	m_CBtnEnable.SetCheck(piinfo->bEnable ? BST_CHECKED : BST_UNCHECKED);
	m_CCombType.SetCurSel(piinfo->iMode);
	/* OnSwitchType(piinfo->iMode); */
	OnCtrlEnable(piinfo->bEnable ? 1 : 0, piinfo->iMode);

	return TRUE;
}

/* iItemEnable:
 * 0  disabled
 * 1  enable
 * -1 no modify
 */
BOOL CDlgItemCmn::OnCtrlEnable(int iItemEnable, int iCurModeSel)
{
	/* int index = m_CCombType.GetCurSel(); */

	if (0 == iItemEnable) {
		m_CCombType.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
		m_CGrpAuto.EnableWindow(FALSE);
		m_CGrpFile.EnableWindow(FALSE);
	} else if(1 == iItemEnable){
		m_CCombType.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
		if(0 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(FALSE);
			m_CGrpFile.EnableWindow(FALSE);
		}
		if(1 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(TRUE);
			m_CGrpFile.EnableWindow(FALSE);
		}
		if(2 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(FALSE);
			m_CGrpFile.EnableWindow(TRUE);
		}
	} else { /* only change iCurModeSel */
		if(0 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(FALSE);
			m_CGrpFile.EnableWindow(FALSE);
		}
		if(1 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(TRUE);
			m_CGrpFile.EnableWindow(FALSE);
		}
		if(2 == iCurModeSel) {
			m_CGrpAuto.EnableWindow(FALSE);
			m_CGrpFile.EnableWindow(TRUE);
		}
	}
	return TRUE;
}

BOOL CDlgItemCmn::SetConfigs(CIniSettingBase *Config, CIniLocalLan *LocalLang)
{
	return TRUE;
}

LRESULT  CDlgItemCmn::OnItemSwitchMessage(WPARAM wParam, LPARAM lParam)
{
	m_CurItemSel = (int) lParam;
	OnItemSwitch(m_CurItemSel);
	return FALSE;
}

LRESULT  CDlgItemCmn::OnItemPreSwitchMessage(WPARAM wParam, LPARAM lParam)
{
	if(OnItemPreSwitch(m_CurItemSel)) {
		return TRUE;
	}
	return FALSE;
}

LRESULT  CDlgItemCmn::OnSettingOkMessage(WPARAM wParam, LPARAM lParam)
{
	if(OnSettingOk()) {
		return TRUE;
	}
	return FALSE;
}

void CDlgItemCmn::OnEnSetfocusEditId()
{
	// TODO: Add your control notification handler code here
	/* MessageBox(_T(""));*/
}

void CDlgItemCmn::OnEnChangeEditId()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	MessageBox(_T("不建议修改这个值"));
}

void CDlgItemCmn::OnBnClickedCheckEnable()
{
	// TODO: Add your control notification handler code here
	item_info * piinfo = &m_iinfo[m_CurItemSel];
	int index = m_CBtnEnable.GetState();

	if(BST_CHECKED == (m_CBtnEnable.GetCheck()&BST_CHECKED)) {
		OnCtrlEnable(1, piinfo->iMode);
		piinfo->bEnable = TRUE;
	} else {
		OnCtrlEnable(0, -1);
		piinfo->bEnable = FALSE;
	}
}

BOOL CDlgItemCmn::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)) {
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);

}
