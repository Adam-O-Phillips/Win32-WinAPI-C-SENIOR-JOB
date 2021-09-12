#pragma once
#include "UDPSocket.h"

#define WM_SLAVE_TO_MASTER			(WM_USER + 0x4411) // WPARAM:0, LPARAM:0
#define WM_MASTER_CONFLICT			(WM_USER + 0x4412) // WPARAM:0, LPARAM:0
#define WM_SLAVE_CONFLICT			(WM_USER + 0x4413) // WPARAM:0, LPARAM:0
#define WM_MATE_RECEIVE_MSG			(WM_USER + 0x4420) // WPARAM: message length, LPARAM: message pointer

enum
{
	MS_UNKNOWN,
	MS_SLAVE,
	MS_MASTER,
};

class CMateComm :
	public UDPSocket
{
public:
	CMateComm(int nMateNo, ULONG ulMateBaseIP);
	CMateComm(HWND hMsgHwnd, int nMateNo, ULONG ulMateBaseIP);
	~CMateComm();

public:
	HWND GetMsgHWnd() { return m_hMsgWnd; }
	void SetMsgHWnd(HWND hMsgWnd) { m_hMsgWnd = hMsgWnd; }

	void SetMSState(int nMSState);
	int GetMSState() { return m_nMSState; }

	ULONG GetMateIPAddr();
	USHORT GetMatePortNumber();

	int SendToMate(const char* pData, int len);

	int CreateMateCommThread();

protected:
	HWND m_hMsgWnd;
	HANDLE m_hThread;
	int m_nMateNo;
	ULONG m_ulMateBaseIP;
	int m_nMSState;

private:
	friend DWORD WINAPI InnerMateCommThreadProc(LPVOID lp);
	bool m_bRunning;

};

