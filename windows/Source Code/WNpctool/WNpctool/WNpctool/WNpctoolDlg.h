
// WNpctoolDlg.h : ͷ�ļ�
//

#pragma once
#include "cmLog.h"
#include "cmIniFile.h"
#include "cmPath.h"
#include "cmFile.h"
#include "cmNumString.h"
#include "cmStrCode.h"
#include "cmCommonDlg.h"
using namespace cm;
#include "settings/SettingBase.h"
#include "afxwin.h"
#include "ConfigMode.h"
#include "afxcmn.h"
#include "./XListBox/XListBox.h"
#include "FontStatic/FontStatic.h"
#include "debug.h"

#define WM_COM_ADD		    1
#define WM_COM_RM		    2
#define UPDATE_LIST         3
#define LIST_EMPTY          4
#define UPDATE_TEST_EXIT    5
#define UPDATE_WINDOW		6
#define UPDATE_PROMPT		7

#define PROMPT_TESTING 1
#define PROMPT_PASS 2
#define PROMPT_FAIL 3
#define PROMPT_EMPTY 4

#define LIST_INFO 0
#define LIST_TIME 1
#define LIST_WARN 2
#define LIST_ERR  3

#define FIELD_DEVSN     1
#define FIELD_WIFIMAC   2
#define FIELD_BTMAC		4
#define FIELD_LANMAC    8
#define FIELD_IMEI	   16
#define FIELD_ALL      32

enum ENUM_WRITEITEM_ID{
	ITEM_SN = 1,
	ITEM_WIFIMAC,
	ITEM_LANMAC,
	ITEM_BTMAC,
	ITEM_IMEI,
	ITEM_OEMUNLOCK,				//GVA
	ITEM_SENSOR_CAL
};
typedef struct  
{
	int flag;
	TCHAR   pszLineText[MAX_PATH];
}STRUCT_LIST_LINE,*PSTRUCT_LIST_LINE;

typedef struct _STRUCT_BURNINGITEM
{
	USHORT nID;
	CString strValue;
}STRUCT_BURNINGITEM,*PSTRUCT_BURNINGITEM;
typedef vector<STRUCT_BURNINGITEM>	BURNINGITEM_VECTOR;
// CWNpctoolDlg �Ի���
class CWNpctoolDlg : public CDialog
{
// ����
public:
	CWNpctoolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WNPCTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
	CIniSettingBase m_Configs;
	CIniLocalLan	m_LocalLan;

	void			ScanDeviceProc();
	BOOL			WriteProc();
	BOOL			ReadProc();
	BOOL			WriteItem(int nItemID);
	BOOL			ReadItem(int nItemID,CString &strNum);
	BOOL			WriteMac(CString strMac,int nItemID);
	BOOL			ReadMac(int nItemID);
	CString			BytesToHexStr(PBYTE pBuf,int nBufLen);
	BOOL			HexStrToBytes(CString strHex,PBYTE pBuf,int &nBufLen);
	LRESULT			OnHandleUpdateMsg(WPARAM wParam,LPARAM lParam);
	void			AddPrompt(CString strPrompt,int flag);
	VOID			WalkMenu(CMenu *pMenu,CString strMainKeyPart);
	BOOL			PreTranslateMessage(MSG* pMsg);
	vector<int>		vecEdit;
	BOOL			bSN,bWifiMac,bLanMac,bImei,bBtMac;
	std::wstring	strUnlock;
	
private:
	CString         m_strModulePath;
	CString			m_strLogPath;
	CString			m_strLoader;
	CLogger         *m_pLog;

	CString         m_strCurDevSn;
	CString         m_strCurWifiMac;
	CString         m_strCurBtMac;
	CString         m_strCurLanMac;
	CString         m_strCurImei;

	BOOL			m_bUserStop;
	BOOL            m_bStarWrite;
	BOOL            m_bStarRead;
	CWinThread      *m_pReadThread;
	CWinThread      *m_pWriteThread;
	CWinThread		*m_pScanThread;
	CEvent			*m_pScanEvent;
	CCriticalSection m_csScanLock;
	CCriticalSection m_csUpdateCount;
	BOOL			m_bUpgradeDllInitOK;
	BOOL			m_bTerminated;
	UINT			m_nDeviceCount;
	BOOL			m_bExistMsc;
	BOOL			m_bExistAdb;
	BOOL			m_bExistLoader;
	BOOL			m_bDownBoot;
	BOOL			m_bRun;

	BOOL            m_bRedLedLight;
	HBITMAP         m_hGreenLedBitmap;
	HBITMAP         m_hRedLedBitmap;
	//CConfigMode m_ConfigModeDlg;

	CStatic		m_lblDevice;
	CFontStatic m_lblPrompt;
	CXListBox	m_listInfo;

	BOOL    SaveWriteResultOnPass(BOOL,DWORD);
	BOOL	LoadConfig();
	VOID	InitUi();
	BOOL	OnStartRead();
	BOOL	OnStartWrite(bool bAuto);
	std::wstring GetLocalString(std::wstring strKey);
	VOID    UpdateMenuItem();
	CString IntStrIncreases(CString);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BURNINGITEM_VECTOR m_arrayDownloadItem;
	afx_msg void OnLogFolder();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnWrite();
	afx_msg void OnSettingMode();
	afx_msg void OnUpdateSettingRead(CCmdUI *pCmdUI);
	afx_msg void OnSettingRead();
	afx_msg void OnLanguageChinese();
	afx_msg void OnUpdateLanguageChinese(CCmdUI *pCmdUI);
	afx_msg void OnLanguageEnglish();
	afx_msg void OnUpdateLanguageEnglish(CCmdUI *pCmdUI);
	afx_msg void OnSettingAuto();
	afx_msg void OnUpdateSettingAuto(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButtonLoader();
	afx_msg void OnSettingReset();
	afx_msg void OnUpdateSettingReset(CCmdUI *pCmdUI);
	afx_msg void OnEnKillfocusEditSn();
	CStatic m_staticSuccessCount;
	CStatic m_staticFailCount;
	CStatic m_staticTotalCount;
	CButton m_oemLock;
	afx_msg void OnBnClickedCheckOem();
};
