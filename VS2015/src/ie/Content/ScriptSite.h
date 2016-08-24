#pragma once

#include "activscp.h"
#include "ActiveScriptSiteDebug.h"

typedef ATL::CAtlMap<ATL::CString, IUnknown*> NamedItemsMap;

class ATL_NO_VTABLE ScriptSite :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IActiveScriptSite,
	public IActiveScriptSiteWindow,
	public IActiveScriptSiteDebugImpl
{
public:
	ScriptSite();
	virtual ~ScriptSite();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	BEGIN_COM_MAP(ScriptSite)
		COM_INTERFACE_ENTRY(IActiveScriptSite)
		COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
		COM_INTERFACE_ENTRY(IActiveScriptSiteDebug)
		COM_INTERFACE_ENTRY_IID(IID_IUnknown, ScriptSite)
	END_COM_MAP()

public:

	STDMETHOD(AddNamedItem)(LPCOLESTR pstrName, DWORD dwFlags, IUnknown* pIUnknown);
	STDMETHOD(InvokeN)(CString name, DISPPARAMS& params, VARIANT* retVal = NULL);
	STDMETHOD(Initialize)();
	STDMETHOD(Uninitialize)();
	STDMETHOD(AddScript)(LPCTSTR filePath, LPCTSTR scriptSrc);
	STDMETHOD(SetScriptState)(SCRIPTSTATE ss);
	STDMETHOD(GetScriptState)(SCRIPTSTATE& ss);

private:
	// IActiveScriptSite 
	STDMETHODIMP GetLCID(LCID* plcid);	
	STDMETHODIMP GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppunkItem, ITypeInfo **ppTypeInfo);	
	STDMETHODIMP GetDocVersionString(BSTR* pbstrVersionString);
	STDMETHODIMP OnScriptTerminate(const VARIANT* pvarResult, const EXCEPINFO* pexcepinfo);
	STDMETHODIMP OnStateChange(SCRIPTSTATE ssScriptState);   
	STDMETHODIMP OnScriptError(IActiveScriptError* pase);
	STDMETHODIMP OnEnterScript(void);
	STDMETHODIMP OnLeaveScript(void);

	// IActiveScriptSiteWindow 
	STDMETHODIMP GetWindow(HWND* pHwnd);
	STDMETHODIMP EnableModeless(BOOL fEnable); 

protected:      
	// reference to the scripting engine
	CComQIPtr<IActiveScript> m_pEngine;      
	// reference to scripting engine in parsing mode
	CComQIPtr<IActiveScriptParse> m_pParser;   
	// named items
	NamedItemsMap m_namedItems;
};
