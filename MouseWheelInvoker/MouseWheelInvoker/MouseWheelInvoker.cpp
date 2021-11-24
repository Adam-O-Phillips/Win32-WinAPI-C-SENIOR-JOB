// MouseWheelInvoker.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MouseWheelInvoker.h"
#include <stdio.h>

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
INT_PTR CALLBACK KontrolSplashProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool gbRunning = true;
bool SetCaptureMode();
void ReleaseCaptureMode();

HINSTANCE ghInst = NULL;
HWND ghMainWnd = NULL;
TCHAR gszFilePath[0x400];
TCHAR gszCaptionVal[0x400] = {0, };

void Log(const TCHAR* fmt, ...)
{
    FILE* fl = NULL;
    TCHAR szPath[0x400];
    _stprintf_s(szPath, _T("%s.log"), gszFilePath);
    if (gszFilePath[0])
        _tfopen_s(&fl, szPath, _T("a"));

    va_list va;

    if (fl)
    {
        SYSTEMTIME st = {0, };
        GetLocalTime(&st);
        _ftprintf_s(fl, _T("%04d/%02d/%02d %02d:%02d:%02d:%03d: "), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        va_start(va, fmt);
        _vftprintf_s(fl, fmt, va);
        va_end(va);
        _ftprintf_s(fl, _T("\n"));

        _fclose_nolock(fl);
    }

	va_start(va, fmt);
	_vstprintf_s(gszCaptionVal, fmt, va);
	va_end(va);

    OutputDebugString(gszCaptionVal);

    ::InvalidateRect(ghMainWnd, NULL, TRUE);
}

RECT grcPrimaryMonitor = {0, 0, 1920, 1080};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    ghInst = hInstance;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOUSEWHEELINVOKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOUSEWHEELINVOKER));

    MSG msg;

    POINT pt = {-10000000, -10000000};
	HMONITOR hM = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO mi = { 0, };
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(hM, &mi))
        grcPrimaryMonitor = mi.rcMonitor;

    //DialogBox(hInst, MAKEINTRESOURCE(IDD_KONTROL_SPLASH), NULL, KontrolSplashProc);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

HHOOK ghMouseHook = NULL;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT* pSt = (MSLLHOOKSTRUCT*)lParam;
        static DWORD dwMouseFlag = 0;
        static POINT ptClicked = {0, };
        static DWORD dwOldMouseFlag = 0;
        switch (wParam)
        {
        case WM_LBUTTONDOWN:
            dwMouseFlag |= 1;
            ptClicked = pSt->pt;
            Log(_T("mouse %d\n"), dwMouseFlag);
            break;
        case WM_RBUTTONDOWN:
            dwMouseFlag |= 2;
            ptClicked = pSt->pt;
            Log(_T("mouse %d\n"), dwMouseFlag);
//             if (dwMouseFlag == 3)
//                 return 1;
            break;
        case WM_LBUTTONUP:
            {
                dwMouseFlag &= ~1;
                DWORD dwOldVal = dwOldMouseFlag;
                if (dwMouseFlag == 0 && dwOldMouseFlag != 0)
                {
                    KillTimer(ghMainWnd, 1);
                    KillTimer(ghMainWnd, 2);
                    dwOldMouseFlag = 0;

                    Log(_T("Kill timer 1, 2\n"));
                }
                Log(_T("mouse %d\n"), dwMouseFlag);
            }
            break;
        case WM_RBUTTONUP:
            {
                dwMouseFlag &= ~2;
                DWORD dwOldVal = dwOldMouseFlag;
                if (dwMouseFlag == 0 && dwOldMouseFlag != 0)
                {
                    KillTimer(ghMainWnd, 1);
                    KillTimer(ghMainWnd, 2);
                    dwOldMouseFlag = 0;

                    Log(_T("Kill timer 1, 2\n"));
                }
                Log(_T("mouse %d\n"), dwMouseFlag);
//                 if (dwOldVal == 3)
//                     return 1;
            }
            break;
        case WM_MOUSEMOVE:
            if ((dwMouseFlag & 3) == 3)
            {
                dwOldMouseFlag = dwMouseFlag;

                int dist = pSt->pt.y - ptClicked.y;
                KillTimer(ghMainWnd, 1);
                KillTimer(ghMainWnd, 2);
                if (dist > 0)
                {
                    DWORD dwPeriod = 0;
                    if (dist < 10)
                        dwPeriod = 200;
                    else if (dist < 20)
                        dwPeriod = 100;
                    else if (dist < 50)
                        dwPeriod = 50;
                    else if (dist < 100)
                        dwPeriod = 30;
                    else
                        dwPeriod = 10;
                    SetTimer(ghMainWnd, 1, dwPeriod, NULL);
                    Log(_T("timer 1 set %d, mouse offset = %d\n"), dwPeriod, dist);
                }
                else
                {
                    DWORD dwPeriod = 0;
                    dist = -dist;
					if (dist < 10)
						dwPeriod = 200;
					else if (dist < 20)
						dwPeriod = 100;
					else if (dist < 50)
						dwPeriod = 50;
					else if (dist < 100)
						dwPeriod = 30;
					else
						dwPeriod = 10;

                    SetTimer(ghMainWnd, 2, dwPeriod, NULL);
                    Log(_T("timer 2 set %d, mouse offset = %d\n"), dwPeriod, -dist);
                }
            }
//             if ((dwMouseFlag & 3) == 3)
//                 return 1;
            break;
        }
	}
    return CallNextHookEx(ghMouseHook, nCode, wParam, lParam);
}

bool SetCaptureMode()
{
    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (hMouseHook == NULL)
    {
        return false;
    }

    ghMouseHook = hMouseHook;
    return true;
}

void ReleaseCaptureMode()
{
    if (ghMouseHook)
        UnhookWindowsHookEx(ghMouseHook);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSEWHEELINVOKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOUSEWHEELINVOKER);
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

   ghMainWnd = hWnd;
   SetCaptureMode();

   return TRUE;
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
            RECT rc = {0, };
            GetClientRect(hWnd, &rc);

            ::DrawText(hdc, gszCaptionVal, _tcslen(gszCaptionVal), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        ReleaseCaptureMode();
        PostQuitMessage(0);
        break;
//     case WM_KEYDOWN:
//             switch (wParam)
//             {
//             case VK_UP:
//                 ::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 65536 / 2, 65536 / 2, 0, 0);
//                 break;
//     		case VK_DOWN:
//     			::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 65536 + 65536 / 2, 65536 / 2, 0, 0);
//     			break;
//     		case VK_RIGHT:
//     			::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 65536 / 2, 65536 / 4, 0, 0);
//     			break;
//     		case VK_LEFT:
//     			::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 65536 + 65536 / 2, 65536 / 4, 0, 0);
//     			break;
//             }
//             break;
    case WM_TIMER:
        if (wParam == 1 || wParam == 2)
        {
		    DWORD dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL;
            POINT pt;
            GetCursorPos(&pt);

		    int xtransformed = (pt.x - grcPrimaryMonitor.left) * 65536 / (grcPrimaryMonitor.right - grcPrimaryMonitor.left);
		    int ytransformed = (pt.y - grcPrimaryMonitor.top) * 65536 / (grcPrimaryMonitor.bottom - grcPrimaryMonitor.top);
            int nVal = 0;
            if (wParam == 1)
                nVal = -WHEEL_DELTA;
            else if (wParam == 2)
                nVal = WHEEL_DELTA;

		    ::mouse_event(dwFlag, xtransformed, ytransformed, nVal, 0);
        }
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

INT_PTR CALLBACK KontrolSplashProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

