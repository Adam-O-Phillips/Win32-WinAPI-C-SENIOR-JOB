#pragma once
#include "UDPSocket.h"

#define WM_MR_RECEIVE_MSG			(WM_USER + 0x4420) // WPARAM: message length, LPARAM: message pointer

class CMoveResizeThread :
	public UDPSocket
{
public:
	CMoveResizeThread();
	CMoveResizeThread(HWND hMsgHwnd);
	~CMoveResizeThread();

public:
	HWND GetMsgHWnd() { return m_hMsgWnd; }
	void SetMsgHWnd(HWND hMsgWnd) { m_hMsgWnd = hMsgWnd; }

	int CreateMRCommThread();

protected:
	HWND m_hMsgWnd;
	HANDLE m_hThread;

private:
	friend DWORD WINAPI InnerMRCommThreadProc(LPVOID lp);
	bool m_bRunning;

};

