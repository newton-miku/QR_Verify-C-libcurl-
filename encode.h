#pragma once
#include <stringapiset.h>
#include <atomic>
char* Utf8ToGb2312(const char* p)                                               //unicode转国标
{
	DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, p, -1, NULL, 0);
	char* psText;                                                         //存储文件名
	wchar_t* pwText = (wchar_t*)malloc(dwNum * sizeof(wchar_t));
	dwNum = MultiByteToWideChar(CP_UTF8, 0, p, -1, pwText, dwNum);
	dwNum = WideCharToMultiByte(CP_ACP, 0, pwText, -1, NULL, 0, NULL, NULL);
	psText = (char*)malloc(dwNum * sizeof(char));
	dwNum = WideCharToMultiByte(CP_ACP, 0, pwText, -1, psText, dwNum, NULL, NULL);
	free(pwText);
	return psText;
}