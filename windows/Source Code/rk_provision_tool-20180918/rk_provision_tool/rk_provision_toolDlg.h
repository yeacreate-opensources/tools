
// rk_provision_toolDlg.h : header file
//

#pragma once
#include ".\settings\SettingBase.h"
#include ".\XListBox\XListBox.h"
#include "log\debug.h"
#include "spawn\scriptexe.h"
#include "afxwin.h"
#include "CmCheck.h"

//定义Adbwinapi.dll接口
typedef void* ADBAPIHANDLE;
typedef struct _AdbInterfaceInfo 
{
	GUID          class_id;
	unsigned long flags;
	wchar_t       device_name[1];
} AdbInterfaceInfo;
typedef ADBAPIHANDLE (*AdbEnumInterfaces)(GUID class_id,bool exclude_not_present,bool exclude_removed, bool active_only);
typedef bool (*AdbNextInterface)(ADBAPIHANDLE adb_handle,AdbInterfaceInfo* info, unsigned long* size);
typedef bool (*AdbCloseHandle)(ADBAPIHANDLE adb_handle);
typedef bool (*AdbGetSerialNumber)(ADBAPIHANDLE adb_interface,void* buffer,unsigned long* buffer_char_size,bool ansi);
typedef ADBAPIHANDLE (*AdbCreateInterfaceByName)(const wchar_t* interface_name);


#define ANDROID_USB_CLASS_ID \
{0xf72fe0d4, 0xcbcb, 0x407d, {0x88, 0x14, 0x9e, 0xd6, 0x73, 0xd0, 0xdd, 0x6b}};



#define ID_OP_NO -1
#define ID_OP_RD 0
#define ID_OP_WR 1
#define WM_UPDATE_MSG WM_USER+1
#define WM_COM_ADD		    1
#define WM_COM_RM		    2
#define UPDATE_LIST         3
#define LIST_EMPTY          4
#define UPDATE_TEST_EXIT    5 /* just update write btn status */
#define UPDATE_WINDOW		6
#define UPDATE_PROMPT		7

#define SHOW_RESULT 8
#define CLEAR_CONTROL 9
#define DEVICE_CHG 10
#define SET_FOCUS 11


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

typedef struct _CONTROLMAP_ {
	int id;
} CONTROLMAP;

typedef struct _DLG_ITEM_INFO_ {
	PBYTE pData;
	int size;
} DLGITEMINFO, *PDLGITEMINFO;

typedef struct  
{
	int flag;
	TCHAR   pszLineText[MAX_PATH];
}STRUCT_LIST_LINE,*PSTRUCT_LIST_LINE;

// Crk_provision_toolDlg dialog
class Crk_provision_toolDlg : public CDialog
{
// Construction
public:
	Crk_provision_toolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RK_PROVISION_TOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	/* SYSTEM CONFIG AND STATUS*/
	CString m_strModulePath;
	TCHAR m_szLogPath[MAX_PATH];
	CIniSettingBase m_Configs;
	CIniLocalLan m_LocalLan;
	CString m_strLoader;

    DLGITEMINFO m_ItemInfo[ITEM_WVCNT];

	/* write status */
	BOOL m_bStarted;
	BOOL m_bRunning;
	BOOL m_bUserStop;
	int m_CurOp;

	CWinThread *m_pReadThread;
	CWinThread *m_pWriteThread;

	/* used to access running status variable,such as:
	 * m_bStarted; m_bRunning; m_bUserStop;
	 *
	 */
	CCriticalSection m_criSection;
	/* scan devices about variable */
	CWinThread *m_pScanThread;
	CEvent m_ScanEvent;
    CEvent m_ScanExitEvent;
    CEvent m_ScanExitNotify;
	CEvent m_MaksDeviceEvent;
	CEvent m_LoaderDeviceEvent;

	BOOL m_bRedLedLight;
	HBITMAP m_hGreenLedBitmap;
	HBITMAP m_hRedLedBitmap;
	BOOL m_bUpgradeDllInitOK;

	/* used to access m_nDeviceCount */
	CCriticalSection m_csScanLock;
	UINT m_nDeviceCount;

	CLogger *get_log();
	BOOL log_enable();
	CLogger *pLog;

    CString m_strLinkName;
    CString m_strDevice; /* MASK ADB LOADER */
    BOOL AdbToRockusb();
	BOOL GetAdbSerialno(CString strDevPath, CString &strSerialno);
#if 0
	CMenu m_Menu;
#endif
	BOOL WriteProc();
	BOOL ReadProc();
	void ScanDeviceProc();
	BOOL LoadConfig();
	void OnUpdateCtrl();
	void OnClearCtrl(BOOL bAll = FALSE);
	void OnLockCtrl();
    VOID WalkMenu(CMenu *pMenu,CString strMainKeyPart);
	std::wstring GetLocalString(std::wstring strKey);
	/* my funcs */
	void OnBnClickedItem(int id);
	BOOL CheckItemValid(int id);
	void FreeBuffer(PDLGITEMINFO pItemInfo);
	BOOL AllocBuffer(PDLGITEMINFO pItemInfo, int iItemSize);
	CString SetItemString(int id, const BYTE *pBuf, int len);
	BOOL SetItem(int id, BYTE *pBuf, int len);
	BOOL GetItem(int id);
	void AddPrompt(CString strPrompt,int flag);
	void OnOperateChg(int RW, int status);
	VOID SetControlsFocus(BOOL bFirst = TRUE);
	BOOL IsInputControlId(INT Id);
	INT NextFocusControlId(INT startId, BOOL loop);
	int ControlsIdToItemId(int ctrl_id);
	INT m_CurCtrlID;
	BOOL IsLastInputControlId(int id);
	BOOL SaveWriteResultOnPass(BOOL bPass, DWORD mask);
	BOOL ShowReadResultOnPass(BOOL bPass, DWORD mask);

	int ReadItem(int id, BYTE *buf, int len);
	int WriteItem(int id, BYTE *buf, int len);
	int WriteVendorRpmbItem(BYTE dest, int nItemID, BYTE *buf, int len);
	int ReadVendorRpmbItem(BYTE dest, int nItemID, BYTE *buf, int len);
	int CompatRead(int id, PBYTE pSN, INT& nSNLen, DWORD dwLayer=0);
	int CompatWrite(int id, PBYTE pSN, INT& nSNLen, DWORD dwLayer=0);
	int CompatWriteItem(int id, BYTE *buf, int len);
	int CompatReadItem(int id, BYTE *buf, int len);
	int DebugReadItem(int id, BYTE *buf, int len);
	int DebugWriteItem(int id, BYTE *buf, int len);

	void OnDeviceArrival();
	void OnDeviceRemoval();
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonWrite();
	BOOL OnStartWriteLocked(BOOL bDoSingle);
	BOOL OnStartReadLocked(BOOL bDoSingle);
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnClose();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuSettings();
	afx_msg void OnBnClickedCheckSn();
	afx_msg void OnBnClickedCheckWifi();
	afx_msg void OnBnClickedCheckLan();
	afx_msg void OnBnClickedCheckBt();
	afx_msg void OnBnClickedCheckImei();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_CBtnEnableSn;
	CButton m_CBtnEnableWifi;
	CButton m_CBtnEnableLan;
	CButton m_CBtnEnableBt;
	CButton m_CBtnEnableImei;
	afx_msg LRESULT OnHandleUpdateMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedCheckSingle();
	CStatic m_lblDevice;
	CStatic m_PicDevice;
	CXListBox m_listInfo;
	afx_msg void OnEnSetfocusEditSn();
	afx_msg void OnEnSetfocusEditWifi();
	afx_msg void OnEnSetfocusEditBt();
	afx_msg void OnEnSetfocusEditImei();
	afx_msg void OnEnSetfocusEditLan();
	afx_msg void OnBnClickedCheckLoader();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonBrowLoader();
	afx_msg void OnMenuLog();
	afx_msg void OnBnClickedButtonReboot();
};
