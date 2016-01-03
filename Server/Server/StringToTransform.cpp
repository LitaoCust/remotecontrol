#include "stdafx.h"
#include "StringToTransform.h"


CStringToTransform::CStringToTransform()
{
}


CStringToTransform::~CStringToTransform()
{
}
wchar_t* CStringToTransform::CharToCString(char* temp)
{
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete[]pwText;
	}
	MultiByteToWideChar(CP_ACP, 0, temp, -1, pwText, dwNum);
	return pwText;
}

void CStringToTransform::CStringToChar(CString str, char* w)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), w, len, NULL, NULL);
	w[len] = '\0';
}

int  CStringToTransform::findStringPos(CString str)
{
	int pos;
	int i;
	for (i = str.GetLength(); i > 0; i--)
	{
		if (str[i] == '\\')
		{
			pos = i;
			break;
		}
	}
	
	if (i == 0)
		return 0;
	return pos;
	
}