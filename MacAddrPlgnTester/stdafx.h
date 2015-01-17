// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <wtsapi32.h>
#include <pchannel.h>

#pragma comment(lib, "wtsapi32.lib")

#define TS_MACADDR_NAME "TsClientMacAddr"


//#import <msxml6.dll> raw_interfaces_only

#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "msxml6.lib")
#include <MsXml6.h>
