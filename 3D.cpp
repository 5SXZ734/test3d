// 3D.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <math.h>
#include "3D.h"
#include "main.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace My;


ITest *ITest::gpSelf(NULL);


int APIENTRY _tWinMain( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ND, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ND));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ND));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ND);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ITest::self()->OnSetup();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
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
		break;
	case WM_KEYDOWN:
		if (ITest::self()->OnKeyDown(wParam))
			InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
		{
				hdc = BeginPaint(hWnd, &ps);
				RECT rect;
				GetClientRect(hWnd, &rect);
				int width = rect.right;
				int height = rect.bottom;

				HDC backbuffDC = CreateCompatibleDC(hdc);

				HBITMAP backbuffer = CreateCompatibleBitmap(hdc, width, height);

				int savedDC = SaveDC(backbuffDC);
				SelectObject(backbuffDC, backbuffer);
				HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
				FillRect(backbuffDC, &rect, hBrush);
				DeleteObject(hBrush);

				ITest::self()->OnDraw(backbuffDC);

				BitBlt(hdc, 0, 0, width, height, backbuffDC, 0, 0, SRCCOPY);
				RestoreDC(backbuffDC, savedDC);

				DeleteObject(backbuffer);
				DeleteDC(backbuffDC);

				EndPaint(hWnd, &ps);
		}
		break;
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			if (ITest::self()->OnMouseDown(LOWORD(lParam), HIWORD(lParam), 0))
				InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_RBUTTONDOWN:
			SetCapture(hWnd);
			if (ITest::self()->OnMouseDown(LOWORD(lParam), HIWORD(lParam), 1))
				InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			ReleaseCapture();
			ITest::self()->OnMouseUp(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			if (ITest::self()->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)))
				InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_MOUSEMOVE:
			if (GetCapture() == hWnd)
			{
				if (ITest::self()->OnMouseMove(LOWORD(lParam), HIWORD(lParam)))
					InvalidateRect(hWnd, NULL, TRUE);
			}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
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
