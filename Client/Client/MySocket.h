#pragma once
class CMySocket
{
public:
	CMySocket();
	~CMySocket();
	//SOCKET Run();
	SOCKET StartSocket(char address[60]);
	int MySend(SOCKET sock, const char* buff, int bytes);
	int MyRecv(SOCKET sock, char* buff, int bytes);
	int m_port;

private:
	SOCKET m_sock;

};

