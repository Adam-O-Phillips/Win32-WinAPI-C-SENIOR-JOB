#include "CMateComm.h"
#include <windows.h>
#include <tchar.h>

#define MATE_COMM_PORT				0x5563
#define MATE_COMM_MAX_DELAY			2000 // 2 seconds

DWORD WINAPI InnerMateCommThreadProc(LPVOID lp);
void Log(const TCHAR* fmt, ...);

CMateComm::CMateComm(HWND hMsgHwnd, int nMateNo, ULONG ulMateBaseIP): 
	UDPSocket(INADDR_ANY, MATE_COMM_PORT), m_hMsgWnd(hMsgHwnd), m_hThread(NULL), m_bRunning(false), 
	m_nMateNo(nMateNo), m_ulMateBaseIP(ulMateBaseIP),
	m_nMSState(MS_UNKNOWN)
{
}

CMateComm::CMateComm(int nMateNo, ULONG ulMateBaseIP) : 
	UDPSocket(INADDR_ANY, MATE_COMM_PORT), m_hMsgWnd(NULL), m_hThread(NULL), m_bRunning(false), 
	m_nMateNo(nMateNo), m_ulMateBaseIP(ulMateBaseIP),
	m_nMSState(MS_UNKNOWN)
{
}

CMateComm::~CMateComm()
{
	m_bRunning = false;
	switch (::WaitForSingleObject(m_hThread, INFINITE))
	{
	case WAIT_OBJECT_0:
		break;
	default:
		break;
	}
}

int CMateComm::CreateMateCommThread()
{
	if (!IsValid())
		return SOCKET_ERROR;

	DWORD dwThreadId = 0;
	m_bRunning = true;
	m_hThread = CreateThread(NULL, 0, InnerMateCommThreadProc, this, 0, &dwThreadId);

	if (m_hThread == NULL)
		return -2;

	return 0;
}

const TCHAR* GetMSStateString(int nMSState)
{
	switch (nMSState)
	{
	case MS_UNKNOWN:
		return _T("unknown");
	case MS_SLAVE:
		return _T("slave");
	case MS_MASTER:
		return _T("master");
	}

	return _T("null");
}

void CMateComm::SetMSState(int nMSState)
{	
	Log(_T("(M/S) %s -> %s\n"), GetMSStateString(m_nMSState), GetMSStateString(nMSState));

	m_nMSState = nMSState;
}

ULONG CMateComm::GetMateIPAddr()
{
	return m_ulMateBaseIP + m_nMateNo;
}

USHORT CMateComm::GetMatePortNumber()
{
	return MATE_COMM_PORT;
}

int CMateComm::SendToMate(const char* pData, int len)
{
	int nlen = SendData(pData, len, GetMateIPAddr(), GetMatePortNumber());
	return nlen;
}

DWORD WINAPI InnerMateCommThreadProc(LPVOID lp)
{
	CMateComm* pComm = (CMateComm*)lp;

	ULONGLONG qwTickKeepAliveRx = GetTickCount64(), qwTickKeepAliveTx = 0;

	while (pComm->m_bRunning && pComm->IsValid())
	{
		HWND hMsgWnd = pComm->GetMsgHWnd();
		if (hMsgWnd == NULL)
			continue;

		static char aucData[0x10000];
		int nRecvLen = 0;

		ULONG ulSentIP = 0;
		USHORT usSentPort = 0;

		nRecvLen = pComm->RecvData(aucData, sizeof(aucData), MATE_COMM_MAX_DELAY, &ulSentIP, &usSentPort);
		char aucMateKeepAlive[3] = {'M', '#', pComm->GetMSState()};

		ULONGLONG qwTickNow = GetTickCount64();

		if (nRecvLen == 0)
		{
			if (qwTickNow >= qwTickKeepAliveRx + MATE_COMM_MAX_DELAY * 2)
			{
				if (pComm->m_nMSState == MS_SLAVE)
				{
					pComm->SetMSState(MS_MASTER);
					::SendMessage(hMsgWnd, WM_SLAVE_TO_MASTER, 0, 0);
					continue;
				}
			}
		}
		else if (nRecvLen == 3 && memcmp(aucMateKeepAlive, aucData, 2) == 0)
		{
			// keepalive message received
			qwTickKeepAliveRx = qwTickNow;
			if (pComm->m_nMSState == MS_UNKNOWN)
				pComm->SetMSState(MS_SLAVE);

			static int seq = 0;
			Log(_T("%d. keepalive (me=%s, mate=%s)\n"), seq ++, GetMSStateString(pComm->GetMSState()), GetMSStateString(aucData[2]));

			if (aucData[2] == MS_MASTER && pComm->GetMSState() == MS_MASTER)
			{
				::SendMessage(hMsgWnd, WM_MASTER_CONFLICT, 0, 0);
				Log(_T("master conflict\n"));
			}
			else if (aucData[2] == MS_SLAVE && pComm->GetMSState() == MS_SLAVE)
			{
				::SendMessage(hMsgWnd, WM_SLAVE_CONFLICT, 0, 0);
				Log(_T("slave conflict\n"));
			}
		}
		else
		{
			::SendMessage(hMsgWnd, WM_MATE_RECEIVE_MSG, (WPARAM)nRecvLen, (LPARAM)aucData);
			Log(_T("received message(%d) from mate\n"), nRecvLen);
		}

		if (qwTickNow >= qwTickKeepAliveTx + MATE_COMM_MAX_DELAY)
		{
			qwTickKeepAliveTx = qwTickNow;

			if (pComm->SendData(aucMateKeepAlive, 3, pComm->m_ulMateBaseIP + pComm->m_nMateNo, MATE_COMM_PORT) <= 0)
			{
				// int err = WSAGetLastError();
				break;
			}

			if (pComm->m_nMSState == MS_UNKNOWN)
				pComm->SetMSState(MS_SLAVE);
		}
	}

	pComm->m_bRunning = false;
	return 0;
}

void Log(const TCHAR* fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	TCHAR szMsg[0x400] = {0, };

	int nlen = _vsntprintf(szMsg, 0x400, fmt, va);

	va_end(va);

	OutputDebugString(szMsg);
}
