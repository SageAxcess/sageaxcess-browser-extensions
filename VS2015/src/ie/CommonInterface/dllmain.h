// dllmain.h : Declaration of module class.

#include "CommonInterface.h"

class CCommonInterfaceModule : public CAtlDllModuleT< CCommonInterfaceModule >
{
public :
	DECLARE_LIBID(LIBID_CommonInterfaceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMMONINTERFACE, "{1B8BEEB2-E407-481D-B102-64110E4C341F}")
};

extern class CCommonInterfaceModule _AtlModule;
