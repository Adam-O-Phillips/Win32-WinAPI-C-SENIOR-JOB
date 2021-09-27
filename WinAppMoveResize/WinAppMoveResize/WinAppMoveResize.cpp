// WinAppMoveResize.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WinAppMoveResize.h"
#include "CMoveResizeThread.h"

#pragma comment (lib, "Ws2_32.lib")

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

static CMoveResizeThread* gMRInst = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	CMoveResizeThread::InitializeNetwork();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPPMOVERESIZE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPPMOVERESIZE));

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

	if (gMRInst)
		delete gMRInst;
	gMRInst = NULL;

	CMoveResizeThread::UnloadNetwork();
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPPMOVERESIZE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPPMOVERESIZE);
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   SetTimer(hWnd, 1, 100, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   gMRInst = new CMoveResizeThread(hWnd);
   gMRInst->CreateMRCommThread();

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
//

#include <stdio.h>

void Log(const TCHAR* fmt, ...)
{
	TCHAR szString[0x400] = {0, };

	va_list va;
	va_start(va, fmt);

	szString[_vsntprintf(szString, sizeof(szString) / sizeof(szString[0]) - 1, fmt, va)] = '\0';
	va_end(va);

	FILE* fl = _tfopen(_T("log.txt"), _T("a"));
	if (fl)
	{
		_ftprintf(fl, _T("%s"), szString);
		fclose(fl);
	}
	OutputDebugString(szString);
}

void errhandler(const TCHAR* text, HANDLE hVal)
{
	Log(_T("error: %s-%d"), text, hVal);
}
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)& bmp))
		errhandler(_T("GetObject"), hwnd);

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE* hp;                   // byte pointer  
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
		errhandler(_T("GlobalAlloc"), hwnd);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		errhandler(_T("GetDIBits"), hwnd);
	}

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
		errhandler(_T("CreateFile"), hwnd);
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	if (!WriteFile(hf, (LPVOID)& hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)& dwTmp, NULL))
	{
		errhandler(_T("WriteFile"), hwnd);
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)& dwTmp, (NULL)))
		errhandler(_T("WriteFile"), hwnd);

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)& dwTmp, NULL))
		errhandler(_T("WriteFile"), hwnd);

	// Close the .BMP file.  
	if (!CloseHandle(hf))
		errhandler(_T("CloseHandle"), hwnd);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

static HWND hStaticFound = NULL;
static HWND hStaticTarget = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_MR_RECEIVE_MSG:
		{
			int nRecvLen = (int)wParam;
			const BYTE* pData = (const BYTE*)lParam;

			int wx = MAKELONG(MAKEWORD(pData[3], pData[2]), MAKEWORD(pData[1], pData[0]));
			int wy = MAKELONG(MAKEWORD(pData[7], pData[6]), MAKEWORD(pData[5], pData[4]));
			int wcx = MAKELONG(MAKEWORD(pData[11], pData[10]), MAKEWORD(pData[9], pData[8]));
			int wcy = MAKELONG(MAKEWORD(pData[15], pData[14]), MAKEWORD(pData[13], pData[12]));

			if (hStaticTarget)
				MoveWindow(hStaticTarget, wx, wy, wcx, wcy, TRUE);
				//SetWindowPos(hStaticTarget, NULL, wx, wy, wcx, wcy, SWP_SHOWWINDOW | SWP_NOZORDER);
		}
		break;
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
			RECT rc1 = {0, };
			GetClientRect(hWnd, &rc1);

			TCHAR szTotalShow[0x200] = {0, };

			_tcscpy(szTotalShow, _T("Move the mouse cursor on the window holding Ctrl key."));

			HWND hTarget = hStaticTarget;
			if (hTarget)
			{
				TCHAR szWndText[0x200] = {0, };
				GetWindowText(hTarget, szWndText, 0x1FF);

				RECT rc = {0, };
				GetWindowRect(hTarget, &rc);

				TCHAR szMsg[0x200] = {0, };
				_stprintf(szMsg, _T("\nwindow(0x%08X): [%s]-(%d, %d, %d, %d)"), hTarget, szWndText, rc.left, rc.top, rc.right, rc.bottom);
				_tcscat(szTotalShow, szMsg);
			}

			DrawText(hdc, szTotalShow, -1, &rc1, DT_CENTER | DT_VCENTER);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_TIMER:
		{
			if (HIWORD(GetKeyState(VK_CONTROL)) == 0)
				break;

			POINT pt = {0, };
			GetCursorPos(&pt);
			HWND hFound = WindowFromPoint(pt);

			if (hFound != hStaticFound && hFound != hWnd)
			{
				hStaticFound = hFound;

				TCHAR szCaption[0x100];
				szCaption[GetWindowText(hFound, szCaption, 0x100)] = '\0';

				RECT rc = {0, };
				GetWindowRect(hFound, &rc);

				if (_tcscmp(szCaption, _T("Form1")) != 0)
					hStaticTarget = hFound;

				::InvalidateRect(hWnd, NULL, TRUE);

				//Log(_T("0x%X-[%s]-(%d, %d, %d, %d)\n"), hFound, szCaption, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
#if 0
				static int seq = 0;
				TCHAR szFilePath[0x200];
				_stprintf(szFilePath, _T("thumb%04d.bmp"), seq++);

				HDC hDC = ::GetWindowDC(hFound);
				HBITMAP hBMP = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);

				PBITMAPINFO pInfo = CreateBitmapInfoStruct(hFound, hBMP);
				CreateBMPFile(hFound, szFilePath, pInfo, hBMP, hDC);
				::LocalFree(pInfo);

				::ReleaseDC(hFound, hDC);
				Log(_T("0x%X-[%s]-(%d, %d, %d, %d)-thumb(%s)\n"), hFound, szCaption, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, szFilePath);
#endif
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
