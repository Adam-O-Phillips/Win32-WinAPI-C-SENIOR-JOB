// RunCloseWindow.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "RunCloseWindow.h"
#include <shellapi.h>
#include <vector>
#include <Psapi.h>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Log(const char* fmt, ...);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RUNCLOSEWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RUNCLOSEWINDOW));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUNCLOSEWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RUNCLOSEWINDOW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

HWND ghWnd = NULL;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   ghWnd = ghWnd;

   return TRUE;
}

typedef struct tagHWndProcessId
{
	HWND hWnd;
	DWORD dwProcessId;
} HWndProcessId;

void ListAllZOrderWindows(vector <HWndProcessId> &pidv)
{
	//Log("----------------------------\n");
	try
	{
		HWND hWnd = 0;

		for (hWnd = GetTopWindow(NULL); hWnd; hWnd = GetNextWindow(hWnd, GW_HWNDNEXT))
		{
			DWORD procId = 0;
			DWORD dwThrId = GetWindowThreadProcessId(hWnd, &procId);

			char szWindowText[0x100];
			GetWindowTextA(hWnd, szWindowText, 0x100);
			if (!GetParent(hWnd) && szWindowText[0])
			{
				HWndProcessId hpi = {hWnd, procId};
				pidv.push_back(hpi);

				//Log("[0x%X:%s]-%d\n", hWnd, szWindowText, procId);
			}
		}
	}
	catch (...)
	{
		return;
	}
	return;
}

void Log(const char* fmt, ...)
{
	char szBuf[0x400] = {0, };
	va_list va;
	va_start(va, fmt);
	vsnprintf(szBuf, 0x400, fmt, va);
	va_end(va);

	OutputDebugStringA(szBuf);

	FILE* fl = fopen("debug.txt", "a");
	if (fl)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		fprintf(fl, "[%04d/%02d/%02d %02d:%02d:%02d:%03d] %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szBuf);
		fclose(fl);
	}
}

//#define SHELL_EXECUTE_METHOD

#if defined SHELL_EXECUTE_METHOD
vector <SHELLEXECUTEINFO> gAppInfo;
#else
vector <PROCESS_INFORMATION> gAppInfo;
#endif

#include <TlHelp32.h>

vector<DWORD> pids_from_ppid(DWORD ppid) {

	Log("parent pid = %d\n", ppid);

	vector<DWORD> pids;
	pids.push_back(ppid);

	HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hp, &pe)) {
		do {
			if (pe.th32ParentProcessID == ppid) {
				pids.push_back(pe.th32ProcessID);
				Log("child pid = %d\n", pe.th32ProcessID);
			}
		} while (Process32Next(hp, &pe));
	}
	CloseHandle(hp);
	return pids;
}

bool CheckProcessLocation(LPCTSTR lpszPath, DWORD dwProcId)
{
	HANDLE processHandle = NULL;
	TCHAR filename[MAX_PATH] = {0, };

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (processHandle != NULL) {
		if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0) {
			
		}
		else {
			
		}
		CloseHandle(processHandle);
	}
	else {
		
	}

	return _tcscmp(lpszPath, filename) == 0;
}

void RunWindow()
{
	LPCTSTR lpApplication = _T("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
	LPCTSTR lpParameter = _T("--app=http://google.com -new-window");
	//LPCTSTR lpApplication = _T("C:\\Program Files\\PremiumSoft\\Navicat Premium 15\\navicat.exe");
	//LPCTSTR lpParameter = _T("");

#if defined SHELL_EXECUTE_METHOD

	SHELLEXECUTEINFO si = {0, };
	si.cbSize = sizeof(si);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.hwnd = NULL;
	si.lpFile = lpApplication;
	si.lpParameters = lpParameter;
	si.nShow = SW_NORMAL;

	vector<HWndProcessId> pidOld;
	ListAllZOrderWindows(pidOld);

	if (ShellExecuteEx(&si))
	{
		int count = 40;

		si.hwnd = NULL;

		while (count-- > 0)
		{
			Sleep(500);
			vector<HWndProcessId> pidNew;
			ListAllZOrderWindows(pidNew);

			int i;
			HWndProcessId pid = { 0, };
			for (i = 0; i < pidNew.size(); i++)
			{
				HWndProcessId& pid1 = pidNew[i];

				int j;
				for (j = 0; j < pidOld.size(); j ++)
				{
					HWndProcessId& pid2 = pidOld[j];

					if (memcmp(&pid1, &pid2, sizeof(HWndProcessId)) == 0)
						break;
				}

				if (j == pidOld.size() && CheckProcessLocation(lpApplication, pid1.dwProcessId))
				{
					pid = pid1;
					break;
				}
			}

			if (pid.hWnd)
			{
				si.hwnd = pid.hWnd;
				si.hProcess = (HANDLE)pid.dwProcessId;
				break;
			}
		}

		if (si.hwnd)
		{
			HWND hWnd = (HWND)si.hwnd;

			char szWindowText[0x100];
			GetWindowTextA(hWnd, szWindowText, 0x100);
			Log("New window 0x%X-(%s), Process Id (%d)\n", hWnd, szWindowText, (DWORD)si.hProcess);
		}
		else
			Log("Not found new window\n");

		gAppInfo.push_back(si);
	}

#else

	TCHAR lpCommandLine[0x200] = {0, };
	_stprintf(lpCommandLine, _T("\"%s\" %s"), lpApplication, lpParameter);

	STARTUPINFO si = { 0, };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;

	PROCESS_INFORMATION pi = {0, };

	vector<HWndProcessId> pidOld;
	ListAllZOrderWindows(pidOld);

	if (CreateProcess(NULL, lpCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		int count = 10;

		pi.hThread = NULL;

		//vector <DWORD> pids = pids_from_ppid(pi.dwProcessId);

		while (count-- > 0)
		{
			Sleep(100);
			
			vector<HWndProcessId> pidNew;
			ListAllZOrderWindows(pidNew);

			int i;
			HWndProcessId pid = {0, };
			for (i = 0; i < pidNew.size(); i++)
			{
				HWndProcessId& pid1 = pidNew[i];

				int j;
				for (j = 0; j < pidOld.size(); j ++)
				{	
					HWndProcessId& pid2 = pidOld[j];

					if (memcmp(&pid1, &pid2, sizeof(HWndProcessId)) == 0)
						break;
				}

				if (j == pidOld.size() && CheckProcessLocation(lpApplication, pid1.dwProcessId))
				{
					pid = pid1;
					break;
				}
			}

			if (pid.hWnd)
			{
				pi.hThread = pid.hWnd;
				pi.dwProcessId = pid.dwProcessId;
				break;
			}
		}

		if (pi.hThread)
		{
			HWND hWnd = (HWND)pi.hThread;

			char szWindowText[0x100];
			GetWindowTextA(hWnd, szWindowText, 0x100);
			Log("New window 0x%X-(%s), Process Id (%d)\n", hWnd, szWindowText, pi.dwProcessId);
		}
		else
			Log("Not found new window\n");

		gAppInfo.push_back(pi);
	}
#endif
}

void RunAll()
{
	int i;
	for (i = 0; i < 12; i ++)
		RunWindow();
}

void CloseAll()
{
	int i;
	for (i = 0; i < gAppInfo.size(); i++)
	{
#if defined SHELL_EXECUTE_METHOD
		SHELLEXECUTEINFO& pi = gAppInfo[i];
		HWND hWnd = pi.hwnd;
		DWORD dwProcId = (DWORD)pi.hProcess;
#else
		PROCESS_INFORMATION& pi = gAppInfo[i];
		HWND hWnd = (HWND)pi.hThread;
		DWORD dwProcId = (DWORD)pi.dwProcessId;
#endif
		bool bWindowExchanged = false;
		if (::IsWindow(hWnd))
		{
			int count = 10;
			while (::IsWindow(hWnd) && count-- > 0)
			{
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
				::Sleep(500);
			}
		}
		else
			bWindowExchanged = true;

		if (bWindowExchanged || ::IsWindow(hWnd))
		{
			//DWORD dwThrId = GetWindowThreadProcessId(pi.hwnd, &dwProcId);

			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcId);
			//HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, dwThrId);
			BOOL bRet = ::TerminateProcess(hProcess, 0);
			//BOOL bRet = ::TerminateThread(hThread, 0);
			CloseHandle(hProcess);
			//CloseHandle(hThread);
	//		BOOL bRet = ::PostMessage((HWND)pi.hwnd, WM_CLOSE, 0, 0);

			if (!bRet)
			{
				int err = GetLastError();
				TCHAR szBuf[0x100];
				_stprintf(szBuf, _T("err = %d\n"), err);
				OutputDebugString(szBuf);
			}
		}

		if (!::IsWindow(hWnd))
			Log("Closed window 0x%X\n", hWnd);
		else
			Log("Error closing window 0x%X\n", hWnd);
	}

	gAppInfo.clear();
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		CloseAll();
        PostQuitMessage(0);
        break;
	case WM_LBUTTONDOWN:
		//SetTimer(hWnd, 1, 1000, NULL);
		RunAll();
		break;
	case WM_RBUTTONDOWN:
		CloseAll();
		break;
	case WM_TIMER:
// 		{
// 			static int seq = 0;
// 			if ((++seq % 10) == 0)
// 			{
// 				Sleep(8000);
// 			}
// 
// 			ULONGLONG qwTick = GetTickCount64();
// 			TCHAR szMsg[0x100] = {0, };
// 			_stprintf(szMsg, _T("%d.%d\n"), (DWORD)(qwTick / 1000), (DWORD)(qwTick % 1000));
// 			OutputDebugString(szMsg);
// 		}
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
