#include "UDPSocket.h"

UDPSocket::UDPSocket(ULONG ulBindAddr, USHORT usBindPort): m_sock(INVALID_SOCKET)
{
	SOCKET s;
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		//printf("Could not create socket : %d", WSAGetLastError());
		return;
	}

	struct sockaddr_in server = {0, };

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(ulBindAddr);
	server.sin_port = htons(usBindPort);

	//Bind
	if (bind(s, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		//printf("Bind failed with error code : %d", WSAGetLastError());
		closesocket(s);
		return;
	}

	m_sock = s;
}

UDPSocket::~UDPSocket()
{
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
}

void UDPSocket::InitializeNetwork()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}

void UDPSocket::UnloadNetwork()
{
	WSACleanup();
}

int UDPSocket::SendData(const char* pData, int len, ULONG ulSendIP, USHORT usSendPort)
{
	if (!IsValid())
		return -3;

	if (pData == NULL || len <= 0)
		return -2;

	int nret = 0;
	struct sockaddr_in sockaddr = {0, };
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(ulSendIP);
	sockaddr.sin_port = htons(usSendPort);

	nret = sendto(m_sock, pData, len, 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

	return nret;
}

int UDPSocket::RecvData(char* pData, int maxlen, ULONG timeOut, ULONG* pulSentIP, USHORT* pusSentPort)
{
	if (!IsValid())
		return -2;

	SOCKET s = m_sock;
	FD_SET readSet;
	FD_ZERO(&readSet);
	FD_SET(s, &readSet);

	struct timeval tm = { timeOut / 1000, timeOut % 1000 };
	struct timeval* ptm = (timeOut == 0xFFFFFFFF) ? NULL : &tm;
	int nret = select(0, &readSet, NULL, NULL, ptm);

	if (nret == 0)
	{
		// timed out
		return 0;
	}
	else if (nret == SOCKET_ERROR)
	{
		//printf("select() returned with error %d\n", WSAGetLastError());
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return SOCKET_ERROR;
	}

	//clear the buffer by filling null, it might have previously received data
	memset(pData, 0, maxlen);

	int recv_len = 0;
	if (FD_ISSET(s, &readSet))
	{
		//try to receive some data, this is a blocking call
		struct sockaddr_in si_other = {0, };
		int slen = sizeof(si_other);

		if ((recv_len = recvfrom(s, pData, maxlen, 0, (struct sockaddr*) & si_other, &slen)) == SOCKET_ERROR)
		{
			//printf("recvfrom() failed with error code : %d", WSAGetLastError());
			return SOCKET_ERROR;
		}

		if (pulSentIP)
			* pulSentIP = ntohl(si_other.sin_addr.s_addr);

		if (pusSentPort)
			* pusSentPort = ntohs(si_other.sin_port);
	}

	return recv_len;
}
