
#ifndef UPGRADELIBRARY_H_
#define UPGRADELIBRARY_H_

typedef enum
{
	RKUSB_NONE=0x0,
	RKUSB_MASKROM=0x01,
	RKUSB_LOADER=0x02,
	RKUSB_MSC=0x04,
	RKUSB_ADB=0x08,
	RKUSB_MTP=0x10,
	RKUSB_UVC=0x20
}ENUM_RKUSB_TYPE;

#pragma pack(1)

typedef struct _INIT_DEV_INFO
{	
	BOOL	bScan4FsUsb;				// Scan for full speed usb device
	USHORT	usRockusbVid;
	USHORT	usRockusbPid;
	USHORT	usRockMscVid;
	USHORT	usRockMscPid;
	USHORT  usRockAdbVid;
	USHORT  usRockAdbPid;
	USHORT  usRockUvcVid;
	USHORT  usRockUvcPid;
	USHORT  usRockMtpVid;
	USHORT  usRockMtpPid;
	UINT	uiRockusbTimeout;
	UINT	uiRockMscTimeout;
	UINT	emSupportDevice;
}INIT_DEV_INFO,PINIT_DEV_INFO;

typedef struct _INIT_LOG_INFO_W
{
	BOOL bLogEnable;
	LPWSTR lpszLogPathName;
}INIT_LOG_INFO_W,PINIT_LOG_INFO_W;
typedef struct _INIT_LOG_INFO_A
{
	BOOL bLogEnable;
	LPSTR lpszLogPathName;
}INIT_LOG_INFO_A,PINIT_LOG_INFO_A;
#ifdef _UNICODE
#define INIT_LOG_INFO INIT_LOG_INFO_W
#define PINIT_LOG_INFO PINIT_LOG_INFO_W
#else
#define INIT_LOG_INFO INIT_LOG_INFO_A
#define PINIT_LOG_INFO PINIT_LOG_INFO_A
#endif

typedef struct _INIT_CALLBACK_INFO
{
	LPVOID pUpgradeStepPromptProc;	// Prompt certain step status is started, passed or failed.
	LPVOID pProgressPromptProc;		// Prompt progress of certain operation which may take long time.
}INIT_CALLBACK_INFO,PINIT_CALLBACK_INFO;

// Struct of device information
typedef struct _STRUCT_DEVICE_DESC_W
{
	USHORT	usVid;
	USHORT	usPid;
	DWORD	dwDeviceInstance;
	WCHAR   szLinkName[MAX_PATH];
	DWORD	dwLayer;   
	UINT	emUsbType;
	UINT	emDeviceType;
	BOOL	bUsb20;
}STRUCT_DEVICE_DESC_W,*PSTRUCT_DEVICE_DESC_W;
typedef struct _STRUCT_DEVICE_DESC_A
{
	USHORT	usVid;
	USHORT	usPid;
	DWORD	dwDeviceInstance;
	CHAR    szLinkName[MAX_PATH];
	DWORD	dwLayer;
	UINT	emUsbType;
	UINT	emDeviceType;
	BOOL	bUsb20;
}STRUCT_DEVICE_DESC_A,*PSTRUCT_DEVICE_DESC_A;
#ifdef _UNICODE
#define STRUCT_DEVICE_DESC STRUCT_DEVICE_DESC_W
#define PSTRUCT_DEVICE_DESC PSTRUCT_DEVICE_DESC_W
#else
#define STRUCT_DEVICE_DESC STRUCT_DEVICE_DESC_A
#define PSTRUCT_DEVICE_DESC PSTRUCT_DEVICE_DESC_A
#endif
typedef struct _STRUCT_UPGRADE_PARAM_A 
{
	BOOL bEnableFormat; //format user disk  
	BOOL bEnableCopyData;//copy file or dir to user disk 
	BOOL bEnableFormatDataDisk;//format data disk  <android not support>
	BOOL bEnableCopyDataDiskData;//copy file or dir to data disk <android not support>
	LPSTR lpszDiskVolume;//user disk volume,can be NULL									  
	LPSTR lpszDataPath;//path of file|dir copying to user disk,can be NULL
	LPSTR lpszDataDiskVolume;//data disk volume,can be NULL
	LPSTR lpszDataDiskPath;//path of file|dir copying to data disk,can be NULL
	UINT  uiMiscModifyFlag;//0:not change misc.img,1:wipe_all 2:wipe_data(only support android)
	BOOL	bMscReset;//true:after copying data,device reset automatically,default is false
	BOOL	bResetAfterUpgrade;//true:after finishing upgrade,device reset(default),false:not to reset device
	USHORT usIgnoreIDBUpgradeFlag;//0x5AA5:upgrade firmware without upgrading idblock only in upgrade function
}STRUCT_UPGRADE_PARAM_A,*PSTRUCT_UPGRADE_PARAM_A;

typedef struct _STRUCT_UPGRADE_PARAM_W 
{
	BOOL bEnableFormat; 
	BOOL bEnableCopyData;
	BOOL bEnableFormatDataDisk;
	BOOL bEnableCopyDataDiskData;
	LPWSTR lpszDiskVolume;									  
	LPWSTR lpszDataPath;
	LPWSTR lpszDataDiskVolume;
	LPWSTR lpszDataDiskPath;
	UINT  uiMiscModifyFlag;
	BOOL	bMscReset;
	BOOL	bResetAfterUpgrade;
	USHORT usIgnoreIDBUpgradeFlag;
}STRUCT_UPGRADE_PARAM_W,*PSTRUCT_UPGRADE_PARAM_W;

#ifdef _UNICODE
#define STRUCT_UPGRADE_PARAM STRUCT_UPGRADE_PARAM_W
#define PSTRUCT_UPGRADE_PARAM PSTRUCT_UPGRADE_PARAM_W
#else
#define STRUCT_UPGRADE_PARAM STRUCT_UPGRADE_PARAM_A
#define PSTRUCT_UPGRADE_PARAM PSTRUCT_UPGRADE_PARAM_A
#endif
#pragma pack()
#ifdef _UNICODE
	typedef BOOL _stdcall InitializeFunc(INIT_DEV_INFO InitDevInfo, INIT_LOG_INFO_W InitLogInfo, INIT_CALLBACK_INFO InitCallbackInfo);
	typedef int  _stdcall ScanDeviceFunc(PSTRUCT_DEVICE_DESC_W *ppDevs);
	typedef int  _stdcall DownloadBootFunc(PWCHAR pszDevicePath,PWCHAR pszFile);
	typedef int  _stdcall ReadLbaFunc(PWCHAR pszDevicePath,DWORD dwOffset,DWORD dwCount,PBYTE pBuf);
	typedef int  _stdcall WriteLbaFunc(PWCHAR pszDevicePath,DWORD dwOffset,DWORD dwCount,PBYTE pBuf);
	typedef BOOL _stdcall SetFirmwareFunc(LPCWSTR lpszFirmwarePathName,BOOL bCheckFw);
	typedef BOOL _stdcall UpgradeFunc(STRUCT_UPGRADE_PARAM_W &upgradeParam,DWORD dwLayer);
	typedef BOOL _stdcall UnlockDeviceFunc(LPCWSTR lpszCertPath,DWORD dwLayer);
#else
	typedef BOOL _stdcall InitializeFunc(INIT_DEV_INFO InitDevInfo, INIT_LOG_INFO_A InitLogInfo, INIT_CALLBACK_INFO InitCallbackInfo);
	typedef int  _stdcall ScanDeviceFunc(PSTRUCT_DEVICE_DESC_A *ppDevs);
	typedef int  _stdcall DownloadBootFunc(PCHAR pszDevicePath,PCHAR pszFile);
	typedef int  _stdcall ReadLbaFunc(PCHAR pszDevicePath,DWORD dwOffset,DWORD dwCount,PBYTE pBuf);
	typedef int  _stdcall WriteLbaFunc(PCHAR pszDevicePath,DWORD dwOffset,DWORD dwCount,PBYTE pBuf);
	typedef BOOL _stdcall SetFirmwareFunc(LPCSTR lpszFirmwarePathName,BOOL bCheckFw);
	typedef BOOL _stdcall UpgradeFunc(STRUCT_UPGRADE_PARAM_A &upgradeParam,DWORD dwLayer);
	typedef BOOL _stdcall UnlockDeviceFunc(LPCSTR lpszCertPath,DWORD dwLayer);
#endif
	typedef BOOL _stdcall WaitDeviceFunc(DWORD dwLayer,UINT uiUsbType);
	typedef BOOL _stdcall DeinitializeFunc();
	typedef BOOL _stdcall EraseFlashFunc(DWORD dwLayer);
	typedef BOOL _stdcall ReadCustomDataFunc(PBYTE pCustomData, INT &nCustomDataLen,INT &nCustomDataOffset,DWORD dwLayer);
	typedef BOOL _stdcall WriteCustomDataFunc(PBYTE pCustomData, INT nCustomDataOffset,INT nCustomDataLen,DWORD dwLayer);
	typedef BOOL _stdcall ReadSNFunc(PBYTE pSN, INT& nSNLen,DWORD dwLayer);
	typedef BOOL _stdcall WriteSNFunc(PBYTE pSN, INT nSNLen,DWORD dwLayer);
	typedef BOOL _stdcall ReadMACFunc(PBYTE pMac, INT& nMacLen,DWORD dwLayer);
	typedef BOOL _stdcall WriteMACFunc(PBYTE pMac, INT nMacLen,DWORD dwLayer);
	typedef BOOL _stdcall ReadWifiFunc(PBYTE pWifi, INT& nWifiLen,DWORD dwLayer);
	typedef BOOL _stdcall WriteWifiFunc(PBYTE pWifi, INT nWifiLen,DWORD dwLayer);
	typedef BOOL _stdcall ReadBTFunc(PBYTE pBT, INT& nBTLen,DWORD dwLayer);
	typedef BOOL _stdcall WriteBTFunc(PBYTE pBT, INT nBTLen,DWORD dwLayer);
	typedef BOOL _stdcall ClearAllInfoFunc(DWORD dwLayer);
	typedef BOOL _stdcall ResetRockusbFunc(BYTE subCode,DWORD dwLayer);

/*---------------------------------------------*/
/* Upgrade Library Class (C++)                  */
/*---------------------------------------------*/
class CUpgradeLibrary
{
public:
  CUpgradeLibrary(CString strLibraryFile,BOOL &bOK);
  ~CUpgradeLibrary(void);
private:
  HINSTANCE m_hinst;        //< Pointer to the loaded library
  CString m_strLibraryFile;
  BOOL Load();
  void* ImportFunction(char *name);
  BOOL GetDllVersion(DWORD &dwVerMS,DWORD &dwVerLS);
public:
  BOOL m_bInitOK;
  InitializeFunc *initialize;
  DeinitializeFunc *deinitialize;
  WaitDeviceFunc  *wait_device;
  ScanDeviceFunc  *scan_device;
  DownloadBootFunc *download_boot;
  WriteLbaFunc    *write_lba;
  ReadLbaFunc     *read_lba;
  SetFirmwareFunc *set_firmware;
  EraseFlashFunc  *erase_flash;
  UpgradeFunc     *upgrade;
  ReadCustomDataFunc *read_custom_data;
  WriteCustomDataFunc *write_custom_data;
  ReadSNFunc  *read_sn;
  WriteSNFunc *write_sn;
  ReadMACFunc *read_mac;
  WriteMACFunc *write_mac;
  ReadBTFunc  *read_bt;
  WriteBTFunc *write_bt;
  ReadWifiFunc *read_wifi;
  WriteWifiFunc *write_wifi;
  ClearAllInfoFunc *clear_all_info;
  ResetRockusbFunc *reset_rockusb;
  UnlockDeviceFunc *unlock_device;
};

#endif /*UPGRADELIBRARY_H_ */

