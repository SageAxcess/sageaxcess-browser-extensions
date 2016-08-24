#pragma once

class CTimerWindow;

#include "Content_i.h"
#include "EventHandlersContainer.h"

class ATL_NO_VTABLE ContentHelper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IContentHelper, &IID_IContentHelper, &LIBID_CommonInterfaceLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public EventHandlersContainer
{
public:
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(ContentHelper)
	DECLARE_GET_CONTROLLING_UNKNOWN()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(ContentHelper)
		COM_INTERFACE_ENTRY(IContentHelper)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	ContentHelper();
	virtual ~ContentHelper();

	HRESULT FinalConstruct();
	void FinalRelease();

	HRESULT ctor(CComQIPtr<IWebBrowser2>& pWebBrowser);

	STDMETHOD(getXMLHttpRequest)(IDispatch** ppRequest);
	STDMETHOD(addListener)(BSTR action, IDispatch* pCallback);
	STDMETHOD(removeListener)(BSTR action, IDispatch* pCallback);
	STDMETHOD(sendMessage)(BSTR action, IDispatch* pData, IDispatch* pCallback);
	STDMETHOD(addUsernameMask)(VARIANT* pMask);
	STDMETHOD(getUsername)(VARIANT* pValue);

	STDMETHOD(SendMessage)(BSTR action, IDispatch* param);
	STDMETHOD(ProcessPostData)(BSTR data);
	STDMETHOD(Reset)();

private:
	CComQIPtr<IWebBrowser2> _pWebBrowser;
	CSimpleArray<CString> _masks;
	CString _lastFoundUsername;
};

class ContentHelperContext :
	public IDispatchImpl<IDispatch, &IID_IHTMLWindow2, &LIBID_MSHTML, 4, 0>,
	public IDispatchEx
{
public:
	ContentHelperContext();
	virtual ~ContentHelperContext();

	STDMETHOD(put_Window)(IHTMLWindow2* p);
	STDMETHOD(put_TimerWindow)(CTimerWindow* newVal);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void** pp);

private:
	STDMETHOD(GetDispID)(__RPC__in BSTR bstrName, DWORD grfdex, __RPC__out DISPID *pid);
	STDMETHOD(InvokeEx)(__in  DISPID id, __in  LCID lcid, __in  WORD wFlags, __in  DISPPARAMS *pdp, __out_opt  VARIANT *pvarRes, __out_opt  EXCEPINFO *pei, __in_opt  IServiceProvider *pspCaller);
	STDMETHOD(DeleteMemberByName)(__RPC__in BSTR bstrName, DWORD grfdex);
	STDMETHOD(DeleteMemberByDispID)(DISPID id);
	STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, __RPC__out DWORD *pgrfdex);
	STDMETHOD(GetMemberName)(DISPID id, __RPC__deref_out_opt BSTR *pbstrName);
	STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, __RPC__out DISPID *pid);
	STDMETHOD(GetNameSpaceParent)(__RPC__deref_out_opt IUnknown **ppunk);

	STDMETHOD(GetTypeInfoCount)(__RPC__out UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(__RPC__in REFIID riid, __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, __RPC__in_range(0, 16384) UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

private:
	enum 
	{
		fnSetTimeout = 0,
		fnSetInterval,
		fnClearTimeout,
		fnClearInterval,
		fnTotal
	};

	DWORD _count;
	CTimerWindow* _pTimerWindow;
	IHTMLWindow2* _p;
	CComQIPtr<IDispatchEx> _spDispatchEx;
	DISPID _dispIds[fnTotal];
	static const BSTR _functions[];
};

