// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "Content_i.h"
#include "dllmain.h"

HINSTANCE g_hInstance = NULL;
CContentModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInstance = hInstance;
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}

	BOOL bRet = _AtlModule.DllMain(dwReason, lpReserved);

	if (dwReason == DLL_PROCESS_DETACH)
	{
		CoUninitialize();
	}

	return bRet;
}
