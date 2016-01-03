#pragma once
class CStringToTransform
{
public:
	CStringToTransform();
	~CStringToTransform();
	wchar_t* CharToCString(char* temp); //Ìæ»»

	void CStringToChar(CString str, char* w);
	int  findStringPos(CString str);
};

