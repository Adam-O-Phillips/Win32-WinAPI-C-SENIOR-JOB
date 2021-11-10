// MouseKeyboardEmulator.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MouseKeyboardEmulator.h"
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
bool IsDoubleCtrlPressed();
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
    LoadStringW(hInstance, IDC_MOUSEKEYBOARDEMULATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOUSEKEYBOARDEMULATOR));

    MSG msg;

	TCHAR szFilePath[0x400] = { 0, };
	GetModuleFileName(GetModuleHandle(NULL), szFilePath, 0x400 - 1);
	TCHAR* pLastSlash = _tcsrchr(szFilePath, '\\');
	if (pLastSlash)
		*++pLastSlash = '\0';

	_stprintf_s(gszFilePath, _T("%scapture.dat"), szFilePath);

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

static int gnState = 0;

bool IsDoubleCtrlPressed()
{
    static bool bPressed = false;
    if (!bPressed && (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 && (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0)
    {
        bPressed = true;
        return true;
    }
    else if (bPressed && (GetAsyncKeyState(VK_LCONTROL) & 0x8000) == 0 && (GetAsyncKeyState(VK_RCONTROL) & 0x8000) == 0)
    {
        bPressed = false;
    }
    return false;
}

bool IsDoubleShiftPressed()
{
	static bool bPressed = false;
	if (!bPressed && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0 && (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0)
	{
		bPressed = true;
		return true;
	}
	else if (bPressed && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) == 0 && (GetAsyncKeyState(VK_RSHIFT) & 0x8000) == 0)
	{
		bPressed = false;
	}
	return false;
}

HHOOK ghKeyHook = NULL, ghMouseHook = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (gnState == 3 || gnState == 5))
    {
        KBDLLHOOKSTRUCT* pSt = (KBDLLHOOKSTRUCT*)lParam;
        DWORD dwNow = GetTickCount();
        FILE* fl = NULL;
        _tfopen_s(&fl, gszFilePath, _T("ab"));
        if (fl)
        {
            char chVal = 0;
            _fwrite_nolock(&chVal, 1, 1, fl);
            _fwrite_nolock(&wParam, sizeof(wParam), 1, fl);
            _fwrite_nolock(pSt, sizeof(*pSt), 1, fl);
            _fclose_nolock(fl);
        }
    }
    return CallNextHookEx(ghKeyHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && (gnState == 3 || gnState == 5))
	{
		MSLLHOOKSTRUCT* pSt = (MSLLHOOKSTRUCT*)lParam;
		DWORD dwNow = GetTickCount();
		FILE* fl = NULL;
		_tfopen_s(&fl, gszFilePath, _T("ab"));
		if (fl)
		{
			char chVal = 1;
			_fwrite_nolock(&chVal, 1, 1, fl);
			_fwrite_nolock(&wParam, sizeof(wParam), 1, fl);
			_fwrite_nolock(pSt, sizeof(*pSt), 1, fl);
			_fclose_nolock(fl);
		}
	}
    return CallNextHookEx(ghMouseHook, nCode, wParam, lParam);
}

static time_t tTickPlaybackGap = 0;
FILE* flCapture = 0;
bool bFileLoad = true;

void PlaybackCaptureData(time_t curTick)
{
    do 
    {
        bool bRewind = false;
        if (bFileLoad)
        {
            bFileLoad = false;

            if (flCapture)
            {
                Log(_T("Finished playing recorded data."));
                _fclose_nolock(flCapture);
                //gnState = 5;
                bFileLoad = true;
                flCapture = NULL;
                return;
            }

		    _tfopen_s(&flCapture, gszFilePath, _T("rb"));

            bRewind = true;
        }

        FILE* fl = flCapture;
        if (fl == NULL)
        {
            Log(_T("No recorded data exist."));
            gnState = 5;
            break;
        }

        static int nPlayStep = 0;
        static char chType = 0;
        static WPARAM wParam = 0;
        static KBDLLHOOKSTRUCT tKeySt = { 0, };
        static MSLLHOOKSTRUCT tMouseSt = { 0, };

        if (nPlayStep == 0)
        {   
            do
            {
                if (_fread_nolock(&chType, 1, 1, fl) < 1)
                    break;
                
                if (_fread_nolock(&wParam, sizeof(wParam), 1, fl) < 1)
                    break;

                if (chType == 0)
                {   
                    if (_fread_nolock(&tKeySt, sizeof(tKeySt), 1, fl) < 1)
                        break;
                }
                else if (chType == 1)
                {   
                    if (_fread_nolock(&tMouseSt, sizeof(tMouseSt), 1, fl) < 1)
                        break;
                }
                nPlayStep = 1;
            } while (0);

            if (nPlayStep == 0)
            {
                bFileLoad = true;
                break;
            }
        }

        if (nPlayStep == 1)
        {
            if (chType == 0) // keyboard hook data
            {
				if (bRewind)
					tTickPlaybackGap = curTick - tKeySt.time;
				time_t tgap = curTick - (tKeySt.time + tTickPlaybackGap);

                if (tgap >= 0)
                {
                    DWORD dwFlags = 0;
                    const TCHAR* pAction = _T("");
                    if (tKeySt.flags & LLKHF_EXTENDED)
                    {
                        dwFlags |= KEYEVENTF_EXTENDEDKEY;
                        pAction = _T("Extended key:");
                    }
                    else
                        pAction = _T("Non-extended key:");

                    if (tKeySt.flags & LLKHF_UP)
                    {
                        pAction = _T("KeyRelease:");
                        dwFlags |= KEYEVENTF_KEYUP;
                    }
                    else
                        pAction = _T("KeyPress:");

                    ::keybd_event(tKeySt.vkCode, tKeySt.scanCode, dwFlags, tKeySt.dwExtraInfo);
                    Log(_T("%s vkcode=%d, scancode=%d"), pAction, tKeySt.vkCode, tKeySt.scanCode);
                    nPlayStep = 0;
                }
                else
                    break;
            }
            else if (chType == 1) // mouse hook data
            {
				if (bRewind)
					tTickPlaybackGap = curTick - tMouseSt.time;
                time_t tgap = curTick - (tMouseSt.time + tTickPlaybackGap);

				if (tgap >= 0)
				{
                    DWORD dwFlag = 0;
                    const TCHAR* pAction = NULL;
                    switch (wParam)
                    {
                    case WM_MOUSEMOVE:
                        dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
                        pAction = _T("mouse move: ");
                        break;
                    case WM_LBUTTONDOWN:
                        dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;
                        pAction = _T("mouse left button down: ");
                        break;
					case WM_LBUTTONUP:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;
                        pAction = _T("mouse left button up: ");
						break;
					case WM_RBUTTONDOWN:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN;
                        pAction = _T("mouse right button down: ");
						break;
					case WM_RBUTTONUP:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP;
                        pAction = _T("mouse right button up: ");
						break;
                    case WM_MBUTTONDOWN:
                        dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEDOWN;
                        pAction = _T("mouse middle button down: ");
                        break;
					case WM_MBUTTONUP:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEUP;
                        pAction = _T("mouse middle button up: ");
						break;
					case WM_MOUSEWHEEL:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL;
                        pAction = _T("mouse wheel: ");
						break;
                    case WM_XBUTTONDOWN:
                        dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_XDOWN;
                        pAction = _T("mouse x button down: ");
                        break;
					case WM_XBUTTONUP:
						dwFlag = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_XUP;
                        pAction = _T("mouse x button up: ");
						break;
                    }

                    int xtransformed = (tMouseSt.pt.x - grcPrimaryMonitor.left) * 65536 / (grcPrimaryMonitor.right - grcPrimaryMonitor.left);
                    int ytransformed = (tMouseSt.pt.y - grcPrimaryMonitor.top) * 65536 / (grcPrimaryMonitor.bottom - grcPrimaryMonitor.top);
                    int nVal = tMouseSt.mouseData;
                    nVal >>= 16;
                    ::mouse_event(dwFlag, xtransformed, ytransformed, nVal, tMouseSt.dwExtraInfo);
                    Log(_T("%s to (%d, %d), data=%d, extra=0x%X, time=%d"), pAction, tMouseSt.pt.x, tMouseSt.pt.y, nVal, tMouseSt.dwExtraInfo, tMouseSt.time);
                    nPlayStep = 0;
				}
				else
					break;
            }
        }
    } while (1);
}

bool SetCaptureMode()
{
    HHOOK hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (hKeyHook == NULL)
        return false;

    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (hMouseHook == NULL)
    {
        UnhookWindowsHookEx(hKeyHook);
        return false;
    }

    ghKeyHook = hKeyHook;
    ghMouseHook = hMouseHook;
    return true;
}

void ReleaseCaptureMode()
{
	if (flCapture)
		_fclose_nolock(flCapture);

    if (ghKeyHook)
        UnhookWindowsHookEx(ghKeyHook);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSEKEYBOARDEMULATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOUSEKEYBOARDEMULATOR);
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

   SetTimer(hWnd, 1, 100, NULL);
   ghMainWnd = hWnd;

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
        KillTimer(hWnd, 1);
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
	    {
            static time_t tTickOld = 0;
            if (tTickOld == 0)
                tTickOld = GetTickCount64();

			time_t tTickNow = GetTickCount64();

			switch (gnState)
			{
			case 0:
				if (IsDoubleCtrlPressed())
				{
                    if (SetCaptureMode())
                    {
                        gnState = 1;
                        tTickOld = tTickNow;
                        Log(_T("Entering to prepare capture mode"));
                    }
                    else
                    {
                        KillTimer(hWnd, 1);
                        Log(_T("Failed to configure capture mode"));
                    }
				}
				else if (tTickOld + 10000 < tTickNow)
				{
					gnState = 2;
					tTickOld = tTickNow;
                    Log(_T("Entering to prepare playback mode"));
				}
				break;
            case 1: // capture mode preparing
				if (IsDoubleCtrlPressed())
				{
					gnState = 5;
					tTickOld = tTickNow;
					Log(_T("Exitting preparation of capture mode"));
					break;
				}

                if (tTickOld + 10000 < tTickNow)
                {
                    gnState = 3;
                    tTickOld = tTickNow;
                    Log(_T("Entering capture mode"));
                }
				
                break;
            case 2: // playback mode preparing
				if (IsDoubleCtrlPressed())
				{
					gnState = 5;
					tTickOld = tTickNow;
					Log(_T("Exitting preparation of playback mode"));
					break;
				}
				if (tTickOld + 10000 < tTickNow)
				{
					gnState = 4;
					tTickOld = tTickNow;
                    Log(_T("Entering playback mode"));
				}
                break;
            case 3: // capture mode
                if (IsDoubleCtrlPressed())
                {
                    gnState = 5;
                    tTickOld = tTickNow;
                    Log(_T("Exitting capture mode"));
                    break;
                }
                break;
            case 4: // playback mode
				if (IsDoubleShiftPressed())
				{
					gnState = 5;
					tTickOld = tTickNow;
					Log(_T("Exitting playback mode"));
					break;
				}
                PlaybackCaptureData(tTickNow);
                break;
            case 5:
                if (tTickOld + 4000 < tTickNow)
                {
                    Log(_T("Exiting now...\n"));
                    KillTimer(hWnd, 1);
                    DestroyWindow(hWnd);
                }
                break;
			}
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

#include <atlimage.h>

void SetPngToWindow(HWND hWnd)
{
	// Load our PNG image
	CImage img;
    {
        TCHAR szTempFolder[MAX_PATH] = {0, };
        GetTempPath(MAX_PATH, szTempFolder);
        _tcscat_s(szTempFolder, _T("Kontrol_splash.png"));

		HMODULE hModule = GetModuleHandle(NULL); // get the handle to the current module (the executable file)
		HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(IDB_SPLASH), _T("PNG")); // substitute RESOURCE_ID and RESOURCE_TYPE.
		HGLOBAL hMemory = LoadResource(hModule, hResource);
        if (hMemory)
        {
            DWORD dwSize = SizeofResource(hModule, hResource);
            LPVOID lpAddress = LockResource(hMemory);

            char* bytes = new char[dwSize];
            memcpy(bytes, lpAddress, dwSize);

            FreeResource(hMemory);

            FILE* fl = NULL;
            _tfopen_s(&fl, szTempFolder, _T("wb"));
            if (fl)
            {
                _fwrite_nolock(bytes, 1, dwSize, fl);
                _fclose_nolock(fl);

                img.Load(szTempFolder);
            }

            delete[]bytes;
        }
    }
	
    //img.LoadFromResource(GetModuleHandle(NULL), IDB_SPLASH);
	// Get dimensions
	int iWidth = img.GetWidth();
	int iHeight = img.GetHeight();
	// Make mem DC + mem  bitmap
	HDC hdcScreen = GetDC(NULL);
	HDC hDC = CreateCompatibleDC(hdcScreen);
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, iWidth, iHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, hBmp);
	// Draw image to memory DC
	img.Draw(hDC, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);

	// Call UpdateLayeredWindow
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;
	POINT ptPos = { 0, 0 };
	SIZE sizeWnd = { iWidth, iHeight };
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(hWnd, hdcScreen, &ptPos, &sizeWnd, hDC, &ptSrc, 0, &blend, ULW_ALPHA);

	SelectObject(hDC, hBmpOld);
	DeleteObject(hBmp);
	DeleteDC(hDC);
	ReleaseDC(NULL, hdcScreen);
}

INT_PTR CALLBACK KontrolSplashProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        {
            SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetPngToWindow(hDlg);

            RECT rcWnd = {0, };
            GetWindowRect(hDlg, &rcWnd);

            HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = {0, };
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfo(hMon, &mi))
            {
                int cx = mi.rcMonitor.right - mi.rcMonitor.left - (rcWnd.right - rcWnd.left);
                int cy = mi.rcMonitor.bottom - mi.rcMonitor.top - (rcWnd.bottom - rcWnd.top);

                SetWindowPos(hDlg, NULL, mi.rcMonitor.left + cx / 2, mi.rcMonitor.top + cy / 2, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
            }
            else
            {
                int err = GetLastError();
                err = err;
            }
        }
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

