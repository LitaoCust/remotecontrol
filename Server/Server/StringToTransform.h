#pragma once
class CStringToTransform
{
public:
	CStringToTransform();
	~CStringToTransform();
	wchar_t* CharToCString(char* temp); //�滻

	void CStringToChar(CString str, char* w);
	int  findStringPos(CString str);
};

