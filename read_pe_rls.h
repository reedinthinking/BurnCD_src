#ifndef READ_PE_RLS
#define READ_PE_RLS

#define IDR_FILEINFO1                   102
#define IDR_ENCODEFILE1                 103
#include <string>

CString replace(CString oripath);
int splitString(CString str, char split, CStringArray& strArray);
void EnumNamesFunc(HMODULE hModule,LPCTSTR lpType,LPTSTR lpName, LONG lParam);
BOOL GetResourceFromFile(HMODULE hFile, LPCTSTR lpName, LPCTSTR lpType, LPBYTE lpOut);
DWORD GetResourceSizeFromFile(HMODULE hFile, LPCTSTR lpName, LPCTSTR lpType);
long GetMyFileSize(FILE* pfile);
void GetFileContent(FILE* pfile,char * OutContent,long file_len);
BOOL UpdateFileResource(LPCTSTR lpszFile, LPBYTE pIniBuf, LPCTSTR lpType, LPCTSTR lpName);
int hexCharToInt(char c);
char* hexstringToBytes(std::string s);
std::string bytestohexstring(char* bytes,int bytelength);

#endif

