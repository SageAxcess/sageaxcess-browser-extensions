#pragma once

#include "activdbg.h"

class ATL_NO_VTABLE IActiveScriptSiteDebugImpl : public IActiveScriptSiteDebug
{
public:
	DWORD m_appCookie;
	CComPtr<IDebugApplication> m_debugApp;
	CComPtr<IProcessDebugManager> m_debugManager;
	CSimpleMap<DWORD_PTR, CComPtr<IDebugDocumentHelper> > m_debugDocHelpers;

public:
	IActiveScriptSiteDebugImpl() : m_appCookie(0xcdcdcdcd)
	{

	}

	~IActiveScriptSiteDebugImpl()
	{
		UninitializeDebugInterface();
	}

	HRESULT InitializeDebugInterface()
	{
		HRESULT hr = E_FAIL;

		hr = CoCreateInstance(CLSID_ProcessDebugManager,
			NULL,
			CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,
			IID_IProcessDebugManager,
			(void**)&m_debugManager);

		if (SUCCEEDED(hr))
			hr = m_debugManager->CreateApplication(&m_debugApp);
		if (SUCCEEDED(hr))
		{
			CString appName;
			appName.LoadString(IDS_PROJNAME);
			hr = m_debugApp->SetName(appName);
		}
		if (SUCCEEDED(hr))
			hr = m_debugManager->AddApplication(m_debugApp, &m_appCookie);
		if (FAILED(hr))
		{
			m_debugManager = NULL;
			m_debugApp = NULL;
			m_appCookie = 0xcdcdcdcd;
		}
		return hr;
	}

	void UninitializeDebugInterface()
	{
		if (m_debugApp)
		{
			if (m_debugManager)
			{
				ATLVERIFY(m_appCookie != 0xcdcdcdcd);
				m_debugManager->RemoveApplication(m_appCookie);
				m_appCookie = 0xfefefefe;
			}
		}
		m_debugDocHelpers.RemoveAll();
		m_debugManager = NULL;
		m_debugApp = NULL;
	}

	HRESULT AddScriptFile(IActiveScript* pas, LPCWSTR filePath, LPCWSTR scriptSrc, DWORD_PTR& sourceContext)
	{
		HRESULT hr = S_FALSE;

		if (!m_debugManager)
			return S_FALSE;

		CComPtr<IDebugDocumentHelper> debugDocHelper;
		{
			TCHAR drive[_MAX_DRIVE];
			TCHAR dir[_MAX_DIR];
			TCHAR fname[_MAX_FNAME] = L"";
			TCHAR ext[_MAX_EXT];
			TCHAR filePathNew[_MAX_PATH] = L"";

			_tsplitpath_s(filePath, drive, dir, fname, ext);
			_tcscat_s(fname, ext);

			// filePathNew
			_tcsncpy_s(filePathNew, filePath, ARRAYSIZE(filePathNew));
			// '/' -> '\'
			for (LPTSTR ppath = _tcschr(filePathNew, L'/'); ppath; ppath = _tcschr(ppath, L'/'))
				*ppath = L'\\';

			hr = m_debugManager->CreateDebugDocumentHelper(NULL, &debugDocHelper);
			if (SUCCEEDED(hr))
				hr = debugDocHelper->Init(m_debugApp, fname, filePathNew, TEXT_DOC_ATTR_READONLY);
		}
		if (SUCCEEDED(hr))
			hr = debugDocHelper->AddUnicodeText(scriptSrc);
		if (SUCCEEDED(hr))
			hr = debugDocHelper->DefineScriptBlock(0, wcslen(scriptSrc), pas, FALSE, &sourceContext);
		if (SUCCEEDED(hr))
			hr = debugDocHelper->Attach(NULL);
		if (SUCCEEDED(hr))
		{
			m_debugDocHelpers.Lookup(sourceContext);
			m_debugDocHelpers.Add(sourceContext, debugDocHelper);
		}
		return hr;
	}

	// IActiveScriptSiteDebug Implementation

	// Used by the language engine to delegate IDebugCodeContext::GetSourceContext. 
	STDMETHODIMP GetDocumentContextFromPosition(DWORD_PTR dwSourceContext,
		ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext **ppsc)
	{
		HRESULT hr = S_FALSE;
		CComPtr<IDebugDocumentHelper> debugDocHelper = m_debugDocHelpers.Lookup(dwSourceContext);
		if (NULL == debugDocHelper)
			hr = E_UNEXPECTED;
		if (SUCCEEDED(hr))
		{
			ULONG ulStartPos = 0;
			hr = debugDocHelper->GetScriptBlockInfo(dwSourceContext, NULL, &ulStartPos, NULL);
			if (SUCCEEDED(hr))
				hr = debugDocHelper->CreateDebugDocumentContext(ulStartPos + uCharacterOffset, uNumChars, ppsc);
		}
		return hr;
	}

	// Returns the debug application object associated with this script site. Provides 
	// a means for a smart host to define what application object each script belongs to. 
	// Script engines should attempt to call this method to get their containing application 
	// and resort to IProcessDebugManager::GetDefaultApplication if this fails. 
	STDMETHODIMP GetApplication(IDebugApplication **ppda)
	{
		HRESULT hr = E_UNEXPECTED;
		if (!ppda)
		{
			return E_INVALIDARG;
		}
		else if (m_debugApp)
		{
			hr = m_debugApp.CopyTo(ppda);
		}
		return hr;
	}

	// Gets the application node under which script documents should be added 
	// can return NULL if script documents should be top-level. 
	STDMETHODIMP GetRootApplicationNode(IDebugApplicationNode **ppdanRoot)
	{
		HRESULT hr = E_FAIL;
		if (!ppdanRoot)
		{
			hr = E_INVALIDARG;
		}
		else if (m_debugApp)
		{
			hr = m_debugApp->GetRootNode(ppdanRoot);
		}
		return hr;
	}

	// Allows a smart host to control the handling of runtime errors 
	STDMETHODIMP OnScriptErrorDebug(IActiveScriptErrorDebug* pErrorDebug, BOOL* pfEnterDebugger, BOOL* pfCallOnScriptErrorWhenContinuing)
	{
		if (pfEnterDebugger)
			*pfEnterDebugger = TRUE;
		if (pfCallOnScriptErrorWhenContinuing)
			*pfCallOnScriptErrorWhenContinuing = TRUE;
		return S_OK;
	}
};


