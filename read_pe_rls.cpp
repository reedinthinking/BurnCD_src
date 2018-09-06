#include "stdafx.h"
#include "resource.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include "read_pe_rls.h"

using namespace std;

CString replace(CString oripath){
	// pdf doc file to buffer
	FILE * pfile = nullptr;

	//获取exe所在目录的路径
	CString rootpathText;
	TCHAR exetppath[MAX_PATH]={0};
	::GetModuleFileName(NULL,exetppath,MAX_PATH);
	rootpathText=exetppath;
	//wchar_t* ff = L"\\";
	rootpathText=rootpathText.Left(rootpathText.ReverseFind('\\'));
	rootpathText += _T("\\");
		
	pfile = _tfopen(oripath, _T("rb"));//_tfopen()：读取CString类型的路径文件
	long file_len=0;
	file_len=GetMyFileSize(pfile);
	char * buffer = nullptr;
	buffer = (char*)malloc(file_len*4 + 1);
	GetFileContent(pfile,buffer,file_len);
	fclose(pfile);

	// lpType: "FILEINFO" "ENCODEFILE" lpName: MAKEINTRESOURCE(IDR_FILEINFO1)  MAKEINTRESOURCE(IDR_ENCODEFILE1)
	LPBYTE lpOut;
	LPCTSTR lpType=L"ENCODEFILE";
	LPCTSTR lpName=MAKEINTRESOURCE(IDR_ENCODEFILE1);
	Sleep(2000);
	CStringArray str;
	int nSize = splitString(oripath,'\\',str); //splitString:用于实现cstring字符串的分隔
	CString rpname = str.GetAt(nSize-1);//获取文件名

	int nPos= rpname.ReverseFind('.'); 
	CString tmp_old_rpname = rpname.Left(nPos);//去掉文件的后缀名

	CString rppathtmp = rootpathText+_T("result\\");
	CString pathFile=rppathtmp+tmp_old_rpname+_T(".exe");
	CopyFile(rootpathText+L"source\\read_source.exe", pathFile, FALSE);
	CString lpszFile=pathFile;
	DWORD Company_Info_ResourceSize=GetResourceSizeFromFile(NULL,MAKEINTRESOURCE(IDR_COMPANYINFO1),L"COMPANYINFO");
	if(Company_Info_ResourceSize<=0){
		return L"";
	}
	lpOut=new BYTE[Company_Info_ResourceSize];
	GetResourceFromFile(NULL,MAKEINTRESOURCE(IDR_COMPANYINFO1),L"COMPANYINFO",lpOut);

	//------------获取当前时间----------------start
	time_t t = time(0); 
    char tmp[40]; 
    strftime(tmp,sizeof(tmp),"%Y-%m-%d %H:%M:%S",localtime(&t)); 
    puts(tmp); 
	BYTE* dateByte = (BYTE*)(tmp);//将char类型转换为byte类型
	//------------获取当前时间----------------end
	
	/*BYTE* cstbyte;
	cstbyte = (BYTE*)rpname.GetBuffer(rpname.GetLength());*/

	//------------将文件名转换为byte类型----------------start
	ULONG rpnmelen = rpname.GetLength()*2;//因为转换的时候两个字符间会有一个00位，故需长度X2
	byte* testbuf = new byte[rpnmelen];
	memcpy(testbuf,rpname.GetBuffer(rpname.GetLength()),rpname.GetLength()*2); 
	//------------将文件名转换为byte类型----------------end

	ULONG ResourceSize = Company_Info_ResourceSize+22+rpname.GetLength();//计算内存空间的总长度，公司信息长度为Company_Info，时间长度为20，两个分隔符“|”长度为2，文件名长度为rpname.GetLength()
	LPBYTE new_buffer;
	new_buffer=new BYTE[ResourceSize];
	byte flag = (byte)'|';
	for(int i=0;i<Company_Info_ResourceSize;i++){
		new_buffer[i] = lpOut[i];
	}//添加公司信息
	new_buffer[Company_Info_ResourceSize] = flag;//添加分隔符|
	for(int i=0;i<19;i++){
		new_buffer[i+Company_Info_ResourceSize+1] = dateByte[i];
	}//添加时间信息
	new_buffer[Company_Info_ResourceSize+20] = flag;//添加分隔符
	for(int i=0;i<rpname.GetLength();i++){
		new_buffer[i+Company_Info_ResourceSize+21] = testbuf[i*2];
	}//添加文件名信息
	new_buffer[Company_Info_ResourceSize+21+rpname.GetLength()] = (byte)'\0';//添加终止符
	

	//byte* testbufddd = (byte*)malloc(rpname.GetLength()*2 + 1);;
	//memcpy(testbufddd,rpname.GetBuffer(rpname.GetLength()),rpname.GetLength()*2);
	BOOL update_result_comp=UpdateFileResource(lpszFile,(LPBYTE)new_buffer,L"FILEINFO",MAKEINTRESOURCE(IDR_FILEINFO1));
	if(update_result_comp==FALSE){
		return L"";
	}
	BOOL update_result_file=UpdateFileResource(lpszFile,(LPBYTE)buffer,lpType,lpName);
	if(update_result_file==FALSE){
		return L"";
	}

	//---------------将基本信息写入日志文件burn_opt.log------------------start

	

	//log write
	FILE *ptlog = NULL;
	FILE *pterror = NULL;
	ptlog = _tfopen(rootpathText+L"burn_opt.log", L"a");
	pterror = _tfopen(rootpathText+L"errormessage.log", L"a");
	char* logstr = (char*)new_buffer;
	char* logtpcominfo = strtok(logstr,"|");
	char* logtptime = strtok(NULL,"|");
	char* logtpfile = strtok(NULL,"|");
	fprintf(ptlog, "%s", logtptime);
	fprintf(ptlog, "  ---   ");
	fprintf(ptlog, "%s", logtpcominfo);
	fprintf(ptlog, "  burn the pdf file:   ");
	fprintf(ptlog, "%s", logtpfile);
	fprintf(ptlog, "\n---------------\n");

	fprintf(pterror, "%s", logtptime);
	fprintf(pterror, "  ---   ");
	fprintf(pterror, "%s", logtpcominfo);
	fprintf(pterror, "  burn the pdf file:   ");
	fprintf(pterror, "%s", logtpfile);
	fprintf(pterror, "\n---------------\n");

	/*CString strlog = _T("http://www.baidu.com");
	ULONG strlen = strlog.GetLength();
	char *logchar = new char[strlen];
	WideCharToMultiByte(CP_ACP, 0, strlog.GetBuffer(0), strlen, logchar, strlen, 0, 0);
	logchar[strlen] = '\0';
	strlog.ReleaseBuffer();*/
	
	
	//char* exepath = _pgmptr;
	
	
	//fprintf(pterror, "%s", exepath);
	
	
	fclose(ptlog);
	fclose(pterror);
	
	//TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	//memset(exeFullPath,0,MAX_PATH);  
 // 
	//GetModuleFileName(NULL,exeFullPath,MAX_PATH);  
	////char *p = strrchr(exeFullPath, '\\'); 
	
	
	
	


	//---------------将基本信息写入日志文件burn_opt.log------------------end

	return lpszFile;
}
int splitString(CString str, char split, CStringArray& strArray) //CString字符串分隔
{ 
	strArray.RemoveAll(); 
	CString strTemp = str; //此赋值不能少 
	int nIndex = 0; // 
	while(1) 
	{ 
		nIndex = strTemp.Find(split); 
		if(nIndex >= 0) 
		{ 
			strArray.Add(strTemp.Left(nIndex)); 
			strTemp = strTemp.Right(strTemp.GetLength()-nIndex-1); 
		} 
		else break; 
	} 
	strArray.Add(strTemp); 
	return strArray.GetSize(); 
} 
int hexCharToInt(char c)
{
	if (c >= '0' && c <= '9') return (c - '0');
	if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
	if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
	return 0;
}  
char* hexstringToBytes(string s)  
{           
	int sz = s.length();
	char *ret = new char[sz/2];
	for (int i=0 ; i <sz ; i+=2) {  
		ret[i/2] = (char) ((hexCharToInt(s.at(i)) << 4)  
							| hexCharToInt(s.at(i+1)));
		}  
	return ret;
}  
  
std::string bytestohexstring(char* bytes,int bytelength)
{
	std::string str;
	str = "";
	std::string str2;
	str2 = "0123456789abcdef";
	for (int i=0;i<bytelength;i++) {  
		int b;
		b = 0x0f&(bytes[i]>>4);
		char s1 = str2.at(b);
		str.append(1,str2.at(b));        
		b = 0x0f & bytes[i];
		str.append(1,str2.at(b));
		char s2 = str2.at(b);
   }  
   return str;
}

BOOL UpdateFileResource(LPCTSTR lpszFile, LPBYTE pIniBuf, LPCTSTR lpType, LPCTSTR lpName){
	BOOL bRet = FALSE;
	if( NULL == lpszFile || !PathFileExists(lpszFile) || NULL == pIniBuf || NULL == lpType || NULL == lpName )  
        return bRet;   
    DWORD dwBufSize = strlen( (char*)pIniBuf );  

	HANDLE hResource = BeginUpdateResource(lpszFile, FALSE);  
	if( NULL != hResource ){
		if( FALSE != UpdateResource( hResource, lpType, lpName, 0, (LPVOID)pIniBuf, dwBufSize)){ 
                    if (EndUpdateResource(hResource, FALSE) != FALSE) {
                        bRet = TRUE;  
						return bRet;
					}
		}
	}
	return bRet;
}

void GetFileContent(FILE* pfile,char * OutContent,long file_len){
	rewind(pfile);
	if (NULL == OutContent){
		return;
	}
	char* tmp_file_content= nullptr;
	tmp_file_content = (char*)malloc(file_len + 1);
	fread(tmp_file_content, sizeof(char), file_len, pfile);

	memset(OutContent, 0, file_len*4 + 1);
	std::string tmp_str_content;
	tmp_str_content=bytestohexstring(tmp_file_content,file_len);
	const char* tmp_outcontent;
	tmp_outcontent=tmp_str_content.data();
	strcpy(OutContent,tmp_outcontent);
	return;
}
long GetMyFileSize(FILE* pfile){
	long lsize = 0;
	char * buffer = nullptr;
	if (NULL == pfile){
		return lsize;
	}
	fseek(pfile, 0, SEEK_END);
	lsize = ftell(pfile);
	rewind(pfile);
	return lsize;
}

DWORD GetResourceSizeFromFile(HMODULE hFile, LPCTSTR lpName, LPCTSTR lpType){
    HRSRC hRsrc = FindResource( hFile, lpName, lpType );  
    if( NULL == hRsrc )  
        return 0;  
    HGLOBAL hGlobal = LoadResource( hFile, hRsrc );  
    if( NULL == hGlobal )  
        return 0;  
    LPBYTE lpBuffer = (LPBYTE)LockResource( hGlobal );  
    if( NULL == lpBuffer)  
        return 0;  
    DWORD dwSize = SizeofResource( hFile, hRsrc );      
    return dwSize;  
}

BOOL GetResourceFromFile(HMODULE hFile, LPCTSTR lpName, LPCTSTR lpType, LPBYTE lpOut) {
	BOOL bRet = FALSE;  
    HRSRC hRsrc = FindResource( hFile, lpName, lpType );  
    if( NULL == hRsrc )  
        return bRet;  
    HGLOBAL hGlobal = LoadResource( hFile, hRsrc );  
    if( NULL == hGlobal )  
        return bRet;  
    LPBYTE lpBuffer = (LPBYTE)LockResource( hGlobal );  
    if( NULL == lpBuffer)  
        return bRet;  
    DWORD dwSize = SizeofResource( hFile, hRsrc );  
    memcpy( lpOut, lpBuffer, dwSize );  
    bRet = TRUE;    
    return bRet;  
}  

void EnumNamesFunc(HMODULE hModule,LPCTSTR lpType,LPTSTR lpName, LONG lParam){
	printf("\tName: %ls\r\n", lpName);  
	int bb;
	bb=10;
}
