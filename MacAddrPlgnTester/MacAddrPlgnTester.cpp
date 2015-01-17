// MacAddrPlgnTester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MacAddrPlgnTester.h"

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

//  My Functions (functions...not bloody methods)
void OpenVC(void);
void GetMAC(void);
void CloseVC(void);

//  My Global Variables
HANDLE _hDVC = NULL;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MACADDRPLGNTESTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MACADDRPLGNTESTER));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MACADDRPLGNTESTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MACADDRPLGNTESTER);
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

		case ID_VCOPERATION_OPENVC:
			OpenVC();
			break;

		case ID_VCOPERATION_GETMAC:
			GetMAC();
			break;

		case ID_VCOPERATION_CLOSEVC:
			CloseVC();
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
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

//  My Functions
void OpenVC(void)
{
	if (_hDVC != NULL)
		return;

	HANDLE hWTSHandle = NULL;
	PVOID vcFileHandlePtr = NULL;

	//  Open Virtual Channel
	hWTSHandle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION,
		TS_MACADDR_NAME, WTS_CHANNEL_OPTION_DYNAMIC);

	if (hWTSHandle == NULL)
	{
		MessageBox(NULL, L"Could not open virtual channel!", L"Oy veh!", MB_OK | MB_ICONERROR);
		return;
	}

	DWORD len;
	BOOL bSucc = WTSVirtualChannelQuery(hWTSHandle, WTSVirtualFileHandle,
		&vcFileHandlePtr, &len);
	if (!bSucc)
	{
		MessageBox(NULL, L"WTSVirtualChannelQuery failed!", L"Oy veh!", MB_OK | MB_ICONERROR);
		return;
	}

	HANDLE hWTSFileHandle = *(HANDLE *)vcFileHandlePtr;

	//  duplicate handle so that we can close
	bSucc = DuplicateHandle(GetCurrentProcess(), hWTSFileHandle, GetCurrentProcess(),
		&_hDVC, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (!bSucc)
	{
		MessageBox(NULL, L"DuplicateHandle failed!", L"Oy veh!", MB_OK | MB_ICONERROR);
		return;
	}

	//  Cleanup
	if (vcFileHandlePtr)
	{
		WTSFreeMemory(vcFileHandlePtr);
	}

	if (hWTSHandle)
	{
		WTSVirtualChannelClose(hWTSHandle);
	}
}

void GetMAC(void)
{
	OVERLAPPED ovr = { 0 };
	int nCmd = 0;

	ULONG BytesWrit = 0;
	wchar_t *wszXml = L"<MacAddrServer><Request RequestType=\"MacAddress\" /></MacAddrServer>";
	BOOL bRet = WriteFile(_hDVC, wszXml, (wcslen(wszXml) + 1) * sizeof(wchar_t), &BytesWrit, &ovr);

	if (!bRet && ERROR_IO_PENDING == GetLastError())
	{
		bRet = GetOverlappedResult(_hDVC, &ovr, &BytesWrit, TRUE);
	}

	if (!bRet)
	{
		MessageBox(NULL, L"GetOverlappedResult failed!", L"Oh no!", MB_OK | MB_ICONERROR);
		return;
	}

	//  Now get client result
	ULONG BytesRead = 0;
	BYTE ReadBuf[CHANNEL_PDU_LENGTH];
	
	//  we have to read at least channel PDU len on the DVC
	//
	bRet = ReadFile(_hDVC, ReadBuf, CHANNEL_PDU_LENGTH, &BytesRead, &ovr);

	if (!bRet && ERROR_IO_PENDING == GetLastError())
	{
		DWORD to = INFINITE;
		
		DWORD dwRet = WaitForSingleObject(_hDVC, to);
		
		if (WAIT_TIMEOUT == dwRet)
		{
			MessageBox(NULL, L"Timed out waiting on client response!", L"Oh no!", MB_OK | MB_ICONERROR);
			return;
		}

		bRet = GetOverlappedResult(_hDVC, &ovr, &BytesRead, TRUE);
	}

	if (!bRet)
	{
		MessageBox(NULL, L"GetOverlappedResult failed!", L"Oh no!", MB_OK | MB_ICONERROR);
		return;
	}

	if (BytesRead == 8)
	{
		MessageBox(NULL, L"Successfully read 8 bytes from the client!!", L"K'Plah!", MB_OK | MB_ICONEXCLAMATION);

	}
	else
	{
		MessageBox(NULL, L"Didn't read 8 bytes from the client!!", L"Uggh!", MB_OK | MB_ICONEXCLAMATION);
	}

}

void CloseVC(void)
{
	//  GNDN
}
