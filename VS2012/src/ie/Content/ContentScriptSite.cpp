#include "stdafx.h"
#include "ScriptSite.h"

ScriptSite::ScriptSite()
{	
	
}

ScriptSite::~ScriptSite()
{
	if (m_pParser) 
        m_pParser.Release();

	if (m_pEngine)
	{
		m_pEngine->Close();
        m_pEngine.Release();
	}
}

HRESULT ScriptSite::FinalConstruct()
{
	return S_OK;
}

void ScriptSite::FinalRelease()
{

}

HRESULT ScriptSite::AddNamedItem(LPCOLESTR pstrName, DWORD dwFlags, IUnknown* pIUnknown)
{
	HRESULT hr(E_FAIL);

	m_namedItems.SetAt(pstrName, pIUnknown);
	
	hr = m_pEngine->AddNamedItem(pstrName, dwFlags);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT ScriptSite::Initialize()
{
	HRESULT hr = E_FAIL;

	CLSID CLSID_JScript;
	CLSIDFromProgID(OLESTR("JScript"), &CLSID_JScript);
	hr = m_pEngine.CoCreateInstance(CLSID_JScript);
	if (SUCCEEDED(hr))
	{
		hr = InitializeDebugInterface();
		// catch (...)
		if (FAILED(hr))
			OutputDebugString(_T("Debug interface not available\r\n"));
		hr = S_FALSE;
	}
	if (SUCCEEDED(hr))
	{
		m_pParser = m_pEngine;
		if (m_pParser)
		{
			hr = m_pParser->InitNew();
			if (SUCCEEDED(hr))
				hr = m_pEngine->SetScriptSite(this);				
		}
		else
			hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT ScriptSite::Uninitialize()
{
	HRESULT hr = S_FALSE;

	if (m_pEngine)
	{
		hr = m_pEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		hr = m_pEngine->InterruptScriptThread(SCRIPTTHREADID_ALL, NULL, 0 );
		hr = m_pEngine->Close();
		ATLASSERT(SUCCEEDED(hr));
		UninitializeDebugInterface();
	}

	return hr;
}

HRESULT ScriptSite::AddScript(LPCTSTR filePath, LPCTSTR scriptSrc)
{
	HRESULT hr = S_FALSE;
	DWORD_PTR sourceContext = 0;	

	hr = AddScriptFile(m_pEngine, filePath, scriptSrc, sourceContext);
	if (SUCCEEDED(hr))
	{
		hr = m_pParser->ParseScriptText(scriptSrc, NULL, NULL, NULL, sourceContext,
			                            0, SCRIPTTEXT_HOSTMANAGESSOURCE | SCRIPTTEXT_ISPERSISTENT, 
                                        NULL, NULL);
	}
	return hr;
}

STDMETHODIMP ScriptSite::GetLCID( LCID *plcid )
{
   return E_NOTIMPL;
}

STDMETHODIMP ScriptSite::GetDocVersionString(BSTR *pbstrVersionString)
{
   return E_NOTIMPL;
}

STDMETHODIMP ScriptSite::OnScriptTerminate(const VARIANT *pvarResult, 
                                    const EXCEPINFO *pexcepinfo)
{
   return S_OK;
}

STDMETHODIMP ScriptSite::OnStateChange( SCRIPTSTATE ssScriptState)
{
   return S_OK;
}

STDMETHODIMP ScriptSite::OnScriptError(IActiveScriptError* pase)
{
	return S_OK;
}

STDMETHODIMP ScriptSite::OnEnterScript(void)
{
	return S_OK;
}

STDMETHODIMP ScriptSite::OnLeaveScript(void)
{
	return S_OK;
}

STDMETHODIMP ScriptSite::GetWindow(HWND *phwnd)
{
	return E_FAIL;
}

STDMETHODIMP ScriptSite::EnableModeless(BOOL fEnable)
{
	return E_FAIL;
}

STDMETHODIMP ScriptSite::GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask,
      IUnknown **ppunkItem, ITypeInfo **ppTypeInfo)
{
   if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
   {
      if (!ppunkItem)
         return E_INVALIDARG;
      *ppunkItem = NULL;
   }
   if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
   {
		if (!ppTypeInfo)
			return E_INVALIDARG;
		*ppTypeInfo = NULL;
   }
	if (m_namedItems.Lookup(pstrName))
	{
		CComPtr<IUnknown> pIUnknown = m_namedItems[pstrName];
		if (pIUnknown != NULL)
		{
			pIUnknown.CopyTo(ppunkItem);
			return S_OK;
		}
	}
	return TYPE_E_ELEMENTNOTFOUND;
}

HRESULT ScriptSite::InvokeN(CString name, DISPPARAMS& params, VARIANT* retVal)
{
	 HRESULT hr;
	 CComPtr<IDispatch> spScript;

	 hr = m_pEngine->GetScriptDispatch(NULL, &spScript);

	 try 
	 {
		 if( SUCCEEDED(hr) && spScript )
		 {
			 CComBSTR scrName = name;
			 CComVariant vaLocalResult;
			 VARIANT* pParams = params.rgvarg;
			 hr = spScript.InvokeN((LPCOLESTR)scrName, pParams, params.cArgs, retVal ? retVal : &vaLocalResult);
		 }
	 }
	 catch (const CString& err)
	 {
#ifdef _DEBUG
		 MessageBox(0, err, name, 0);
#endif // _DEBUG
	 }

	 return S_OK;
 }

STDMETHODIMP ScriptSite::SetScriptState(SCRIPTSTATE ss)
{
	HRESULT hr = m_pEngine ? m_pEngine->SetScriptState(ss) : E_UNEXPECTED;
	if (SUCCEEDED(hr))
	{
		switch (ss)
		{
		case SCRIPTSTATE_UNINITIALIZED:
			hr = m_pEngine->SetScriptSite(this);
			break;
		}
	}
	return hr;
}

STDMETHODIMP ScriptSite::GetScriptState(SCRIPTSTATE& ss)
{
	HRESULT hr = m_pEngine ? m_pEngine->GetScriptState(&ss) : E_UNEXPECTED;
	return hr;
}
