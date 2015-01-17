// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "MacAddrPlgn_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CMacAddrPlgnModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString(L"MacAddrPlgn:  DLL_PROCESS_ATTACH\n");
		break;

	case DLL_PROCESS_DETACH:
		OutputDebugString(L"MacAddrPlgn:  DLL_PROCESS_DETACH\n");
		break;

	default:
		break;
	}

#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
