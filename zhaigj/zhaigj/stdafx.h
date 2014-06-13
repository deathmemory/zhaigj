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

#include "TrayIcon.h"
#include "../DuiLibEx/DUIlibEx.h"
#include "../CPPClient4GMP/GAClient.h"
#define GOOGLEID_NIGOULE_COM				L"UA-51732277-1"
#define GOOGLEID_AppID						L"zhaigj"
extern GAClient gaClient;
// TODO: reference additional headers your program requires here
