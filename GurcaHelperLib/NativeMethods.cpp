//  Filename:  NativeMethods.cpp

#include "Stdafx.h"
#include <Windows.h>
#include <vcclr.h>
#include <WtsApi32.h>
#include <pchannel.h>
#include <iostream>

#pragma comment (lib, "User32.lib")
#pragma comment (lib, "wtsapi32.lib")

#define _MAX_WAIT		60000
#define MAX_MSG_SIZE	0x20000
#define START_MSG_SIZE	4
#define STEP_MSG_SIZE	13

//  Callback signatures
//
typedef void(__stdcall * CLIENTCALLBACK)(const wchar_t *);
typedef void(__stdcall * ERRORCALLBACK)(const wchar_t *);

//  Need something along the lines of  READINFO and WRITEINFO
//
typedef struct _ReadInfoStruct
{
	HANDLE hVC;
	CLIENTCALLBACK ccb;
	ERRORCALLBACK ecb;
} READINFOSTRUCT, *LPREADINFOSTRUCT;

typedef struct _WriteInfoStruct
{
	HANDLE hVC;
	wchar_t *wszMessage;
	ERRORCALLBACK ecb;
} WRITEINFOSTRUCT, *LPWRITEINFOSTRUCT;

//  Thread Functions
//
DWORD WINAPI ReadThread(PVOID param)
{
	OutputDebugString(L"Starting ReadThread\n");
	LPREADINFOSTRUCT lpRis = (LPREADINFOSTRUCT)param;

	HANDLE hFile;
	BYTE ReadBuffer[CHANNEL_PDU_LENGTH];
	DWORD dwRead;
	BYTE b = 0;
	CHANNEL_PDU_HEADER *pHdr;
	BOOL fSucc;

	HANDLE hEvent;

	hFile = (HANDLE)lpRis->hVC;
	pHdr = (CHANNEL_PDU_HEADER*)ReadBuffer;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		OVERLAPPED Overlapped = { 0 };
		DWORD TotalRead = 0;
		DWORD TotalData = 0;
		LPBYTE lpData = NULL;

		do
		{
			Overlapped.hEvent = hEvent;

			//  Read the entire message
			fSucc = ReadFile(
				hFile,
				ReadBuffer,
				sizeof(ReadBuffer),
				&dwRead,
				&Overlapped);
			if (!fSucc)
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
					DWORD dw = WaitForSingleObject(Overlapped.hEvent, INFINITE);
					_ASSERT(WAIT_OBJECT_0 == dw);
					fSucc = GetOverlappedResult(
						hFile,
						&Overlapped,
						&dwRead,
						FALSE);
				}
			}

			if (!fSucc)
			{
				//  do the error callback thingy
				wchar_t wszErrXml[1024];
				DWORD dwError = GetLastError();
				LPVOID lpMsgBuf;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					dwError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&lpMsgBuf,
					0,
					NULL);

				wsprintf(wszErrXml, L"<Error ErrorReason=\"%s\" />", (LPCTSTR)lpMsgBuf);
				lpRis->ecb((const wchar_t*)wszErrXml);

				OutputDebugString((LPCTSTR)lpMsgBuf);
				OutputDebugString(L"\n");

				goto ErrorExit;
			}

			ULONG packetSize = dwRead - sizeof(*pHdr);
			TotalRead += packetSize;

			lpData = (LPBYTE)((lpData == NULL) ? GlobalAlloc(GPTR, packetSize) : 
				GlobalReAlloc((HGLOBAL)lpData, GlobalSize(lpData) + packetSize, GMEM_ZEROINIT));
			LPBYTE lpWriteData = &lpData[GlobalSize(lpData) - packetSize];
			RtlCopyMemory(lpWriteData, &ReadBuffer[sizeof(*pHdr)], packetSize);

		} while (0 == (pHdr->flags & CHANNEL_FLAG_LAST));

		OutputDebugString(L"Calling ClientCallback with: ");
		OutputDebugString((const wchar_t*)lpData);
		OutputDebugString(L"\n");
		lpRis->ccb((const wchar_t*)lpData);

		GlobalFree(lpData);
	}

ErrorExit:
	//  do error-exity things here
	
	return 0;
}

DWORD WINAPI WriteThread(PVOID param)
{
	LPWRITEINFOSTRUCT lpWis = (LPWRITEINFOSTRUCT)param;
	HANDLE hFile;	
	DWORD dwWritten;
	BOOL fSucc;
	BYTE  b = 0;
	HANDLE hEvent;

	hFile = lpWis->hVC;
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	OVERLAPPED Overlapped = { 0 };
	Overlapped.hEvent = hEvent;

	DWORD dwNumBytes = (wcslen(lpWis->wszMessage) * sizeof(wchar_t)) + 2;
	fSucc = WriteFile(hFile, lpWis->wszMessage, dwNumBytes, &dwWritten, &Overlapped);
	if (!fSucc)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			DWORD dw = WaitForSingleObject(Overlapped.hEvent, _MAX_WAIT);
			_ASSERT(WAIT_OBJECT_0 == dw);
			fSucc = GetOverlappedResult(hFile, &Overlapped, &dwWritten, FALSE);
		}
	}

	if (!fSucc && lpWis->ecb != NULL)
	{
		//  do the error callback thingy
		wchar_t wszErrXml[1024];
		DWORD dwError = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		wsprintf(wszErrXml, L"<Error ErrorReason=\"%s\" />", (LPCTSTR)lpMsgBuf);
		lpWis->ecb((const wchar_t*)wszErrXml);

		OutputDebugString((LPCTSTR)lpMsgBuf);
		OutputDebugString(L"\n");
	}

	_ASSERT(dwWritten = dwNumBytes);

ErrorExit:
	if (lpWis)
	{
		if (lpWis->wszMessage)
			delete lpWis->wszMessage;
		delete lpWis;
	}

	return 0;
}

//
//  External API
//
extern "C" __declspec(dllexport) HANDLE InitializeVCAndRegisterCallback(
	const char *szVCName, CLIENTCALLBACK clientCallback, ERRORCALLBACK errorCallback)
{
	HANDLE hWTSHandle = NULL;
	HANDLE hWTSFileHandle;
	HANDLE hRetHandle = NULL;
	PVOID vcFileHandlePtr = NULL;
	DWORD len;
	DWORD rc = ERROR_SUCCESS;

	hWTSHandle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)szVCName, WTS_CHANNEL_OPTION_DYNAMIC);
	if (NULL == hWTSHandle)
		goto ErrorExit;

	BOOL fSucc = WTSVirtualChannelQuery(hWTSHandle,
		WTSVirtualFileHandle, &vcFileHandlePtr, &len);
	if (!fSucc)
		goto ErrorExit;
	if (len != sizeof(HANDLE))
		goto ErrorExit;

	hWTSFileHandle = *(HANDLE*)vcFileHandlePtr;

	fSucc = DuplicateHandle(GetCurrentProcess(), hWTSFileHandle,
		GetCurrentProcess(), &hRetHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (!fSucc)
		goto ErrorExit;

	LPREADINFOSTRUCT lpRis = new READINFOSTRUCT();
	lpRis->hVC = hRetHandle;
	lpRis->ecb = errorCallback;
	lpRis->ccb = clientCallback;

	DWORD dwThreadId;
	CreateThread(NULL, 0, ReadThread, lpRis, 0, &dwThreadId);

ErrorExit:
	if (vcFileHandlePtr)
		WTSFreeMemory(vcFileHandlePtr);

	if (hWTSHandle)
		WTSVirtualChannelClose(hWTSHandle);

	return hRetHandle;
}

extern "C" __declspec(dllexport) BOOL SendVCMessage(HANDLE hChannelHandle, const wchar_t *wszMessage, 
	ERRORCALLBACK errorCallback)
{
	LPWRITEINFOSTRUCT lpWis = new WRITEINFOSTRUCT();
	lpWis->hVC = hChannelHandle;
	lpWis->ecb = errorCallback;
	lpWis->wszMessage = new wchar_t[wcslen(wszMessage) + 1];
	wcscpy(lpWis->wszMessage, wszMessage);

	DWORD dwThreadId;
	CreateThread(NULL, 0, WriteThread, lpWis, 0, &dwThreadId);

	return TRUE;
}