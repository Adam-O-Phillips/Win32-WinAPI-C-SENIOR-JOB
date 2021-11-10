// FindWindowTextOnMouse.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "FindWindowTextOnMouse.h"

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
    LoadStringW(hInstance, IDC_FINDWINDOWTEXTONMOUSE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINDWINDOWTEXTONMOUSE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINDWINDOWTEXTONMOUSE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FINDWINDOWTEXTONMOUSE);
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
	Log(_T("error: %s-0x%08X, error=%d\n"), text, hVal, GetLastError());
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)& bmp))
	{
		int err = GetLastError();
		struct {
			BITMAPINFOHEADER bmiHeader;
			RGBQUAD bmiColors[256];
		} bmi;

		HWND hDesktop = GetDesktopWindow();
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		HDC hDC = GetDC(NULL);  // any DC will work

		GetDIBits(hDC, hBmp, 0, 1, NULL, (BITMAPINFO*)& bmi,
			DIB_RGB_COLORS);

		ReleaseDC(NULL, hDC);

		if (bmi.bmiHeader.biBitCount > 0)
		{	
			bmp.bmBitsPixel = bmi.bmiHeader.biBitCount;
			bmp.bmHeight = bmi.bmiHeader.biHeight;
			bmp.bmPlanes = bmi.bmiHeader.biPlanes;
			bmp.bmWidth = bmi.bmiHeader.biWidth;
		}
		else
		{
			errhandler(_T("GetObject"), hBmp);
			return NULL;
		}
	}

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

void CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi,
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
		errhandler(_T("GlobalAlloc"), hBMP);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		errhandler(_T("GetDIBits"), hBMP);
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
		errhandler(_T("CreateFile"), hBMP);
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
		errhandler(_T("WriteFile"), hBMP);
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)& dwTmp, (NULL)))
		errhandler(_T("WriteFile"), hBMP);

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)& dwTmp, NULL))
		errhandler(_T("WriteFile"), hBMP);

	// Close the .BMP file.  
	if (!CloseHandle(hf))
		errhandler(_T("CloseHandle"), hBMP);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

static int seq = 0;

void CreateBitmapFileFromWnd(HWND hWnd)
{
	TCHAR szCaption[0x100];
	szCaption[GetWindowText(hWnd, szCaption, 0x100)] = '\0';

	hWnd = GetDesktopWindow();

	RECT rc = { 0, };
	GetWindowRect(hWnd, &rc);

	TCHAR szFilePath[0x200];
	_stprintf(szFilePath, _T("thumb%04d.bmp"), seq++);

	HDC hDC = ::GetWindowDC(hWnd);
	HBITMAP hBMP = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);

	PBITMAPINFO pInfo = CreateBitmapInfoStruct(hBMP);
	if (pInfo)
	{
		CreateBMPFile(szFilePath, pInfo, hBMP, hDC);
		::LocalFree(pInfo);

		Log(_T("0x%X-[%s]-(%d, %d, %d, %d)-thumb(%s)\n"), hWnd, szCaption, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, szFilePath);
	}

	::ReleaseDC(hWnd, hDC);
}

#include <atlimage.h>

BOOL SaveHBITMAPToFile(HDC hBmpDC, HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight = -Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
		* Bitmap0.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

// 	hPal = GetStockObject(DEFAULT_PALETTE);
// 	if (hPal)
// 	{
// 		hDC = GetDC(NULL);
// 		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
// 		RealizePalette(hDC);
// 	}
// 

	GetDIBits(hBmpDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

// 	if (hOldPal2)
// 	{
// 		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
// 		RealizePalette(hDC);
// 		ReleaseDC(NULL, hDC);
// 	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

void GetScreenShot(void)
{
	int x1, y1, x2, y2, w, h;

	// get screen dimensions
	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	w = x2;
	h = y2;

	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

	// save bitmap to clipboard
// 	OpenClipboard(NULL);
// 	EmptyClipboard();
// 	SetClipboardData(CF_BITMAP, hBitmap);
// 	CloseClipboard();
	CImage img;
	img.Attach(hBitmap);
	img.Save(_T("1.jpg"));

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
}

void CreateBitmapFileFromDesktop(HWND hWnd)
{
	DWORD dwTick1 = GetTickCount();
	GetScreenShot();
	dwTick1 = GetTickCount() - dwTick1;
	char szText[0x100] = {0, };
	sprintf_s(szText, "%d =>\n", dwTick1);
	OutputDebugStringA(szText);

	return;
	DWORD dwTick = GetTickCount();
// 	RECT rcWnd = {0, };
// 	GetWindowRect(hWnd, &rcWnd);
// 
// 	HWND hDesktopWnd = GetDesktopWindow();
// 
// 	RECT rcDesktop = {0, };
// 	GetWindowRect(hDesktopWnd, &rcDesktop);
// 
// 	int cx = rcWnd.right - rcWnd.left, cy = rcWnd.bottom - rcWnd.top;

	HDC hDC = ::GetDC(NULL);
 	HBITMAP hDesktopBmp = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
// 	CImage img;
// 	img.Attach(hDesktopBmp);
// 	img.Save(_T("1.jpg"));

// 	HDC hMemDC = CreateCompatibleDC(hDC);
// 	HBITMAP hMemBmp = (HBITMAP)::CreateCompatibleBitmap(hDC, cx, cy);
// 	HBITMAP hOldMemBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);
// 
// 	::BitBlt(hMemDC, 0, 0, cx, cy, hDC, rcWnd.left, rcWnd.top, SRCCOPY);

	// save bitmap to clipboard
	TCHAR szFilePath[0x200];
	_stprintf(szFilePath, _T("thumb%04d.bmp"), seq++);

	SaveHBITMAPToFile(hDC, hDesktopBmp, szFilePath);
		
// 	HBITMAP hMemBmp = hDesktopBmp;
// 	HDC hMemDC = hDC;
// 	PBITMAPINFO pInfo = CreateBitmapInfoStruct(hMemBmp);
// 	if (pInfo)
// 	{
// 		CreateBMPFile(szFilePath, pInfo, hMemBmp, hMemDC);
// 		::LocalFree(pInfo);
// 
// 		TCHAR szCaption[0x100] = {0, };
// 		GetWindowText(hWnd, szCaption, 0xFF);
// 		Log(_T("0x%X-[%s]-(%d, %d, %d, %d)-thumb(%s)\n"), hWnd, szCaption, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, szFilePath);
//	}

// 	::DeleteObject(::SelectObject(hMemDC, hOldMemBmp));
// 	::DeleteObject(hMemDC);
 	::ReleaseDC(NULL, hDC);

	dwTick = GetTickCount() - dwTick;
	dwTick = dwTick;
}

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
        PostQuitMessage(0);
        break;
	case WM_TIMER:
		{
			POINT pt = {0, };
			GetCursorPos(&pt);
			HWND hFound = WindowFromPoint(pt);
			HWND hWndParent = NULL;
			while (hWndParent = ::GetParent(hFound))
				hFound = hWndParent;

			static HWND hStaticFound = NULL;
			if (HIWORD(GetAsyncKeyState(VK_CONTROL)) > 0 && hFound != hStaticFound)
			{
				hStaticFound = hFound;
				CreateBitmapFileFromDesktop(hFound);
				//CreateBitmapFileFromWnd(hStaticFound);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		CreateBitmapFileFromDesktop(NULL);
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
