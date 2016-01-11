#include "stdafx.h"
#include "resource.h"
#include "Content_i.h"
#include "dllmain.h"
#include "Config.h"
#include "Utils.h"
#include "Vista.h"
#include <constants.h>
#include <Registry.h>
#include <constants.h>

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	HRESULT hr = S_OK;
	_AtlModule.SetRegistrationConfig(Config::Instance());
	hr = _AtlModule.DllRegisterServer();
	Config::Drop();
	return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	_AtlModule.SetRegistrationConfig(Config::Instance());
	Registry::RemoveAll();
	HRESULT hr = _AtlModule.DllUnregisterServer();
	Config::Drop();
	return hr;
}

HRESULT CContentModule::AddConfigReplacements(IRegistrarBase* pRegistrar, Config* pConfig)
{
	if (pConfig)
	{
		pRegistrar->AddReplacement(OLESTR("NAME"), pConfig->name);
		pRegistrar->AddReplacement(OLESTR("DESCRIPTION"), pConfig->description);
	}

	OLECHAR clsIdBrowserHelper[64] = OLESTR("");
	StringFromGUID2(CLSID_BrowserHelper, clsIdBrowserHelper, ARRAYSIZE(clsIdBrowserHelper));
	pRegistrar->AddReplacement(OLESTR("CLSIDBrowserHelper"), clsIdBrowserHelper);

	OLECHAR libIdContentLib[64] = OLESTR("");
	StringFromGUID2(LIBID_ContentLib, libIdContentLib, ARRAYSIZE(libIdContentLib));
	pRegistrar->AddReplacement(OLESTR("LIBIDContentLib"), libIdContentLib);

	return S_OK;
}


// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL)
    {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    	{
    		AtlSetPerUserRegistration(true);
    	}
    }

    if (bInstall)
    {	
    	hr = DllRegisterServer();
    	if (FAILED(hr))
    	{	
    		DllUnregisterServer();
    	}
    }
    else
    {
    	hr = DllUnregisterServer();
    }

    return hr;
}


