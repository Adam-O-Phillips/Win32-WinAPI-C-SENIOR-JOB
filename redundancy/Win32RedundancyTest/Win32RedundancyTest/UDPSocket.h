#pragma once

#include<winsock2.h>

class UDPSocket
{
public:
	UDPSocket(ULONG ulBindAddr, USHORT usBindPort);
	~UDPSocket();

	bool IsValid() { return m_sock != INVALID_SOCKET;  };

	int SendData(const char *pData, int len, ULONG ulSendIP, USHORT usSendPort);
	int RecvData(char *pData, int maxlen, ULONG timeOut, ULONG *pulSentIP, USHORT *pusSentPort);
	
public:
	static void InitializeNetwork();
	static void UnloadNetwork();

protected:
	SOCKET m_sock;

};

