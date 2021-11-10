#include "CMoveResizeThread.h"
#include <windows.h>
#include <tchar.h>

#define MR_COMM_PORT				27123

void Log(const TCHAR* fmt, ...);

DWORD WINAPI InnerMRCommThreadProc(LPVOID lp);
void Log(const TCHAR* fmt, ...);

CMoveResizeThread::CMoveResizeThread(HWND hMsgHwnd): 
	UDPSocket(INADDR_ANY, MR_COMM_PORT), m_hMsgWnd(hMsgHwnd), m_hThread(NULL), m_bRunning(false)
{
}

CMoveResizeThread::CMoveResizeThread() : 
	UDPSocket(INADDR_ANY, MR_COMM_PORT), m_hMsgWnd(NULL), m_hThread(NULL), m_bRunning(false)
{
}

CMoveResizeThread::~CMoveResizeThread()
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

int CMoveResizeThread::CreateMRCommThread()
{
	if (!IsValid())
		return SOCKET_ERROR;

	DWORD dwThreadId = 0;
	m_bRunning = true;
	m_hThread = CreateThread(NULL, 0, InnerMRCommThreadProc, this, 0, &dwThreadId);

	if (m_hThread == NULL)
		return -2;

	return 0;
}

DWORD WINAPI InnerMRCommThreadProc(LPVOID lp)
{
	CMoveResizeThread* pComm = (CMoveResizeThread*)lp;

	ULONGLONG qwTickKeepAliveRx = GetTickCount64(), qwTickKeepAliveTx = 0;

	while (pComm->m_bRunning && pComm->IsValid())
	{
		HWND hMsgWnd = pComm->GetMsgHWnd();
		if (hMsgWnd == NULL)
			continue;

		static char aucData[0x1000];
		int nRecvLen = 0;

		ULONG ulSentIP = 0;
		USHORT usSentPort = 0;

		nRecvLen = pComm->RecvData(aucData, sizeof(aucData), 1000, &ulSentIP, &usSentPort);
		
		if (nRecvLen > 0)
		{
			::SendMessage(hMsgWnd, WM_MR_RECEIVE_MSG, (WPARAM)nRecvLen, (LPARAM)aucData);
			//Log(_T("received message(%d) from mate\n"), nRecvLen);
		}
		else if (nRecvLen < 0)
		{
			Log(_T("socket error %d\n"), WSAGetLastError());
		}
	}

	pComm->m_bRunning = false;
	return 0;
}
