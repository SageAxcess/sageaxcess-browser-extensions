#include "stdafx.h"
#include "BrowserHelper.h"
#include "ContentHelper.h"
#include <Utils.h>
#include "TimerWindow.h"
#include <constants.h>
#include <shlguid.h>
#include <comdef.h>
#include <Registry.h>

HRESULT BrowserHelper::FinalConstruct()
{
#ifdef _DEBUG
	::OutputDebugString(_T("BrowserHelper::FinalConstruct\r\n"));
#endif
	ATLVERIFY(NULL == _pContentHelperContext);
	_pContentHelperContext = new ContentHelperContext();
	_pContentHelperContext->AddRef();
	_pContentHelper = NULL;
	_timerWindow.SetTimer(0, 0, 0, 0);
	return S_OK;
}

void BrowserHelper::FinalRelease()
{
#ifdef _DEBUG
	::OutputDebugString(_T("BrowserHelper::FinalRelease\r\n"));
#endif
	if (_pContentHelperContext)
	{
		_pContentHelperContext->Release();
		delete _pContentHelperContext;
		_pContentHelperContext = NULL;
	}
	if (_pContentHelper)
	{
		_pContentHelper->Release();
		_pContentHelper = NULL;
	}
	if (_timerWindow.IsWindow())
	{
		_timerWindow.DestroyWindow();
	}

	Config::Drop();
}

BrowserHelper::BrowserHelper() : _pContentHelperContext(NULL),
_contentScriptSite(NULL),
_pContentHelper(NULL),
_isScriptLoaded(FALSE)
{

}

BrowserHelper::~BrowserHelper() 
{
	FinalRelease();
}

STDMETHODIMP BrowserHelper::SetSite(IUnknown *pUnkSite)
{
	Lock();

	if (pUnkSite != NULL)
	{
		CreateMutex(NULL, FALSE, INSTALLER_MUTEX_NAME);

		// Cache the pointer to IWebBrowser2
		HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void**)&_spWebBrowser);

		if (SUCCEEDED(hr))
		{
			// Register to sink events from DWebBrowserEvents2.
			hr = IWebBrowserEvents2::DispEventAdvise(_spWebBrowser, &DIID_DWebBrowserEvents2);
		}

#ifdef _DEBUG
		//::MessageBox(0, _T("BrowserHelper::SetSite"), _T("BrowserHelper"), 0);
#endif // _DEBUG
		
		if (!_pContentHelper)
		{
			hr = CComObjectCached<ContentHelper>::CreateInstance(&_pContentHelper);

			if (SUCCEEDED(hr))
				_pContentHelper->AddRef();
		}

		if (SUCCEEDED(hr))
		{
			_pContentHelper->ctor(_spWebBrowser);
		}

		hr = CComObject<ScriptSite>::CreateInstance(&_contentScriptSite);
		ATLASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			_contentScriptSite->AddRef();
			hr = _contentScriptSite->Initialize();
			hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_UNINITIALIZED);
		}

		ATLASSERT(SUCCEEDED(hr));
	}
	else
	{
		HRESULT hr;

		if (_contentScriptSite)
			hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_UNINITIALIZED);
		if (_pContentHelper)
		{
			hr = _pContentHelper->RemoveAllDrivers();
		}
		ATLASSERT(SUCCEEDED(hr));

		hr = _timerWindow.Stop();
		ATLASSERT(SUCCEEDED(hr));

		// Another way will access to 'window' object in destructor and may cause crash
		_documentListener.DetachAll();

		// Unregister event sink
		hr = IWebBrowserEvents2::DispEventUnadvise(_spWebBrowser, &DIID_DWebBrowserEvents2);
		if (hr == CONNECT_E_NOCONNECTION)
			hr = S_OK;
		ATLASSERT(SUCCEEDED(hr));

		// Release cached pointers and other resources here
		if (_contentScriptSite)
		{
			hr = _contentScriptSite->Uninitialize();
			ATLASSERT(SUCCEEDED(hr));
			_contentScriptSite->Release();
			_contentScriptSite = NULL;
		}

		// Release helper object
		if (_pContentHelper)
		{
			_pContentHelper->Release();
			_pContentHelper = NULL;
		}
	}
	Unlock();

	return IObjectWithSiteImpl<BrowserHelper>::SetSite(pUnkSite);
}

STDMETHODIMP BrowserHelper::AddCommonInterface()
{
	CComQIPtr<IHTMLWindow2> spWindow = utils::IWebBrowserToIHTMLWindow(_spWebBrowser);
	CComPtr<IHTMLDocument2> spDocument;
	HRESULT hr = S_OK;
	if (!spWindow)
		hr = E_FAIL;
	if (SUCCEEDED(hr))
	{
		_pContentHelperContext->put_Window(spWindow);
		_pContentHelperContext->put_TimerWindow(&_timerWindow);
		CComPtr<IUnknown> spExposedContext;
		_pContentHelperContext->QueryInterface(IID_IUnknown, (void**)&spExposedContext);
		_contentScriptSite->AddNamedItem(L"window", 
			SCRIPTITEM_GLOBALMEMBERS | SCRIPTITEM_ISSOURCE | SCRIPTITEM_ISVISIBLE,
			spExposedContext);
	}
	if (SUCCEEDED(hr))
	{
		_contentScriptSite->AddNamedItem(Config::Instance()->commonInterfaceName,
			SCRIPTITEM_ISSOURCE | SCRIPTITEM_ISVISIBLE, (IDispatchImpl<IContentHelper, &IID_IContentHelper, &LIBID_CommonInterfaceLib, /*wMajor =*/ 1, /*wMinor =*/ 0>*)_pContentHelper);
	}

	return hr;
}

STDMETHODIMP BrowserHelper::OnWindowStateChanged(DWORD dwFlags, DWORD dwValidFlagsMask)
{
#ifdef _DEBUG
	OutputDebugString(_T("OnWindowStateChanged\r\n"));
#endif
	if (dwValidFlagsMask & OLECMDIDF_WINDOWSTATE_USERVISIBLE &&
		(dwValidFlagsMask & OLECMDIDF_WINDOWSTATE_ENABLED) && (dwFlags & OLECMDIDF_WINDOWSTATE_ENABLED))
	{
		Lock();
		BOOL isTabActive = FALSE;
		if (dwFlags & OLECMDIDF_WINDOWSTATE_USERVISIBLE) 
		{
			isTabActive = TRUE;
		} 
		else 
		{
			isTabActive = FALSE;
		}
		Unlock();
	} 
	return S_OK;
}

STDMETHODIMP BrowserHelper::OnDownloadComplete()
{
#ifdef _DEBUG
	::OutputDebugString(_T("BrowserHelper::OnDownloadComplete\r\n"));
#endif
	HRESULT hr = E_FAIL;
	Lock();
	CComQIPtr<IDispatch> pTemp = _spWebBrowser;
	if (_documentListener.IsAttached(pTemp))
		_documentListener.ReAttach();
	else
		_documentListener.Attach(pTemp, this);
	hr = S_OK;
	Unlock();
	return hr;
}

STDMETHODIMP BrowserHelper::OnNavigateComplete2(IDispatch* pDisp, VARIANT* vtURL)
{
	CComQIPtr<IWebBrowser2> pWebBrowser = pDisp;
	if (pWebBrowser == NULL)
		return S_OK;

	CComQIPtr<IDispatch> pTemp = _spWebBrowser;
	if (_documentListener.IsAttached(pTemp))
		_documentListener.ReAttach();
	else
		_documentListener.Attach(pTemp, this);

	return S_OK;
}

STDMETHODIMP BrowserHelper::OnDocumentComplete(IDispatch* pDisp, VARIANT* pvarURL)
{
#ifdef _DEBUG
	OutputDebugString(_T("BrowserHelper::OnDocumentComplete\r\n"));
#endif
	HRESULT hr = E_FAIL;
	Lock();

	if (_spWebBrowser.IsEqualObject(pDisp))
	{
		// "access denied" fix (middle button click throws access denied)
		CComPtr<IDispatch> pDocDisp;
		_spWebBrowser->get_Document(&pDocDisp);
		CComQIPtr<IHTMLDocument2> pHTMLDocument2 = pDocDisp;
		if (pHTMLDocument2)
		{
			CComPtr<IHTMLLocation> pLocation;
			pHTMLDocument2->get_location(&pLocation);
			if (pLocation)
			{
				CComBSTR str;
				if (E_ACCESSDENIED == pLocation->get_href(&str))
				{
					Unlock();
					return S_FALSE;
				}
				if (CString(str).Find(_T("about")) == 0)
				{
					Unlock();
					return S_FALSE;
				}
			}
		}
		hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_UNINITIALIZED);
		if (_pContentHelper)
		{
			hr = _pContentHelper->RemoveAllDrivers();
		}
		if (_pContentHelperContext)
			_pContentHelperContext->put_Window(utils::IWebBrowserToIHTMLWindow(_spWebBrowser));
		hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_INITIALIZED);
		hr = AddCommonInterface();
		if (SUCCEEDED(hr))
		{
			if (_isScriptLoaded == FALSE)
			{
				for (size_t i = 0; i < Config::Instance()->contentScripts.size(); ++i)
				{
					ScriptData* script = Config::Instance()->contentScripts[i];
					if (script->IsRoot())
						ATLVERIFY(SUCCEEDED(_contentScriptSite->AddScript(script->GetName(), script->GetScript())));
				}
				_isScriptLoaded = TRUE;
			}
			hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_CONNECTED);
			StartScriptsInIframes(_spWebBrowser);
		}
	}

	Unlock();	
	return hr;
}

STDMETHODIMP BrowserHelper::StartScriptsInIframes(IDispatch* pDisp)
{
	HRESULT hr = S_FALSE;

	CComQIPtr<IWebBrowser2> pBrowser = pDisp;

	if (pBrowser) {
		CComPtr<IDispatch> pDocDisp;
		hr = pBrowser->get_Document(&pDocDisp);
		if (pDocDisp)
		{
			CComQIPtr<IOleContainer> pContainer = pDocDisp;
			if (pContainer)
			{
				CComQIPtr<IEnumUnknown> pEnumerator;
				hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
				if (SUCCEEDED(hr) && pEnumerator)
				{
					CComPtr<IUnknown> pUnk;
					ULONG uFetched;

					for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
					{
						CComQIPtr<IWebBrowser2> pFrame = pUnk;
						if (pFrame == NULL)
						{
							pUnk = NULL;
							continue;
						}

						CComQIPtr<IDispatch> frameDispatch;
						if (FAILED(pFrame->get_Document(&frameDispatch)) || frameDispatch == NULL)
						{
							pUnk = NULL;
							continue;
						}

						CComQIPtr<IHTMLDocument2> htmlDocument2 = frameDispatch;
						if (htmlDocument2 == NULL)
						{
							pUnk = NULL;
							continue;
						}

						CComPtr<IHTMLWindow2> pWindow2;
						if (FAILED(htmlDocument2->get_parentWindow(&pWindow2)) || pWindow2 == NULL)
						{
							pUnk = NULL;
							continue;
						}

						hr = utils::ExposeObject(pFrame, Config::Instance()->commonInterfaceName, _pContentHelper);
						if (FAILED(hr))
						{
							pUnk = NULL;
							continue;
						}

						for (size_t i = 0; i < Config::Instance()->contentScripts.size(); ++i)
						{
							ScriptData* script = Config::Instance()->contentScripts[i];
							if (script->IsFrame())
							{
								CComVariant dummy;
								pWindow2->execScript(CComBSTR(script->GetScript()), L"JScript", &dummy);
							}
						}

						pUnk = NULL;
					}
				}
			}
		}
	}

	return S_OK;
}

HRESULT BrowserHelper::OnDocumentEvent(CComPtr<IDispatch> disp, DocumentEventListener::EventType type, DISPPARAMS* pdispparams)
{
	HRESULT hr = S_FALSE;
	if (type == DocumentEventListener::load)
	{
		CComQIPtr<IWebBrowser2> browser = disp;
		CComBSTR str;
		if (browser && SUCCEEDED(browser->get_LocationURL(&str)))
		{
			CComQIPtr<IDispatch> disp = browser;
			hr = OnDocumentComplete(disp, &CComVariant(str));
		}
	} 
	else if (type == DocumentEventListener::unload)
	{
		CComQIPtr<IWebBrowser2> browser = disp;
		CComBSTR str;
		if (browser && SUCCEEDED(browser->get_LocationURL(&str)))
		{
			CComQIPtr<IDispatch> disp = browser;
			Lock();
			if (_spWebBrowser.IsEqualObject(disp))
			{
				_documentListener.Detach(disp);
				hr = _contentScriptSite->SetScriptState(SCRIPTSTATE_UNINITIALIZED);
				ATLASSERT(SUCCEEDED(hr));
			}
			Unlock();
		}
	}
	return hr;
}

//---------------------------
//
//---------------------------
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="Language"
//
//
//
//russian
//
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="login"
//
//
//
//1
//
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="domain"
//
//
//
//ngs.ru
//
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="Username"
//
//
//
//qwerty
//
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="Password"
//
//
//
//123
//
//-----------------------------7e0387323f067c
//
//Content-Disposition: form-data; name="remember"
//
//
//
//1
//
//-----------------------------7e0387323f067c--
//
//
//---------------------------
//OK   
//---------------------------

STDMETHODIMP BrowserHelper::OnBeforeNavigate2(IDispatch* pDisp, 
	VARIANT* url, 
	VARIANT* flags, 
	VARIANT* tarGetFrameName, 
	VARIANT* postData, 
	VARIANT* headers, 
	VARIANT_BOOL* cancel)
{
	HRESULT hr = S_OK;
	Lock();
	if (postData && postData->vt == (VT_BYREF | VT_VARIANT))
	{
		if (postData->pvarVal->vt == (VT_ARRAY | VT_UI1))
		{
			SAFEARRAY* pSafeArray = postData->pvarVal->parray;
			if (pSafeArray)
			{
				if (pSafeArray->cDims == 1 && pSafeArray->cbElements == 1)
				{
					unsigned int nPostDataSize = pSafeArray->rgsabound[0].cElements * pSafeArray->cbElements; //in bytes
					void* pPostData = pSafeArray->pvData;
					if (_pContentHelper)
					{
						CStringA tmp = CStringA((char*)pPostData, nPostDataSize);
						_pContentHelper->ProcessPostData(CComBSTR(tmp.GetBuffer()));
					}
				}
			}

		}
	}
	Unlock();
    return hr;    
}

STDMETHODIMP BrowserHelper::OnDocumentCompleteRegular(IDispatch* pDisp, VARIANT* pvarURL)
{
	if (_spWebBrowser.IsEqualObject(pDisp))
	{
		if (!_documentListener.IsAttached(pDisp))
			_documentListener.Attach(pDisp, this);
	}
	return S_OK;
}

STDMETHODIMP BrowserHelper::GetSite(REFIID riid, void **ppvSite)
{
	return IObjectWithSiteImpl<BrowserHelper>::GetSite(riid, ppvSite);
}




