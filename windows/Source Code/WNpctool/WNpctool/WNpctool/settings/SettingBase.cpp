#include "windows.h"
#include "commctrl.h"
#include "SettingBase.h"
static const unsigned char base64_enc_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const unsigned char base64_dec_map[128] =
{
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

/*
 * Encode a buffer into base64 format
 */
int base64_encode( unsigned char *dst, unsigned int *dlen,
                   const unsigned char *src, unsigned int slen )
{
    int             nret;
    unsigned int    i, n;
    int             C1, C2, C3;
    unsigned char   *p;

    nret = -1;

    if( slen == 0 )
        return nret;

    n = (slen << 3) / 6;

    switch( (slen << 3) - (n * 6) ) {
        case  2: n += 3; break;
        case  4: n += 2; break;
        default: break;
    }

    if(dlen) {
        if( *dlen < n + 1 ) {
            *dlen = n + 1;
            return nret;
        }
    }

    n = (slen / 3) * 3;

    for( i = 0, p = dst; i < n; i += 3 ) {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;
        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    if( i < slen ) {
        C1 = *src++;
        C2 = ((i + 1) < slen) ? *src++ : 0;
        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
        if( (i + 1) < slen )
             *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        else *p++ = '=';
        *p++ = '=';
    }
    nret = p - dst;
    if(dlen) {
        *dlen = nret;
    }
    return nret;
}

/*
 * Decode a base64-formatted buffer
 */
int base64_decode( unsigned char *dst, unsigned int *dlen,
                   const unsigned char *src, unsigned int slen )
{
    int             nret;
    unsigned int    i, n;
    unsigned int    j, x;
    unsigned char   *p;

    for( i = j = n = 0; i < slen; i++ ) {
        if( ( slen - i ) >= 2 &&
            src[i] == '\r' && src[i + 1] == '\n' )
            continue;
        if( src[i] == '\n' )
            continue;
        if( src[i] == '=' && ++j > 2 )
            return -1;
        if( src[i] > 127 || base64_dec_map[src[i]] == 127 )
            return  -1 ;
        if( base64_dec_map[src[i]] < 64 && j != 0 )
            return -1;
        n++;
    }
    if( n == 0 )
        return( 0 );

    n = ((n * 6) + 7) >> 3;

    if(dlen) {
        if( *dlen < n ) {
            *dlen = n;
            return  -1;
        }
    }

    for( j = 3, n = x = 0, p = dst; i > 0; i--, src++ ) {
        if( *src == '\r' || *src == '\n' )
            continue;

        j -= ( base64_dec_map[*src] == 64 );
        x  = (x << 6) | ( base64_dec_map[*src] & 0x3F );

        if( ++n == 4 )
        {
            n = 0;
            if( j > 0 ) *p++ = (unsigned char)( x >> 16 );
            if( j > 1 ) *p++ = (unsigned char)( x >>  8 );
            if( j > 2 ) *p++ = (unsigned char)( x       );
        }
    }

    nret = p - dst;
    if(dlen) {
        *dlen = nret;
    }
    return nret;
}
CIniSettingBase::~CIniSettingBase()
{
    if(pIniFile) {
        delete pIniFile;
        /* pIniFile = NULL; **/
    }
}
#ifdef __cplusplus
extern "C" {
#endif
#define AES_ALIGN_SIZE   16
#define AES_BUF_LEN      32
int aes_encrypt_api(const unsigned char *plaintext ,const int len,unsigned char *ciphertext);
int aes_decrypt_api(const unsigned char *ciphertext,const int len,unsigned char *plaintext);
static unsigned short chartohex(const wchar_t * str,int len)
{
    unsigned short  usRet   = 0;
    int             j;
    int             nlen;
    if(0 > len ) {
        nlen = wcslen(str);
    } else {
        nlen = len;
    }
    if( 0 == nlen ) return usRet;
    if (8 < nlen) {
        j = 8;
    }
    for(int i = 0 ; i < nlen ;i ++ ) {
        unsigned short val = 0;  
        if(str[i]>=TEXT('A')&&str[i]<=TEXT('F')) {
            val = (str[i]-TEXT('A')+10);
        } else if (str[i]>=TEXT('a')&&str[i]<=TEXT('f')) {
            val = (str[i]-TEXT('a')+10);
        } else if (str[i]>=TEXT('0')&&str[i]<=TEXT('9')) {
            val = (str[i]-TEXT('0'));
        }
        usRet = usRet * 16+ val;
    }
    return usRet;
}


#ifdef __cplusplus
}
#endif
std::wstring CIniSettingBase::EncryptPassWord(std::wstring input)
{	
#if 1
    return input;
#else 
    int len,alignlen;
    std::string     plaintexta;
    unsigned char 	*plaintext;
    unsigned char 	*ciphertext;
    std::wstring    ouput;
    wchar_t         szTemp[16];
    ouput.clear();
    len = input.length();
    if(0 >= len) return TEXT("");
    alignlen = (len/AES_ALIGN_SIZE)*AES_ALIGN_SIZE + ((len%AES_ALIGN_SIZE)?AES_ALIGN_SIZE:0);
    plaintext   = new unsigned char[alignlen];
    ciphertext  = new unsigned char[alignlen];
    if(NULL == plaintext||NULL == plaintext) goto encryptpassword_exit;
    memset(plaintext,0,alignlen);
    plaintexta = wstr2str(input);
    sprintf((char *)plaintext,plaintexta.c_str());
    aes_encrypt_api(plaintext,alignlen,ciphertext);
    for(int i = 0 ; i < alignlen ; i ++ ) {
        wsprintf(szTemp,TEXT("%02x"),ciphertext[i]);
        ouput += szTemp;
    }
encryptpassword_exit:
    if(plaintext ) delete plaintext;
    if(ciphertext) delete ciphertext;
    return ouput;
#endif
}
std::wstring CIniSettingBase::DecryptPassWord(std::wstring input)
{
#if 1
    return input;
#else 
    int len;
    const wchar_t   *p_buf;
    unsigned char 	*plaintext;
    unsigned char 	*ciphertext;
    std::wstring    ouput;
    ouput.clear();
    len             = input.length();
    if((len%AES_ALIGN_SIZE != 0)|| (0 >= len)) return TEXT("");
    len = len >> 1;
    plaintext   = new unsigned char[len ];
    ciphertext  = new unsigned char[len ];
    if(NULL == plaintext||NULL == plaintext) goto dncryptpassword_exit;
    p_buf = input.c_str();
    for(int i = 0;i < len;i++) {
        ciphertext[i] = chartohex(&p_buf[i*2],2);
    }
    aes_decrypt_api(ciphertext,len,plaintext);
    ouput = str2wstr(std::string((char*)plaintext));
dncryptpassword_exit:
    if(plaintext ) delete plaintext;
    if(ciphertext) delete ciphertext;
    return ouput;
#endif
}

bool CIniSettingBase::LoadToolSetting(std::wstring strConfig)
{
	int nValue;
	std::wstring strValue;
    szFileName  = strConfig;
    pIniFile    = new CIniFile;
    if(!pIniFile) return false;
    bool bRet = pIniFile->Load(szFileName, false);
    if (!bRet) {
        delete pIniFile;
        pIniFile = NULL;
        return false;
    }
	/********************** Language config **********************/
	strLanPath			= GetStr(TEXT("Language:LangPath"));
	nValue				= _wtoi(GetStr(TEXT("Selected")).c_str());
	nCurLan				= ((1 <= nValue)&&(2 >= nValue))?nValue:1;
	strCnFilename		= GetStr(TEXT("Lang1File"));
	strEnFilename		= GetStr(TEXT("Lang2File"));
	strCnFontName		= GetStr(TEXT("Lang1FontName"));
	strEnFontName		= GetStr(TEXT("Lang2FontName"));
	nValue				= _wtoi(GetStr(TEXT("Lang1FontSize")).c_str());
	nCnFontSize			= (0 < nValue)?nValue:1;
	nValue				= _wtoi(GetStr(TEXT("Lang2FontSize")).c_str());
	nEnFontSize			= (0 < nValue)?nValue:1;
	/********************** System config **********************/
	strLogPath          = GetStr(TEXT("System:LogPath"));
	bDebug              = 1 == _wtoi(GetStr(TEXT("Debug")).c_str());
	bReadInfo			= 1 == _wtoi(GetStr(TEXT("READ")).c_str());
	bAutoTest			= 1 == _wtoi(GetStr(TEXT("AUTO")).c_str());
	bReboot				= 1 == _wtoi(GetStr(TEXT("Reboot")).c_str());
	nLogLevel           = _wtoi(GetStr(TEXT("LogLevel")).c_str());
	strOemUnlock        = GetStr(TEXT("OEMUNLOCK"));
	/********************** DevSn **********************/
	devsn.bEnable		= _wtoi(GetStr(TEXT("DSWR")).c_str());
	devsn.strPrefix		= GetStr(TEXT("DSPF"));
	devsn.strSuffix		= GetStr(TEXT("DSSF"));

	nValue				= _wtoi(GetStr(TEXT("DSAI")).c_str());
	devsn.nAutoMode		= ((0 <= nValue)&&(3 >= nValue))?nValue:0;
	devsn.strStartSn	= GetStr(TEXT("DSSS"));
	devsn.strCurrentSn	= GetStr(TEXT("DSSC"));
	devsn.strEndSn		= GetStr(TEXT("DSSD"));
	nValue				= _wtoi(GetStr(TEXT("DSST")).c_str());
	devsn.nSnCount		= (0 <= nValue)?nValue:0;
	nValue				= _wtoi(GetStr(TEXT("DSSR")).c_str());
	devsn.nRemainCount	= (0 <= nValue)?nValue:0;
	nValue				= _wtoi(GetStr(TEXT("DSLEN")).c_str());
	nSnLen				= (0 <= nValue)?nValue:0;

	/********************** WifiMac **********************/
	WifiMac.bEnable			= _wtoi(GetStr(TEXT("WMWR")).c_str());
	nValue					= _wtoi(GetStr(TEXT("WMST")).c_str());
	WifiMac.nCount			= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("WMSR")).c_str());
	WifiMac.nRemainCount	= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("WMAI")).c_str());
	WifiMac.nAutoMode		= ((0 <= nValue)&&(3 >= nValue))?nValue:0;

	WifiMac.strStartMac		= GetStr(TEXT("WMSS"));
	WifiMac.strCurrentMac	= GetStr(TEXT("WMSC"));
	WifiMac.strEndMac		= GetStr(TEXT("WMSD"));
	nValue					= _wtoi(GetStr(TEXT("WMTP")).c_str());
	WifiMac.dwType			= (( nValue <= 0)||(nValue >= 5))?4:nValue;

	/*file*/
	wsprintf(confPath.filePath[FLAG_WIFIMAC],GetStr(TEXT("WMFN")).c_str());
	confPath.lFilePos[FLAG_WIFIMAC]	= _wtoi(GetStr(TEXT("WMFP")).c_str());
	confPath.dwLineCnt[FLAG_WIFIMAC]= _wtoi(GetStr(TEXT("WMLC")).c_str());
	confPath.dwLinePos[FLAG_WIFIMAC]= _wtoi(GetStr(TEXT("WMLP")).c_str());

	/********************** BtMac **********************/
	BtMac.bEnable			= _wtoi(GetStr(TEXT("BMWR")).c_str());
	nValue					= _wtoi(GetStr(TEXT("BMST")).c_str());
	BtMac.nCount			= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("BMSR")).c_str());
	BtMac.nRemainCount		= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("BMAI")).c_str());
	BtMac.nAutoMode			= ((0 <= nValue)&&(3 >= nValue))?nValue:0;

	BtMac.strStartMac		= GetStr(TEXT("BMSS"));
	BtMac.strCurrentMac		= GetStr(TEXT("BMSC"));
	BtMac.strEndMac			= GetStr(TEXT("BMSD"));
	nValue					= _wtoi(GetStr(TEXT("BMTP")).c_str());
	BtMac.dwType			= (( nValue <= 0)||(nValue >= 5))?4:nValue;

	/*file*/
	wsprintf(confPath.filePath[FLAG_BTMAC],GetStr(TEXT("BMFN")).c_str());
	confPath.lFilePos[FLAG_BTMAC]	= _wtoi(GetStr(TEXT("BMFP")).c_str());
	confPath.dwLineCnt[FLAG_BTMAC]= _wtoi(GetStr(TEXT("BMLC")).c_str());
	confPath.dwLinePos[FLAG_BTMAC]= _wtoi(GetStr(TEXT("BMLP")).c_str());	

	/********************** LanMac **********************/
	LanMac.bEnable			= _wtoi(GetStr(TEXT("LMWR")).c_str());
	nValue					= _wtoi(GetStr(TEXT("LMST")).c_str());
	LanMac.nCount			= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("LMSR")).c_str());
	LanMac.nRemainCount		= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("LMAI")).c_str());
	LanMac.nAutoMode			= ((0 <= nValue)&&(3 >= nValue))?nValue:0;

	LanMac.strStartMac		= GetStr(TEXT("LMSS"));
	LanMac.strCurrentMac	= GetStr(TEXT("LMSC"));
	LanMac.strEndMac		= GetStr(TEXT("LMSD"));
	nValue					= _wtoi(GetStr(TEXT("LMTP")).c_str());
	LanMac.dwType			= (( nValue <= 0)||(nValue >= 5))?4:nValue;

	/*file*/
	wsprintf(confPath.filePath[FLAG_LANMAC],GetStr(TEXT("LMFN")).c_str());
	confPath.lFilePos[FLAG_LANMAC]	= _wtoi(GetStr(TEXT("LMFP")).c_str());
	confPath.dwLineCnt[FLAG_LANMAC]	= _wtoi(GetStr(TEXT("LMLC")).c_str());
	confPath.dwLinePos[FLAG_LANMAC]	= _wtoi(GetStr(TEXT("LMLP")).c_str());	

	/********************** IMEI **********************/
	Imei.bEnable			= _wtoi(GetStr(TEXT("IMWR")).c_str());
	nValue					= _wtoi(GetStr(TEXT("IMST")).c_str());
	Imei.nCount1			= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("IMSR")).c_str());
	Imei.nRemainCount1		= (0 < nValue)?nValue:0;
	nValue					= _wtoi(GetStr(TEXT("IMAI")).c_str());
	Imei.nAutoMode			= ((0 <= nValue)&&(3 >= nValue))?nValue:0;

	Imei.strStartImei1		= GetStr(TEXT("IMSS"));
	Imei.strCurrentImei1	= GetStr(TEXT("IMSC"));
	Imei.strEndImei1		= GetStr(TEXT("IMSD"));
	nValue					= _wtoi(GetStr(TEXT("IMTP")).c_str());
	//Imei.dwType			= (( nValue <= 0)||(nValue >= 5))?4:nValue;

	/*file*/
	wsprintf(confPath.filePath[FLAG_IMEI1],GetStr(TEXT("IMFN")).c_str());
	confPath.lFilePos[FLAG_IMEI1]	= _wtoi(GetStr(TEXT("IMFP")).c_str());
	confPath.dwLineCnt[FLAG_IMEI1]	= _wtoi(GetStr(TEXT("IMLC")).c_str());
	confPath.dwLinePos[FLAG_IMEI1]	= _wtoi(GetStr(TEXT("IMLP")).c_str());	
    return true;

}
bool CIniSettingBase::SaveToolSetting(std::wstring strConfig)
{

    std::wstring checke = TEXT("1"),unckeck = TEXT("0");
    std::wstring szValue1,szValue2;
    TCHAR szTemp1[64];
    if(!pIniFile ) {
        pIniFile    = new CIniFile;
    }
    if(pIniFile) {
		//
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),nCurLan);
		SetStr( TEXT("Language:Selected")          , szTemp1);

        SetStr( TEXT("System:LogPath")  , strLogPath);
        SetStr( TEXT("Debug")			, bDebug   ?checke:unckeck);
        SetStr( TEXT("READ")    		, bReadInfo ?checke:unckeck);
		SetStr( TEXT("AUTO")    		, bAutoTest ?checke:unckeck);
		SetStr( TEXT("Reboot")    		, bReboot ?checke:unckeck);

        SetStr( TEXT("DSWR")    		, devsn.bEnable ?checke:unckeck);
        SetStr( TEXT("DSPF")        	, devsn.strPrefix);
        SetStr( TEXT("DSSF")        	, devsn.strSuffix);
        SetStr( TEXT("DSSS") 			, devsn.strStartSn);
		SetStr( TEXT("DSSC") 			, devsn.strCurrentSn);
		SetStr( TEXT("DSSD") 			, devsn.strEndSn);

        swprintf(szTemp1,nof(szTemp1),TEXT("%d"),devsn.nAutoMode);
        SetStr( TEXT("DSAI")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),devsn.nSnCount);
		SetStr( TEXT("DSST")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),devsn.nRemainCount);
		SetStr( TEXT("DSSR")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),nSnLen);
		SetStr( TEXT("DSLEN")          , szTemp1);
		/********************** WifiMac **********************/
		SetStr( TEXT("WMSS") 			, WifiMac.strStartMac);
		SetStr( TEXT("WMSC") 			, WifiMac.strCurrentMac);
		SetStr( TEXT("WMSD") 			, WifiMac.strEndMac);
		SetStr( TEXT("WMWR")    		, WifiMac.bEnable ?checke:unckeck);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),WifiMac.nCount);
		SetStr( TEXT("WMST")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),WifiMac.nRemainCount);
		SetStr( TEXT("WMSR")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),WifiMac.nAutoMode);
		SetStr( TEXT("WMAI")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),WifiMac.dwType);
		SetStr( TEXT("WMTP")          , szTemp1);
		/*file*/
		SetStr( TEXT("WMFN") 			, confPath.filePath[FLAG_WIFIMAC]);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.lFilePos[FLAG_WIFIMAC]);
		SetStr( TEXT("WMFP")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLineCnt[FLAG_WIFIMAC]);
		SetStr( TEXT("WMLC")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLinePos[FLAG_WIFIMAC]);
		SetStr( TEXT("WMLP")          , szTemp1);

		/********************** BtMac **********************/
		SetStr( TEXT("BMSS") 			, BtMac.strStartMac);
		SetStr( TEXT("BMSC") 			, BtMac.strCurrentMac);
		SetStr( TEXT("BMSD") 			, BtMac.strEndMac);
		SetStr( TEXT("BMWR")    		, BtMac.bEnable ?checke:unckeck);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),BtMac.nCount);
		SetStr( TEXT("BMST")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),BtMac.nRemainCount);
		SetStr( TEXT("BMSR")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),BtMac.nAutoMode);
		SetStr( TEXT("BMAI")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),BtMac.dwType);
		SetStr( TEXT("BMTP")          , szTemp1);
		/*file*/
		SetStr( TEXT("BMFN") 			, confPath.filePath[FLAG_BTMAC]);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.lFilePos[FLAG_BTMAC]);
		SetStr( TEXT("BMFP")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLineCnt[FLAG_BTMAC]);
		SetStr( TEXT("BMLC")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLinePos[FLAG_BTMAC]);
		SetStr( TEXT("BMLP")          , szTemp1);

		/********************** LanMac **********************/
		SetStr( TEXT("LMSS") 			, LanMac.strStartMac);
		SetStr( TEXT("LMSC") 			, LanMac.strCurrentMac);
		SetStr( TEXT("LMSD") 			, LanMac.strEndMac);
		SetStr( TEXT("LMWR")    		, LanMac.bEnable ?checke:unckeck);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),LanMac.nCount);
		SetStr( TEXT("LMST")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),LanMac.nRemainCount);
		SetStr( TEXT("LMSR")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),LanMac.nAutoMode);
		SetStr( TEXT("LMAI")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),LanMac.dwType);
		SetStr( TEXT("LMTP")          , szTemp1);
		/*file*/
		SetStr( TEXT("LMFN") 			, confPath.filePath[FLAG_LANMAC]);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.lFilePos[FLAG_LANMAC]);
		SetStr( TEXT("LMFP")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLineCnt[FLAG_LANMAC]);
		SetStr( TEXT("LMLC")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLinePos[FLAG_LANMAC]);
		SetStr( TEXT("LMLP")          , szTemp1);

		/********************** IMEI **********************/
		SetStr( TEXT("IMSS") 			, Imei.strStartImei1);
		SetStr( TEXT("IMSC") 			, Imei.strCurrentImei1);
		SetStr( TEXT("IMSD") 			, Imei.strEndImei1);
		SetStr( TEXT("IMWR")    		, Imei.bEnable ?checke:unckeck);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),Imei.nCount1);
		SetStr( TEXT("IMST")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),Imei.nRemainCount1);
		SetStr( TEXT("IMSR")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),Imei.nAutoMode);
		SetStr( TEXT("IMAI")          , szTemp1);
		//swprintf(szTemp1,nof(szTemp1),TEXT("%d"),Imei.dwType);
		//SetStr( TEXT("IMTP")          , szTemp1);
		/*file*/
		SetStr( TEXT("IMFN") 			, confPath.filePath[FLAG_IMEI1]);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.lFilePos[FLAG_IMEI1]);
		SetStr( TEXT("IMFP")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLineCnt[FLAG_IMEI1]);
		SetStr( TEXT("IMLC")          , szTemp1);
		swprintf(szTemp1,nof(szTemp1),TEXT("%d"),confPath.dwLinePos[FLAG_IMEI1]);
		SetStr( TEXT("IMLP")          , szTemp1);

		return pIniFile->Save(szFileName);
    }
    return false;

}
std::wstring CIniSettingBase::GetStr(const wchar_t *key)
{
#if 0
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return TEXT("");
    if(NULL == key ) return TEXT("");
    wchar_t * npos = wcschr(key,TEXT(':'));
    if(npos) {
    } else {
    }
    return TEXT("");
#else 
    return GetStr(std::wstring(key));
#endif
}

std::wstring CIniSettingBase::GetStr(std::wstring &key)
{
    CIniSection  *pSec;
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return TEXT("");
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pSec  = pIniFile->GetSection(szSection);
        SetCurSection(pSec);
        szKey = key.substr(npos +1);
    } else {
        pSec = GetCurSection();
        szKey = key;
    }
    if(NULL == pSec||key.empty())return TEXT("");
    return pSec->GetKeyValue(szKey);
}
bool CIniSettingBase::SetStr(const wchar_t *key,std::wstring &value)
{
    return SetStr(std::wstring(key),value);
}
bool CIniSettingBase::SetStr(const wchar_t *key,const wchar_t * value)
{
    return SetStr(std::wstring(key),std::wstring(value));
}
bool CIniSettingBase::SetStr(std::wstring &key,std::wstring &value)
{
    CIniSection  *pSec;
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return false;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pSec = pIniFile->AddSection(szSection);
        SetCurSection(pSec);
        szKey = key.substr(npos +1);
    } else {
        pSec = GetCurSection();
        szKey = key;
    }
    if(NULL == pSec||key.empty()) return false;
    pSec->SetKeyValue(szKey,value);
    return true;
}
void CIniSettingBase::ParseBools(std::wstring s, bool *pBool, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for( j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pBool[j++] = 1 == _wtoi(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pBool[j++] = 1 == _wtoi(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pBool[j++] = false;
        }
    }
    *pLen = j;
    return;
}
void CIniSettingBase::ParseIntegers(std::wstring s, double *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for( j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtof(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtof(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
    return;
}
void CIniSettingBase::ParseIntegers(std::wstring s, float *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtof(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtof(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}

void CIniSettingBase::ParseIntegers(std::wstring s, int *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtoi(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtoi(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}
void CIniSettingBase::ParseIp(std::wstring s, unsigned int *IPV4)
{

    unsigned char           IP;
    int                     j;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    if (s.empty()) return;
    posl    = 0;
    *IPV4   = 0;
    for(j = 0;j< 4 ;) {
        posr = s.find_first_of(TEXT(" ."),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs    = s.substr(posl,posr - posl );
            posl    = posr + 1;
            IP      = _wtoi(subs.c_str());
            *IPV4   = ((*IPV4)<<8)|IP;
            j++;
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs    = s.substr(posl);
            IP      = _wtoi(subs.c_str());
            *IPV4   = ((*IPV4)<<8)|IP;
            j++;
            break;
        } 
        
    }
    /*parseintegers_exit: **/
    return;


}
void CIniSettingBase::ParseStr(std::wstring s, std::wstring *pInt, int *pLen)
{
    size_t                  StrLen;
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    StrLen  = s.length();
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        if(posl > StrLen) return ; /*
                                     except out range,but posl always no greater StrLen
                                     posl = posr + 1;
                                     but posr will never equal to StrLen 
                                   **/
        posr = s.find_first_of(TEXT(","),posl); /*posl string posr **/
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = subs.c_str();
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl); /*If this is equal to the string length, 
                                     the function returns an empty string. **/
            pInt[j++] = subs;
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            subs.clear();
            posl = posr + 1;
            pInt[j++] = subs.c_str();
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}

bool CIniLocalLan::LoadToolSetting(std::wstring strConfig)
{
    szFileName  = strConfig;
    pIniFile    = new CIniFile;
    if(!pIniFile) return false;
    bool bRet = pIniFile->Load(szFileName, false);
    if (!bRet) {
        delete pIniFile;
        pIniFile = NULL;
        return false;
    }
    bLanLoadOK = true;
    return true;
}

bool CSettingBase::IsPathFileExists(const std::wstring strFilaName)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE          hFind;
    if(strFilaName.empty()) return false;
    hFind = FindFirstFile(strFilaName.c_str(), &FindFileData);
    if (INVALID_HANDLE_VALUE == hFind) {
        return false;
    }  else  {
        FindClose(hFind);
    }
    return true;
}
bool CSettingBase::CreateFolder(std::wstring strDir)
{
    bool         bSubCreate = false;
    std::wstring strSubDir;
    size_t       pos,start;
    start   = 0;
    /*remove the last backslash **/
    pos     = strDir.find_last_of (TEXT("\\/"),std::string::npos);
    if(strDir.size() == pos + 1) {
        strDir.erase(pos,std::string::npos);
    }
    pos     = strDir.find_first_of (TEXT("\\/"),0);
    while (std::string::npos != pos ) {
        strSubDir = strDir.substr (0,pos);
#if 1 
        if(!bSubCreate) {
            if(!IsPathFileExists(strSubDir.c_str())) {
                bSubCreate = true;
            }
        }
        if (bSubCreate) {
#else 
        if(!IsPathFileExists(strSubDir.c_str())) {
#endif
            if (!CreateDirectory(strSubDir.c_str(),NULL)) {
                return false;
            }
        }
        start   = pos+1;
        pos     = strDir.find_first_of(TEXT("\\/"),start);
    }
    if (!IsPathFileExists(strDir.c_str())) {
        if (!CreateDirectory(strDir.c_str(),NULL)) {
            return false;
        }
    }
    return true;
}
bool CIniLocalLan::SaveToolSetting(std::wstring strConfig)
{
    if(!strConfig.empty()) szFileName = strConfig;
    if(szFileName.empty()) return false;
    if(pIniFile) {
        return pIniFile->Save(szFileName);
    } else {
        size_t pos = szFileName.find_last_of(TEXT("\\/"));
        if( std::string::npos != pos ) {
            if(!CreateFolder(szFileName.substr(0,pos))) {
                return false;
            }
        }
        pIniFile = new CIniFile;
        if(pIniFile) return pIniFile->Save(szFileName);
    }
    return false;
}
bool CIniLocalLan::SetStr(const wchar_t *key,const wchar_t * value)
{
    return SetStr(std::wstring(key),std::wstring(value));
}
bool CIniLocalLan::SetStr(std::wstring &key,std::wstring &value)
{
    CIniSection  *pSec;
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return false;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection   = key.substr(0,npos);
        pSec        = pIniFile->AddSection(szSection);
        SetCurSection(pSec);
        szKey       = key.substr(npos +1);
    } else {
        pSec        = GetCurSection();
        szKey       = key;
    }
    if(NULL == pSec||key.empty()) return false;
    pSec->SetKeyValue(szKey,value);
    return true;
}
std::wstring CIniLocalLan::GetLanStr(std::wstring key)
{
    std::wstring szSection;
    std::wstring szKey;
    if(true != bLanLoadOK) return key;
    if(NULL == pIniFile) return TEXT("");
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pLanSec = pIniFile->GetSection(szSection);
        szKey   = key.substr(npos +1);
    } else {
        szKey = key;
    }
    if(NULL == pLanSec||key.empty())return TEXT("");
    return pLanSec->GetKeyValue(szKey);

}
std::wstring CIniLocalLan::GetStr(const wchar_t *key)
{
    return GetStr(std::wstring(key));
}
std::wstring CIniLocalLan::GetStr(std::wstring &key,std::wstring strDefault) 
{
    CIniSection  *pSec;
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return strDefault;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection   = key.substr(0,npos);
        pSec        = pIniFile->GetSection(szSection);
        SetCurSection(pSec);
        szKey       = key.substr(npos +1);
    } else {
        pSec        = GetCurSection();
        szKey       = key;
    }
    if(NULL == pSec||key.empty()) return strDefault;
    return pSec->GetKeyValue(szKey);

}
std::wstring CIniLocalLan::GetStr(std::wstring &key) 
{
    CIniSection  *pSec;
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return TEXT("");
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection   = key.substr(0,npos);
        pSec        = pIniFile->GetSection(szSection);
        SetCurSection(pSec);
        szKey       = key.substr(npos +1);
    } else {
        pSec        = GetCurSection();
        szKey       = key;
    }
    if(NULL == pSec||key.empty()) return TEXT("");
    if(pSec) return pSec->GetKeyValue(szKey);
    return TEXT("");
}

static BOOL CALLBACK SetStringProc(HWND hwnd,LPARAM lParam )
{
    return ((CIniLocalLan*)lParam)->SetStringProc((void *)hwnd,(void *)lParam);
}
static BOOL CALLBACK GetStringProc(HWND hwnd,LPARAM lParam )
{
    return ((CIniLocalLan*)lParam)->GetStringProc((void *)hwnd,(void *)lParam);
}
void CIniLocalLan::TreeMenu(void * pParam,std::wstring strMainKeyPart)
{
    HMENU   pMenu = (HMENU)pParam;
    TCHAR   strKey[260];
    if(NULL == pMenu) return ;
    if(true != bLanLoadOK) return ;
    for (int i=0;i < GetMenuItemCount(pMenu);i++) {
        swprintf(strKey,nof(strKey),TEXT("%s_%d"),strMainKeyPart.c_str(),i);
        int id = GetMenuItemID(pMenu,i);
        if (0 == id) { 
            /*If nPos corresponds to a SEPARATOR menu item, the return value is 0. **/
        } else if (-1 == id) { 
            /*If the specified item is a pop-up menu (as opposed to an item within the pop-up menu), the return value is �C1 **/
            ModifyMenu(pMenu,i, MF_BYPOSITION, i, GetStr(strKey).c_str());
        	TreeMenu(GetSubMenu(pMenu,i),strKey);
        } else {
            ModifyMenu(pMenu,id, MF_BYCOMMAND, id, GetStr(strKey).c_str());
        }
    }
}
bool CIniLocalLan::TreeControls(void * pParam,BOOL bSvae,int DlgId,bool bVer)
{
    HWND            hWnd =  (HWND)pParam;
    BOOL            bReturn;
    TCHAR           strSection[260] = {0},strValue[260] = {0};
    long            resID = DlgId;
    swprintf(strSection,nof(strSection),TEXT("DIALOG_%d:DIALOG_TITLE"),DlgId);
    if(bSvae) {
        GetWindowText(hWnd,strValue,260);
        SetStr(strSection,strValue);
        bReturn = EnumChildWindows(hWnd,(WNDENUMPROC)::GetStringProc,(LPARAM)this);
        SaveToolSetting(TEXT(""));
    } else {
        if(true != bLanLoadOK) return false;
        std::wstring  strTitle = GetStr( strSection);
        if(bVer) {
            strTitle = GetLanStr(TEXT("LANG:IDS_TEXT_APPNAME")) + TEXT(APP_VERSION);
        }
        SetWindowText(hWnd,strTitle.c_str());
        bReturn = EnumChildWindows(hWnd,(WNDENUMPROC)::SetStringProc,(LPARAM)this);
    }
    if (!bReturn) {
        return false;
    }
    return true;
}
bool CIniLocalLan::GetStringProc(void * pParam,void* lParam )
{
    HWND    hwnd    = (HWND)pParam;
    int     resID   = GetWindowLong(hwnd,GWL_ID);
    if (resID==0) {
        return TRUE;
    }
    TCHAR strKey[256],strValue[256],szClassName[256] = TEXT("\0");
    if (GetClassName(hwnd,szClassName,256)==0) {
        return false;
    }
    if (_wcsicmp(szClassName,TEXT("Button"))==0) {
        swprintf(strKey,nof(strKey),TEXT("BUTTON_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("Static"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LABEL_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("SysTreeView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TREEVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysListView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LISTVIEW_%d"),resID);
        GetListViewString(hwnd,strKey);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysTabControl32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TABCTRL_%d"),resID);
        return true;
    } else {
        return true;
    }
    GetWindowText(hwnd,strValue,nof(strValue));
    SetStr(strKey,strValue);
    return true;
}
void CIniLocalLan::GetListViewString(HWND pWnd,TCHAR *strKey)
{
    HWND    hListView = pWnd;
    TCHAR   strColumnKey[MAX_PATH];
	HWND    hListViewHeader = ListView_GetHeader(hListView);
    int     iHeadCount = 0;
    if(hListViewHeader) {
        iHeadCount = Header_GetItemCount(hListViewHeader);
    }
    LVCOLUMN    lvcol;
    TCHAR       tzTextBuf[MAX_PATH];
    lvcol.mask          = LVCF_TEXT;
    lvcol.pszText       = tzTextBuf;
    lvcol.cchTextMax    = MAX_PATH;
    for (int i = 0;i < iHeadCount;i++) {
        if(ListView_GetColumn(hListView,i,&lvcol)){
            swprintf(strColumnKey,nof(strColumnKey),TEXT("%s_Header_%d"),strKey,i);
            SetStr(strColumnKey,tzTextBuf);
        }
    }
}
void CIniLocalLan::SetListViewString(HWND pWnd,TCHAR *strKey)
{
    HWND    hListView = pWnd;
    TCHAR   strColumnKey[MAX_PATH];
	HWND    hListViewHeader = ListView_GetHeader(hListView);
    int     iHeadCount = 0;
    if(hListViewHeader) {
        iHeadCount = Header_GetItemCount(hListViewHeader);
    }
    LVCOLUMN    lvcol;
    TCHAR       tzTextBuf[MAX_PATH];
    lvcol.mask          = LVCF_TEXT;
    lvcol.pszText       = tzTextBuf;
    lvcol.cchTextMax    = MAX_PATH;
	for (int i = 0;i < iHeadCount;i++) {
        ListView_GetColumn(hListView,i,&lvcol);
		swprintf(strColumnKey,nof(strColumnKey),TEXT("%s_Header_%d"),strKey,i);
        swprintf(tzTextBuf   ,nof(tzTextBuf)   ,TEXT("%s")          ,GetStr(strColumnKey).c_str());
        ListView_SetColumn(hListView,i,&lvcol);
    }
}
bool CIniLocalLan::SetStringProc(void * pParam,void* lParam )
{
    HWND hwnd = (HWND)pParam;
    int resID = GetWindowLong(hwnd,GWL_ID);
    if (0 == resID) {
        return TRUE;
    }
    TCHAR strKey[256];
    TCHAR szClassName[256] = {0};
    if (GetClassName(hwnd,szClassName,256)==0) {
        return false;
    }
    if (_wcsicmp(szClassName,TEXT("Button"))==0) {
        swprintf(strKey,nof(strKey),TEXT("BUTTON_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("Static"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LABEL_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("SysTreeView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TREEVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysListView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LISTVIEW_%d"),resID);
        SetListViewString(hwnd,strKey);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysTabControl32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TABCTRL_%d"),resID);
        return true;
    } else {
        return true;
    }
    SetWindowText(hwnd,GetStr(strKey).c_str());
    return true;
}

