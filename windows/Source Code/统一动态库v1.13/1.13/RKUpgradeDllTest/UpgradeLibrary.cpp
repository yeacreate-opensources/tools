

#include "stdafx.h"
#include "UpgradeLibrary.h"
#include <stdexcept>
using namespace std;
#pragma   comment(lib,"version.lib")
BOOL CUpgradeLibrary::GetDllVersion(DWORD &dwVerMS,DWORD &dwVerLS)
{
	int iVerInfoSize;
	char *pBuf;
	VS_FIXEDFILEINFO *pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	DWORD dwHandle;
	
	iVerInfoSize = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)m_strLibraryFile,&dwHandle);

	if(iVerInfoSize!=0)
	{
		pBuf = new char[iVerInfoSize];
		if( GetFileVersionInfo((LPTSTR)(LPCTSTR)m_strLibraryFile,0,iVerInfoSize,pBuf) )
		{
			if(VerQueryValue(pBuf,_T("\\"),(void **)&pVsInfo,&iFileInfoSize))
			{
				dwVerMS = pVsInfo->dwFileVersionMS;
				dwVerLS = pVsInfo->dwFileVersionLS;
				delete []pBuf;
				return TRUE;
			}
		}
		delete []pBuf;
	}
	return FALSE;
}
CUpgradeLibrary::CUpgradeLibrary(CString strLibraryFile,BOOL &bOK)
{
	m_strLibraryFile = strLibraryFile;

	m_hinst = NULL;
	m_bInitOK = FALSE;
	
	bOK = Load();
}

CUpgradeLibrary::~CUpgradeLibrary(void)
{
	if (m_hinst)
	{
		FreeLibrary(m_hinst);
		m_hinst = NULL;
	}
}


void* CUpgradeLibrary::ImportFunction(char *name)
{

	void* func = (void*)GetProcAddress(m_hinst, name);
	if (!func)
	{
		throw runtime_error(name);
	}
	
	return func;
}

BOOL CUpgradeLibrary::Load()
{

  if (m_hinst)
    FreeLibrary(m_hinst);


  m_hinst = LoadLibrary(m_strLibraryFile);


  if(!m_hinst)
  {
	  return FALSE;
  }
  try
  {
#ifdef _UNICODE
	initialize = (InitializeFunc *)ImportFunction("RK_InitializeW");
	scan_device = (ScanDeviceFunc *)ImportFunction("RK_ScanDeviceW");   
	download_boot = (DownloadBootFunc *)ImportFunction("RKD_DownloadBoot_W");
	read_lba =  (ReadLbaFunc *)ImportFunction("RKD_ReadLBA_W");
	write_lba = (WriteLbaFunc *)ImportFunction("RKD_WriteLBA_W");
	set_firmware = (SetFirmwareFunc *)ImportFunction("RK_SetFirmwareW");
	upgrade = (UpgradeFunc *)ImportFunction("RK_UpgradeW");
	unlock_device = (UnlockDeviceFunc *)ImportFunction("RK_UnlockDeviceW");
#else
	initialize = (InitializeFunc *)ImportFunction("RK_InitializeA");
	scan_device = (ScanDeviceFunc *)ImportFunction("RK_ScanDeviceA");
	download_boot = (DownloadBootFunc *)ImportFunction("RKD_DownloadBoot_A");
	read_lba =  (ReadLbaFunc *)ImportFunction("RKD_ReadLBA_A");
	write_lba = (WriteLbaFunc *)ImportFunction("RKD_WriteLBA_A");
	set_firmware = (SetFirmwareFunc *)ImportFunction("RK_SetFirmwareA");
	upgrade = (UpgradeFunc *)ImportFunction("RK_UpgradeA");
	unlock_device = (UnlockDeviceFunc *)ImportFunction("RK_UnlockDeviceA");
#endif
	  deinitialize = (DeinitializeFunc *)ImportFunction("RK_Uninitialize");
	  wait_device = (WaitDeviceFunc *)ImportFunction("RK_WaitDevice");
	  erase_flash = (EraseFlashFunc *)ImportFunction("RK_EraseFlash");
	  read_custom_data = (ReadCustomDataFunc *)ImportFunction("RK_ReadCustomData");
	  write_custom_data = (WriteCustomDataFunc *)ImportFunction("RK_WriteCustomData");
	  read_sn = (ReadSNFunc *)ImportFunction("RK_ReadSN");
	  write_sn = (WriteSNFunc *)ImportFunction("RK_WriteSN");
	  read_mac = (ReadMACFunc *)ImportFunction("RK_ReadMAC");
	  write_mac = (WriteMACFunc *)ImportFunction("RK_WriteMAC");
	  read_wifi = (ReadWifiFunc *)ImportFunction("RK_ReadWifi");
	  write_wifi = (WriteWifiFunc *)ImportFunction("RK_WriteWifi");
	  read_bt = (ReadBTFunc *)ImportFunction("RK_ReadBT");
	  write_bt = (WriteBTFunc *)ImportFunction("RK_WriteBT");
	  clear_all_info = (ClearAllInfoFunc *)ImportFunction("RK_ClearAllInfo");
	  reset_rockusb = (ResetRockusbFunc *)ImportFunction("RK_ResetRockusb");

  }
  catch (runtime_error err)
  {
	  return FALSE;
  }
  return TRUE;
}

