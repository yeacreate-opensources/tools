#include "stdafx.h"
#include "CmCheck.h"
#define IMEI_LEN 15
#define MAC_LEN 6
#define SN_lEN 32

int ReverseFindStrW(const TCHAR * bRecieveBytes, int iBytesLength, const TCHAR* CharStrToFind,int CharStrLength)
{
	BOOL bResult = TRUE;

	if (0 >= CharStrLength) {
		return -1;
	}

	// end of the position ,the  CharStrLength can not be zero ,else int i = iBytesLength - CharStrLength will be error
	for (int i = iBytesLength - CharStrLength ; i>= 0; i-- ) {
		bResult = TRUE;
		for (int j = 0; j < CharStrLength; j++) {
			if (bRecieveBytes[i + j] != CharStrToFind[j] ) {
				bResult = FALSE;
				break;
			}
		}
		if ( bResult ) {
			return i;
		}
	}
	return -1;
}

int ReverseFindStrA(CHAR * bRecieveBytes,int iBytesLength,CHAR* CharStrToFind,int CharStrLength)
{
	BOOL bResult = TRUE;

	if (0 >= CharStrLength) {
		return -1;
	}
	for (int i = iBytesLength - CharStrLength ; i>= 0; i-- ) {
		bResult = TRUE;
		for (int j = 0; j < CharStrLength; j++) {
			if (bRecieveBytes[i + j] != CharStrToFind[j] ) {
				bResult = FALSE;
				break;
			}
		}
		if ( bResult ) {
			return i;
		}
	}
	return -1;
}

int FindStrA(int StartPos,char * bRecieveBytes,int iBytesLength,char* CharStrToFind,int CharStrLength)
{
	BOOL bResult = TRUE;

	if (0 >= CharStrLength) {
		return -1;
	}
	for (int i = StartPos; i< iBytesLength - CharStrLength + 1; i++ ) {
		bResult = TRUE;
		for (int j = 0; j < CharStrLength; j++) {
			if (bRecieveBytes[i + j] != CharStrToFind[j] ) {
				bResult = FALSE;
				break;
			}
		}
		if ( bResult ) {
			return i;
		}
	}
	return -1;
}

int FindStrW(const int StartPos,const TCHAR * TSourceStr,const int iBytesLength,const TCHAR* TStrToFind,const int CharStrLength)
{
	BOOL bResult = TRUE;

	if (0 >= CharStrLength) {
		return -1;
	}
	for (int i = StartPos; i< iBytesLength - CharStrLength + 1; i++ ) {
		bResult = TRUE;
		for (int j = 0; j < CharStrLength; j++) {
			if (TSourceStr[i + j] != TStrToFind[j] ) {
				bResult = FALSE;
				break;
			}
		}
		if ( bResult ) {
			return i;
		}
	}
	return -1;
}

int WCharToChar(CHAR *pszDst,int nDstSize,const WCHAR *pszSrc)
{
	CHAR *pszOut = NULL;
	int nOutSize;
	if (!pszSrc) {
		return 0;
	}
	if ((NULL == pszDst)&&(0 != nDstSize)) {
        return 0;
    } else if (pszDst&&(0 == nDstSize)) {
        return 0;
    }
    /* If the function succeeds and cbMultiByte is 0, the return value is the required size */
    nOutSize = WideCharToMultiByte(CP_ACP, 0,pszSrc, -1,NULL, 0/*cbMultiByte*/, NULL, NULL) /* + 1 */;
    if( 0 == nDstSize) {
        /* Calculate the requires size */
        return nOutSize;
    }
    if(nOutSize > nDstSize) {
        return 0;
    }
    nOutSize = WideCharToMultiByte(CP_ACP, 0,pszSrc,-1,pszDst,nDstSize,NULL,NULL);
    return nOutSize;
}

int CharToWChar(WCHAR *pszDst,int nDstSize,const CHAR *pszSrc)
{
    WCHAR *pszOut = NULL;
    int nOutSize;
	if (!pszSrc) {
		return 0;
	}
	if ((NULL == pszDst)&&(0 != nDstSize)) {
        return 0;
    } else if (pszDst&&(0 == nDstSize)) {
        return 0;
    }
    nOutSize = MultiByteToWideChar(CP_ACP, 0,pszSrc,-1,NULL,0);
    if(0 == nDstSize) {
        /*Calculate the requires size*/
        return nOutSize;
    }
    if(nOutSize > nDstSize) {
        return 0;
    }
    nOutSize = MultiByteToWideChar(CP_ACP, 0,pszSrc,-1,pszDst,nDstSize);
    return nOutSize;
}

int CCharToWChar(WCHAR *pszDst, int nDstSize, const CHAR *pszSrc, int nSrcSize)
{
    WCHAR *pszOut = NULL;
    int nOutSize;

	if (!pszSrc || 0 == nSrcSize) {
		return 0;
	}
	if ((NULL == pszDst) && (0 != nDstSize)) {
        return 0;
    } else if (pszDst && (0 == nDstSize)) {
        return 0;
    }
    nOutSize = MultiByteToWideChar(CP_ACP, 0, pszSrc, nSrcSize, NULL, 0);
    if(0 == nDstSize) {
        return nOutSize;
    }
    if(nOutSize > nDstSize) {
        return 0;
    }
    nOutSize = MultiByteToWideChar(CP_ACP, 0, pszSrc, nSrcSize, pszDst,nDstSize);
    return nOutSize;
}

int CWCharToChar(CHAR *pszDst,int nDstSize,const WCHAR *pszSrc, int nSrcSize)
{
	CHAR *pszOut = NULL;
	int nOutSize;

	if (!pszSrc || 0 == nSrcSize) {
		return 0;
	}
	if ((NULL == pszDst) && (0 != nDstSize)) {
        return 0;
    } else if (pszDst && (0 == nDstSize)) {
        return 0;
    }
    nOutSize = WideCharToMultiByte(CP_ACP, 0, pszSrc, nSrcSize, NULL, 0, NULL, NULL) /* + 1 */;
    if( 0 == nDstSize) {
        return nOutSize;
    }
    if (nOutSize > nDstSize) {
        return 0;
    }
    nOutSize = WideCharToMultiByte(CP_ACP, 0, pszSrc, nSrcSize, pszDst, nDstSize, NULL, NULL);
    return nOutSize;
}

std::wstring  MID(std::wstring str, const std::wstring prefix, const std::wstring subfix)
{
	size_t pos;

	pos = str.find(prefix);
	if(std::wstring::npos != pos)
		str.erase(0, pos + prefix.length());
	pos = str.rfind(subfix);
	if(std::wstring::npos != pos)
		str.erase(pos);
    return str;
}





/*by lanshh 2015-06-18*/
/**************************************************************
* CHAR:0--------9  A-------F    a---------f
* HEX :30      39  41     46   61--------66
*
***************************************************************/
#ifdef _UNICODE
    #define IsNumberString  IsNumberStringW
    #define IsHexString     IsHexStringW
#else
    #define IsNumberString  IsNumberStringA
    #define IsHexString     IsHexStringA
#endif

BOOL IsNumberStringW(const wchar_t *str)
{
    int     nIndex;
    int     nLen;

    if (NULL == str) {
        return FALSE;
    }
    nLen = _tcslen(str);
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = _tcsspn( str, L"0123456789");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}

BOOL IsNumberStringA(const char *str)
{
    int     nIndex;
    int     nLen;

    if (NULL == str) {
        return FALSE;
    }
    nLen    = strlen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = strspn( str, "0123456789" );
    return ( nLen == nIndex ) ? TRUE : FALSE;
}

BOOL IsHexStringW(const wchar_t *str)
{
    int         nIndex;
    int         nLen;

    if (NULL == str) {
        return FALSE;
    }    
    nLen    = _tcslen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = _tcsspn( str, L"0123456789ABCDEFabcdef");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}

BOOL IsHexStringA(const char *str)
{
    int         nIndex;
    int         nLen;
    if(NULL == str){
        return FALSE;
    }    
    nLen    = strlen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = strspn( str, "0123456789ABCDEFabcdef");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}

/* 1122 ---> 462 return 3
 * 
 */
DWORD intToHexLen(DWORD dwValue)
{
    DWORD       dwLen;

    dwLen       = 0;
    dwValue     /=16;
    while(dwValue){
        dwLen ++;
        dwValue /=16;
    }
    return dwLen;
}

/* 1122 ---> return 4
 *
 */
DWORD intToDecLen(DWORD dwValue)
{
    DWORD       dwLen;

    dwLen       = 0;
    dwValue     /=10;
    while(dwValue){
        dwLen ++;
        dwValue /=10;
    }
    return dwLen;
}

BOOL IntStrIncrease(TCHAR *strValue, int spn, TCHAR *strNEW, int len)
{
    TCHAR       szChar;
    TCHAR       newChar;
    TCHAR       *STR;
    BOOL        isINC;
    int         i;
    int         nLen;

    if (!IsNumberString(strValue) || spn > 10 || spn < -10) {
		return FALSE;
	}
    if (0 > spn) {
        spn = -spn;
        isINC = FALSE;
    } else if(0 < spn) {
        isINC = TRUE;
    } else {
        return FALSE;
    }
	nLen = _tcslen(strValue);
    if ( NULL != strNEW ) {
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW, TEXT("%s"), strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }

    if (isINC) {
    	for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            newChar = szChar + spn;
    		if (newChar <= _T('9') ) {
    			szChar = newChar;
                spn = 0;
    		} else if (newChar > _T('9')) {
    		    szChar  = newChar - 10;
                STR[i]  = szChar;
                spn = 1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    } else {
        for (i = nLen-1; i >= 0; i-- ) {
			szChar = STR[i];
			newChar = szChar - spn;
    		if (newChar >= _T('0')) {
    			szChar = newChar;
                spn = 0;
    		} else if ( newChar < _T('0') ){
    		    szChar = newChar + 10 ;
                STR[i] = szChar;
                spn =  1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    }
    return TRUE;
}

/**************************************************************
* CHAR:0--------9  A-------F    a---------f
* HEX :30      39  41     46   61--------66
*
***************************************************************/
/*
*for plus result calculating,the input must in range 0 ~ 31
*
*
*/
static BOOL HEXTOCHARP(int hex,int *out)
{
    if(hex < 10){                   /*0 ---- 9 */
        *out = hex + TEXT('0');
        return FALSE;
    }else if (hex < 16 ){           /*10----15*/
        *out = hex - 10 + TEXT('A');
        return FALSE;
    }else if (hex < 26){
        *out = hex - 16 + TEXT('0');/*16----25 carry out */
        return TRUE;
    }else if (hex < 32){
        *out = hex - 26 + TEXT('A');/*26----31 carry out */
        return TRUE;
    }
    return FALSE;
}

/*
*for minus result calculating,the input must in range -15 ~ +15
*
*
*/
static BOOL HEXTOCHARM(int hex,int *out)
{
    if(hex >= 0 ){
        if(hex < 10){                   /*0 ---- 9*/
            *out = hex + TEXT('0');
        }else if (hex < 16 ){           /*10----15*/
            *out = hex - 10 + TEXT('A');
        }
        return FALSE;   //hex > 0 ,just the result
    } else {
        if(hex < -6 ){  // -7 ---- -15 16 + hex = 9 8 7 6 5 4 3 2 1 0 
            *out = 16 + hex + TEXT('0');
        } else {        // -1 ---- -6  16 + hex = f e d c b a 
            *out = 1 + hex + TEXT('F');
        }
        return TRUE;    //hex < 0 ,need borrow 
    }
}
BOOL HexStrIncreaseA(char *strValue,int spn,char *strNEW,int len)
{
    int         szChar;
    char        *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if (!IsHexStringA(strValue)||spn > 16 ||spn < -16 ) {  //here must ensure input is hex string and the span is less than 16,
                                                           //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn) {
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = strlen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        sprintf(strNEW, "%s" ,strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= '0'&&szChar <= '9'){
                newChar = szChar -  '0' + spn;
            }else if (szChar >= 'A'&&szChar <= 'F'){
                newChar = szChar -  'A' + spn + 10;
            }else if (szChar >= 'a' &&szChar <= 'f' ){
                newChar = szChar - 'a' + spn + 10;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  = szChar;
                spn     = 1;
                continue;
            } 
            STR[i] =  szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= '0' &&szChar <= '9' ){
                newChar = szChar - '0' - spn;
            }else if (szChar >= 'A' &&szChar <= 'F' ){
                newChar = szChar - 'A' - spn + 10;
            }else if (szChar >= 'a' &&szChar <= 'f' ){
                newChar = szChar - 'a' - spn + 10;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  = szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = szChar;
            break;
    	}
    }
	return TRUE;
}
BOOL HexStrIncrease(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    int         szChar;
    TCHAR       *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if (!IsHexString(strValue)||spn > 16 ||spn < -16 ) {    //here must ensure input is hex string and the span is less than 16,
                                                            //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') + spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') + spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') + spn + 10;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') - spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') - spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') - spn + 10;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
    	}
    }
	return TRUE;
}
BOOL HexStrIncreaseSkipAlpha(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    int         szChar;
    TCHAR       *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if ( spn > 16 ||spn < -16 ) {  //here must ensure input is hex string and the span is less than 16,
                                                           //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') + spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') + spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') + spn + 10;
            } else {
                continue;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') - spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') - spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') - spn + 10;
            } else {
                continue;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
    	}
    }
	return TRUE;
}

BOOL IntStrIncreaseSkipAlpha(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    TCHAR       szChar;
    TCHAR       newChar;
    TCHAR       *STR;
    BOOL        isINC;
    int         i;
    int         nLen;
    if ( spn > 10 ||spn < -10) {
		return FALSE;
	}
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
	nLen    = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
    	for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            if(!isdigit(szChar)){
                continue;
            }
            newChar = szChar + spn;
    		if (newChar <=_T('9') ) {
    			szChar  = newChar;
                spn     =  0;
    		} else if (newChar > _T('9')) {
    		    szChar  = newChar - 10;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }else {
                continue;
            }
            STR[i] = szChar;
            break;
        }
    } else {
        for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            if(!isdigit(szChar)){
                continue;
            }
            newChar = szChar - spn;
    		if (newChar >=_T('0')) {
    			szChar  = newChar;
                spn     =  0;
    		} else if ( newChar < _T('0') ){
    		    szChar  = newChar + 10 ;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    }
    return TRUE;
}

BOOL TrimAndInc(TCHAR *str, const TCHAR *prefix, const TCHAR *subfix, int spn)
{
    int left, right, len;
	TCHAR *strTmp;

	left = FindStrW(0, str, wcslen(str), prefix, wcslen(prefix));
	right = ReverseFindStrW(str, wcslen(str), subfix, wcslen(subfix));

	if(left >= 0) /* find prefix */
		left += wcslen(prefix);
	else
		left = 0;

	if(right >= 0 ) /* find subfix */
		len = right - left;
	else
		len = wcslen(str) - left;
	if (len > 0 ) {
		strTmp = new TCHAR[len +1];
		memcpy(strTmp, &str[left], sizeof(TCHAR)*len);
		strTmp[len] = _T('\0');
		IntStrIncreaseSkipAlpha(strTmp, spn);
		memcpy(&str[left], strTmp, sizeof(TCHAR)*len);
		delete strTmp;
		return TRUE;
	}
	return FALSE;
}
BOOL TrimAndIncHex(TCHAR *str, const TCHAR *prefix, const TCHAR *subfix, int spn)
{
    int left, right, len;
	TCHAR *strTmp;

	left = FindStrW(0, str, wcslen(str), prefix, wcslen(prefix));
	right = ReverseFindStrW(str, wcslen(str), subfix, wcslen(subfix));

	if(left >= 0) /* find prefix */
		left += wcslen(prefix);
	else
		left = 0;

	if(right >= 0 ) /* find subfix */
		len = right - left;
	else
		len = wcslen(str) - left;
	if (len > 0 ) {
		strTmp = new TCHAR[len +1];
		memcpy(strTmp, &str[left], sizeof(TCHAR)*len);
		strTmp[len] = _T('\0');
		IntStrIncreaseSkipAlpha(strTmp, spn);
		memcpy(&str[left], strTmp, sizeof(TCHAR)*len);
		delete strTmp;
		return TRUE;
	}
	return FALSE;
}

bool check_sn(const TCHAR *str ,unsigned char  flag)
{
	size_t i;

	if(!str)
		return false;
	i = _tcslen(str);
    return (i > 0)&&(i <= SN_lEN);
}

bool check_mac (const TCHAR *str, unsigned char  flag)
{
    int nIndex;
    int len;

    if(NULL == str ) {
        return false;
    }
    len = _tcslen( str );
    if ( MAC_LEN*2 == len ) {
        nIndex = _tcsspn( str , TEXT("0123456789ABCDEFabcdef"));
        return ( len == nIndex ) ? TRUE : FALSE;
    }
    return false;

}

TCHAR calc_imei_sp(INT64 nImei)
{
    INT64 d;
    int nTime = 0;
    INT64 nCD = 0;  
    while (nImei != 0) {
        d = nImei % 10;
        if(nTime % 2 == 0) {
            d *= 2;
            if(d >= 10) {
				d = d % 10 + d / 10;
            }
        } 
        nCD += d;   
        nTime++;
        nImei /= 10;
    }  
    nCD %= 10;
    if(nCD != 0) {
        nCD = 10 - nCD;
    }  
    return (TCHAR)nCD;
}
/*flag 1--14   2---15    0:14 or 15
 *
 */
bool check_imei (const TCHAR *str, unsigned char  flag)
{
    __int64 int64Imei;
    size_t nLen;
    bool bLen1;
    bool bLen2;

    if (NULL == str) {
        return false;
    }
    if (0 == flag) {
        bLen1 = bLen2 = 1;
    } else {
        bLen1 = 0 != (flag&1);
        bLen2 = 0 != (flag&2);
    }
    nLen = _tcslen( str );
    if (bLen2&&(IMEI_LEN == nLen)) {
        if ( nLen == _tcsspn( str,TEXT("0123456789"))) {
            int64Imei = _ttoi64(str);
            int64Imei /= 10;
            return  str[IMEI_LEN -1] == calc_imei_sp(int64Imei) + TEXT('0');
        }
    } else if (bLen1&&(IMEI_LEN -1 == nLen)) {
        return nLen == _tcsspn( str,TEXT("0123456789"));
    }
    return FALSE;
}

int tchar_to_byte(const TCHAR ch)
{
	int result = 0;
    if(ch >= _T('0') && ch <= _T('9')) {
		result = (int)(ch - _T('0'));
    } else if(ch >= _T('a') && ch <= _T('z') ){
		result = (int)(ch - _T('a')) + 10;
    } else if(ch >= _T('A') && ch <= _T('Z')) {
        result = (int)(ch - _T('A')) + 10;
    } else{
        result = -1;
    }
    return result;
}

TCHAR byte_to_tchar(BYTE hex)
{
    if(hex < 10){
		return hex + _T('0');
    }else if (hex < 16 ){
        return hex - 10 + _T('A');
    }
	return _T(' ');
}

