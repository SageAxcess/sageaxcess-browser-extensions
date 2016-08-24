#pragma once

#include "resource.h"       // main symbols
#include "Content_i.h"
#include <exdispid.h>
#include "DocumentEventListenerCallback.h"
#include "DocumentEventListener.h"
#include "ScriptSite.h"
#include "TimerWindow.h"
#include "Config.h"
#include <Messages.h>
#include "ContentHelper.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// BrowserHelper
class BrowserHelper;
class ContentHelperContext;

typedef IDispEventImpl<1, BrowserHelper, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1> IWebBrowserEvents2;

DEFINE_GUID(CATID_AppContainerCompatible, 0x59fb2056, 0xd625, 0x48d0, 0xa9, 0x44, 0x1a, 0x85, 0xb5, 0xab, 0x26, 0x40);

class ATL_NO_VTABLE BrowserHelper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<BrowserHelper, &CLSID_BrowserHelper>,
	public IObjectWithSiteImpl<BrowserHelper>,
	public IDispatchImpl<IDispatch>,
	public DocumentEventListener::ICallback,
	public IWebBrowserEvents2
{
public:
	BrowserHelper();
	~BrowserHelper();

	DECLARE_REGISTRY_RESOURCEID(IDR_BrowserHelper)
	DECLARE_NOT_AGGREGATABLE(BrowserHelper)
	DECLARE_GET_CONTROLLING_UNKNOWN()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(BrowserHelper)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IObjectWithSite)
	END_COM_MAP()

	BEGIN_SINK_MAP(BrowserHelper)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_WINDOWSTATECHANGED, OnWindowStateChanged)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOWNLOADCOMPLETE, OnDownloadComplete)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentCompleteRegular)
	END_SINK_MAP()   

	BEGIN_CATEGORY_MAP(BrowserHelper)
		IMPLEMENTED_CATEGORY(CATID_AppContainerCompatible)
	END_CATEGORY_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, void **ppvSite);

	STDMETHOD(OnWindowStateChanged)(DWORD dwFlags, DWORD dwValidFlagsMask);
	STDMETHOD(OnDownloadComplete)();
	STDMETHOD(OnNavigateComplete2)(IDispatch * pdisp, VARIANT* vtURL);
	STDMETHOD(OnBeforeNavigate2)(IDispatch* pDisp, VARIANT* url, VARIANT* flags, VARIANT* tarGetFrameName, VARIANT* postData, VARIANT* headers, VARIANT_BOOL* cancel);
	STDMETHOD(OnDocumentCompleteRegular)(IDispatch* pDisp, VARIANT* pvarURL);

	//DocumentEventListener::ICallback
	HRESULT OnDocumentEvent(CComPtr<IDispatch> disp, DocumentEventListener::EventType type, DISPPARAMS* pdispparams = NULL);	

	STDMETHOD(OnDocumentComplete)(IDispatch *pDisp, VARIANT *pvarURL);
	STDMETHOD(StartScriptsInIframes)(IDispatch* pDisp);
	STDMETHOD(AddCommonInterface)();

private:
	CComQIPtr<IWebBrowser2> _spWebBrowser;	
	CComObject<ScriptSite>* _contentScriptSite;
	CComObjectCached<ContentHelper>* _pContentHelper;
	ContentHelperContext* _pContentHelperContext;
	DocumentEventListener _documentListener;
	CTimerWindow _timerWindow;
	BOOL _isScriptLoaded;
};

OBJECT_ENTRY_AUTO(__uuidof(BrowserHelper), BrowserHelper)
