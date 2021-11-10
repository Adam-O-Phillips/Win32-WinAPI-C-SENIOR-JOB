// ScreenCaptureJpeg.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ScreenCaptureJpeg.h"
#include <stdio.h>

extern "C"
{
    #include "../libjpeg/jpeglib.h"
}

#if defined _DEBUG
#pragma comment(lib, "../Debug/libjpeg.lib")
#else
#pragma comment(lib, "../Release/libjpeg.lib")
#endif

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
    LoadStringW(hInstance, IDC_SCREENCAPTUREJPEG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENCAPTUREJPEG));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENCAPTUREJPEG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCREENCAPTUREJPEG);
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

void GetScreenShot(void);

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
	case WM_LBUTTONDOWN:
        {
            DWORD dwTick = GetTickCount();
            GetScreenShot();
            dwTick = GetTickCount() - dwTick;
            char szText[0x100] = {0, };
            sprintf_s(szText, "%d <=\n", dwTick);
            OutputDebugStringA(szText);
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

typedef unsigned long CARD32;
typedef unsigned short CARD16;
typedef short INT16;
typedef unsigned char  CARD8;

static JOCTET* jpegDstBuffer;
static size_t jpegDstBufferLen;
static struct jpeg_destination_mgr jpegDstManager;

static bool jpegError;
static int jpegDstDataLen;

static void
JpegInitDestination(j_compress_ptr cinfo)
{
	jpegError = false;
	jpegDstManager.next_output_byte = jpegDstBuffer;
	jpegDstManager.free_in_buffer = jpegDstBufferLen;
}

static boolean
JpegEmptyOutputBuffer(j_compress_ptr cinfo)
{
	jpegError = true;
	jpegDstManager.next_output_byte = jpegDstBuffer;
	jpegDstManager.free_in_buffer = jpegDstBufferLen;

	return TRUE;
}

static void
JpegTermDestination(j_compress_ptr cinfo)
{
	jpegDstDataLen = jpegDstBufferLen - jpegDstManager.free_in_buffer;
}

static void
JpegSetDstManager(j_compress_ptr cinfo, JOCTET* buf, size_t buflen)
{
	jpegDstBuffer = buf;
	jpegDstBufferLen = buflen;
	jpegDstManager.init_destination = JpegInitDestination;
	jpegDstManager.empty_output_buffer = JpegEmptyOutputBuffer;
	jpegDstManager.term_destination = JpegTermDestination;
	cinfo->dest = &jpegDstManager;
}


void
PrepareRowForJpeg24(BYTE* dst, BYTE* src, int count)
{
	while (count--) {
		*dst++ = (BYTE)(*src++);
		*dst++ = (BYTE)(*src++);
		*dst++ = (BYTE)(*src++);
	}
}

void PrepareRowForJpeg16(BYTE *dst, CARD16 *src, int count)
{
	int r_shift = 11;
	int g_shift = 5;
	int b_shift = 0;
	int r_max = 0x1F;
	int g_max = 0x3F;
	int b_max = 0x1F;

	CARD16 pix;
	while (count--) {
		pix = *src++;
		*dst++ = (BYTE)((pix >> r_shift & r_max) * 255 / r_max);
		*dst++ = (BYTE)((pix >> g_shift & g_max) * 255 / g_max);
		*dst++ = (BYTE)((pix >> b_shift & b_max) * 255 / b_max);
	}
}


void PrepareRowForJpeg32(BYTE* dst, CARD32* src, int count)
{
	int r_shift = 16;
	int g_shift = 8;
	int b_shift = 0;
	int r_max = 0xFF;
	int g_max = 0xFF;
	int b_max = 0xFF;

	CARD32 pix;
	while (count--) {
		pix = *src++;
		*dst++ = (BYTE)((pix >> r_shift & r_max));
		*dst++ = (BYTE)((pix >> g_shift & g_max));
		*dst++ = (BYTE)((pix >> b_shift & b_max));
	}
}

void 
PrepareRowForJpeg(BYTE* dst, int y, int w, BITMAPINFO *pBmpInfo, BYTE* srcBuffer)
{	
	if (pBmpInfo->bmiHeader.biBitCount == 32) {
		CARD32* src = (CARD32*)&srcBuffer[y * w * (pBmpInfo->bmiHeader.biBitCount / 8)];
		PrepareRowForJpeg32(dst, src, w);
	}
	else if (pBmpInfo->bmiHeader.biBitCount == 24) {
		int nEffectWidth = ((((24 * w) + 31) / 32) * 4);
		BYTE* src = (BYTE*)&srcBuffer[y * nEffectWidth];
		PrepareRowForJpeg24(dst, src, w);
	}
	else {
		// 16 bpp assumed.
		CARD16* src = (CARD16*)&srcBuffer[y * w * (pBmpInfo->bmiHeader.biBitCount / 8)];
		PrepareRowForJpeg16(dst, src, w);
	}
}


int CompressByJpeg(BYTE* dst, int dstlen, int w, int h, int quality, BITMAPINFO *pBmpInfo, BYTE* from)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	BYTE* srcBuf = new byte[w * 3];
	JSAMPROW rowPointer[1];
	rowPointer[0] = (JSAMPROW)srcBuf;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	cinfo.image_width = w;
	cinfo.image_height = h;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	JpegSetDstManager(&cinfo, dst, dstlen);

	jpeg_start_compress(&cinfo, TRUE);

	for (int dy = 0; dy < h; dy++) {
		PrepareRowForJpeg(srcBuf, h - 1 - dy, w, pBmpInfo, from);
		jpeg_write_scanlines(&cinfo, rowPointer, 1);
		if (jpegError)
			break;
	}

 	if (!jpegError)
 		jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);
	delete[] srcBuf;

	return dstlen - cinfo.dest->free_in_buffer;
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

	HDC     hScreen = GetDC(NULL);

	// copy screen to bitmap
	static int screen_width = 0, screen_height = 0;
	static HDC     hMemDC = NULL;
	static HBITMAP hMemBmp = NULL;
	static BITMAP memBmp = { 0, };

	static BITMAPINFO bmi = { 0, };
	static BYTE *pucDIBbits = NULL;

	PBITMAPINFO pbmi = &bmi;

	if (screen_width < w || screen_height < h)
	{
		if (hMemBmp)
			::DeleteObject(hMemBmp);

		if (hMemDC != NULL)
			::DeleteObject(hMemDC);

		hMemDC = ::CreateCompatibleDC(hScreen);
		hMemBmp = ::CreateCompatibleBitmap(hScreen, w, h);

		if (!GetObject(hMemBmp, sizeof(BITMAP), (LPSTR)&memBmp))
		{
			// error
		}

		::DeleteObject(hMemBmp);

		// Convert the color format to a count of bits.  
		WORD cClrBits = (WORD)(memBmp.bmPlanes * memBmp.bmBitsPixel);
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
		else
			cClrBits = 32;

		// Allocate memory for the BITMAPINFO structure. (This structure  
		// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
		// data structures.)  

		// Initialize the fields in the BITMAPINFO structure.  

		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = memBmp.bmWidth;
		pbmi->bmiHeader.biHeight = memBmp.bmHeight;
		pbmi->bmiHeader.biPlanes = memBmp.bmPlanes;
		pbmi->bmiHeader.biBitCount = memBmp.bmBitsPixel;
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

		HBITMAP tempbitmap = CreateDIBSection(
			hMemDC,
			pbmi,
			DIB_RGB_COLORS,
			(void**)&pucDIBbits,
			NULL,
			0);

		hMemBmp = tempbitmap;

		screen_width = w;
		screen_height = h;
	}

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

	BOOL bRet = BitBlt(hMemDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

	::SelectObject(hMemDC, hOldBitmap);

	// save bitmap to clipboard
// 	OpenClipboard(NULL);
// 	EmptyClipboard();
// 	SetClipboardData(CF_BITMAP, hBitmap);
// 	CloseClipboard();
// 	CImage img;
// 	img.Attach(hMemBmp);
// 	img.Save(_T("2.jpg"));

	static LPBYTE lpJPGBits = NULL;
	static int nSrcBitSize = 0;
	if (nSrcBitSize < pbmi->bmiHeader.biSizeImage)
	{
		nSrcBitSize = pbmi->bmiHeader.biSizeImage;
		lpJPGBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, nSrcBitSize);
	}

	if (!lpJPGBits)
	{
		//error
	}

//	int nlen = CompressByJpeg(lpJPGBits, pbmi->bmiHeader.biSizeImage, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, 90, pbmi, pucDIBbits);
// 	{
// 		LPBYTE lpData = lpJPGBits;
// 		lpData = pucDIBbits;
// 		int nlen = nSrcBitSize;
// 		FILE* fl = NULL;
// 		fopen_s(&fl, "1.jpg", "wb");
// 		if (fl)
// 		{
// 			_fwrite_nolock(lpJPGBits, 1, nlen, fl);
// 			_fclose_nolock(fl);
// 		}
// 	}

	ReleaseDC(NULL, hScreen);
}
