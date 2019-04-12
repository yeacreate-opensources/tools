#ifndef CM_FILE_H
#define CM_FILE_H

namespace cm
{
	typedef enum
	{
		IE_RET_NOT_EXIST=0,
		IE_RET_EXIST_FILE,
		IE_RET_EXIST_DIR
	}ENUM_ISEXISTED_RETURN;
#define FO_ERROR_SUCCESS	0
#define FO_ERROR_RELATIVE_PATH -1
#define FO_ERROR_NOT_DIR -2
#define FO_ERROR_FILESTATUS_FAIL -3
	class cmFile
	{
	public:
		static ENUM_ISEXISTED_RETURN IsExisted(CString strPath);
		//dir operation
		static int MakeDir(CString strDir);
		static int MakeAllDir(CString strDir);
		static BOOL DelDir(CString strDir,BOOL bDelSelf=TRUE);
		static BOOL CopyDir(CString strDir,CString strDest);
		static BOOL MoveDir(CString strDir,CString strDest);
		static BOOL GetDirInfo(CString strDir,ULONGLONG &ullSize,int &nDir,int &nFile);
		static BOOL IsEmptyDir(CString strDir);
		static BOOL Rename(CString strOld,CString strNew);
		static int GetLastFOError();
	};
}

#endif