// ContentHelper.cpp : Implementation of IContentHelper

#include "stdafx.h"
#include "ContentHelper.h"
#include "TimerWindow.h"
#include <Registry.h>
#include <Utils.h>
#include "Content_i.h"
#include <shlguid.h>
#include <constants.h>
#include <Messages.h>
#include <Config.h>

ContentHelper::ContentHelper() 
{

}

ContentHelper::~ContentHelper()  
{

}

HRESULT ContentHelper::FinalConstruct()
{
	return S_OK;
}

void ContentHelper::FinalRelease()
{

}

HRESULT ContentHelper::ctor(CComQIPtr<IWebBrowser2>& pWebBrowser)
{
	_pWebBrowser = pWebBrowser;
	return S_OK;
}

STDMETHODIMP ContentHelper::getXMLHttpRequest(IDispatch** request)
{
	HRESULT hr = E_FAIL;
	CComPtr<IXMLHttpRequest> pRequest;
	hr = pRequest.CoCreateInstance(L"MSXML2.XMLHTTP");
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IDispatch> pDisp;
		pDisp = pRequest;
		pDisp.CopyTo(request);
	}
	return hr;
}

STDMETHODIMP ContentHelper::addListener(BSTR action, IDispatch* pDispCallback)
{
	return SetDriver(action, pDispCallback);
}

STDMETHODIMP ContentHelper::removeListener(BSTR action, IDispatch* pDispCallback)
{
	return RemoveDriver(action, pDispCallback);
}

STDMETHODIMP ContentHelper::sendMessage(BSTR action, IDispatch* data, IDispatch* pDispCallback)
{
	CComVariant vars[2];
	vars[0] = pDispCallback;
	vars[0].vt = VT_DISPATCH;
	vars[1] = data;
	vars[1].vt = VT_NULL;
	if (data)
		vars[1].vt = VT_DISPATCH;
	DISPPARAMS params = { vars, 0, 2, 0 };

	return InvokeDriver(action, &params);
}

STDMETHODIMP ContentHelper::SendMessage(BSTR action, IDispatch* param)
{
	CComVariant vars[1];
	vars[0] = param;
	vars[0].vt = VT_DISPATCH;
	DISPPARAMS params = { vars, 0, 1, 0 };

	return InvokeDriver(action, &params);
}

STDMETHODIMP ContentHelper::addUsernameMask(VARIANT* pMask)
{
	HRESULT hr = E_FAIL;

	if (VT_DISPATCH == pMask->vt)
	{
		CComQIPtr<IDispatchEx> pObjectDispatch(pMask->pdispVal);

		DISPID dispId = DISPID_STARTENUM;

		while (pObjectDispatch && NOERROR == pObjectDispatch->GetNextDispID(fdexEnumDefault, dispId, &dispId))
		{
			CComBSTR bsPropertyName;
			hr = pObjectDispatch->GetMemberName(dispId, &bsPropertyName);

			if (FAILED(hr))
				continue;

			VARIANT vPropertyValue;
			VariantInit(&vPropertyValue);

			DISPPARAMS dispParams = { 0 };

			hr = pObjectDispatch->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, &vPropertyValue, NULL, NULL);

			if (SUCCEEDED(hr) && vPropertyValue.vt == VT_BSTR)
			{
				CString mask(vPropertyValue.bstrVal);
				mask.MakeLower();
				if (_masks.Find(mask) == -1)
					_masks.Add(mask);
			}

			VariantClear(&vPropertyValue);
		}
	}
	else if (VT_BSTR == pMask->vt)
	{
		CString mask(pMask->bstrVal);
		mask.MakeLower();
		if (_masks.Find(mask) == -1)
			_masks.Add(mask);
	}

	return hr;
}

STDMETHODIMP ContentHelper::getUsername(VARIANT* pValue)
{
	pValue->vt = VT_BSTR;
	pValue->bstrVal = ::SysAllocString(_lastFoundUsername);
	return S_OK;
}

STDMETHODIMP ContentHelper::ProcessPostData(BSTR data)
{
	CString postData(data);
	int tokenPos = 0;
	CString token;

	token = postData.Tokenize(_T("\r\n"), tokenPos);
	BOOL nextLineWillContainsUsername = FALSE;
	while (token != _T(""))
	{
		if (nextLineWillContainsUsername)
		{
			nextLineWillContainsUsername = FALSE;
			_lastFoundUsername = token.Trim();
		}
		if (token.Find(_T("Content-Disposition: form-data; name=")) != -1)
		{
			CString name = token.Mid(_tcslen(_T("Content-Disposition: form-data; name=")));
			name.Replace(_T("\""), _T(""));
			name.MakeLower();
			for (int i = 0; i < _masks.GetSize(); i++)
			{
				if (name == _masks[i])
					nextLineWillContainsUsername = TRUE;
			}
		}
		token = postData.Tokenize(_T("\r\n"), tokenPos);
	}

	return S_OK;
}

STDMETHODIMP ContentHelper::Reset()
{	
	_masks.RemoveAll();
	_lastFoundUsername = _T("");
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

const BSTR ContentHelperContext::_functions[] = { L"setTimeout", L"setInterval", L"clearTimeout", L"clearInterval" };

ContentHelperContext::ContentHelperContext() : _p(NULL), 
_pTimerWindow(NULL)
{
	for (int dispId = 0; dispId < ARRAYSIZE(_dispIds); ++dispId)
	{
		_dispIds[dispId] = DISPID_UNKNOWN;
	}
}

ContentHelperContext::~ContentHelperContext()
{
	_pTimerWindow = NULL;
	_spDispatchEx.Detach();
	_p = NULL;
}

STDMETHODIMP ContentHelperContext::put_Window(IHTMLWindow2* p)
{
	_count = 0;
	AtlComPtrAssign((IUnknown**)&_p, p);
	_spDispatchEx = p;
	return S_OK;
}

STDMETHODIMP ContentHelperContext::put_TimerWindow(CTimerWindow* newVal)
{
	_pTimerWindow = newVal;
	return S_OK;
}

STDMETHODIMP_(ULONG) ContentHelperContext::AddRef()
{
	InterlockedIncrement(&_count);
	return (_p ? _p->AddRef() : S_OK);
}

STDMETHODIMP_(ULONG) ContentHelperContext::Release()
{
	if (_count == 0)
	{
		_p = 0;
		return S_OK;
	}
	InterlockedDecrement(&_count);
	return (_p ? _p->Release() : S_OK);
}

STDMETHODIMP ContentHelperContext::QueryInterface(REFIID iid, void** pp)
{
	if (IsEqualIID(iid, IID_IUnknown))
	{
		*pp = static_cast<IDispatchEx*>(this);
		AddRef();
		return S_OK;
	}
#if 0
	else if (IsEqualIID(iid, IID_IHTMLWindow2))
	{
		*pp = static_cast<IHTMLWindow2*>(this);
		AddRef();
		return S_OK;
	}
#endif
	else if (IsEqualIID(iid, IID_IDispatch))
	{
		*pp = static_cast<IDispatchEx*>(this);
		AddRef();
		return S_OK;
	}
	else if (IsEqualIID(iid, IID_IDispatchEx))
	{
		*pp = static_cast<IDispatchEx*>(this);
		AddRef();
		return S_OK;
	}
	return (_p ? _p->QueryInterface(iid, pp) : E_NOINTERFACE);
}

STDMETHODIMP ContentHelperContext::GetDispID(__RPC__in BSTR bstrName, DWORD grfdex, __RPC__out DISPID *pid)
{
	CComBSTR newName = bstrName;
	if (newName == _T("$") || newName == _T("jQuery"))
	{
		newName += _T("Safe");
	}

	HRESULT hr = _spDispatchEx ? _spDispatchEx->GetDispID(newName, grfdex, pid) : E_FAIL;
	if (SUCCEEDED(hr))
	{
		for (int dispId = 0; dispId < ARRAYSIZE(_dispIds); dispId++)
		{
			if (0 == wcscmp(newName, _functions[dispId]))
			{
				_dispIds[dispId] = *pid;
				break;
			}
		}
	}
	return hr;
}

STDMETHODIMP ContentHelperContext::InvokeEx(__in  DISPID id, __in  LCID lcid, __in  WORD wFlags, __in  DISPPARAMS *pdp, __out_opt  VARIANT *pvarRes, __out_opt  EXCEPINFO *pei, __in_opt  IServiceProvider *pspCaller)
{
	for (int dispId = 0; dispId < ARRAYSIZE(_dispIds); dispId++)
	{
		if ((DISPID_UNKNOWN != _dispIds[dispId]) && (id == _dispIds[dispId]))
		{
			switch (dispId)
			{
			case fnSetTimeout:
				if (pdp && 1 == pdp->cArgs && VT_DISPATCH == pdp->rgvarg[0].vt)
				{
					if (pvarRes)
						pvarRes->vt = VT_I4;
					return _pTimerWindow->SetTimer(pdp->rgvarg[0].pdispVal,
						1, pvarRes ? &pvarRes->lVal : NULL, FALSE);
				}
			case fnSetInterval:
				if (pdp && 2 == pdp->cArgs && VT_DISPATCH == pdp->rgvarg[1].vt && VT_I4 == pdp->rgvarg[0].vt)
				{
					if (pvarRes)
						pvarRes->vt = VT_I4;
					return _pTimerWindow->SetTimer(pdp->rgvarg[1].pdispVal,
						pdp->rgvarg[0].lVal, pvarRes ? &pvarRes->lVal : NULL,
						fnSetInterval == dispId);
				}
				break;
			case fnClearTimeout:
			case fnClearInterval:
				if (pdp && 1 == pdp->cArgs && VT_I4 == pdp->rgvarg[0].vt)
					return _pTimerWindow->RemoveTimer(pdp->rgvarg[0].lVal);
			default:
				break;
			}
		}
	}
	return _spDispatchEx ? _spDispatchEx->InvokeEx(id, lcid, wFlags, pdp, pvarRes, pei, pspCaller) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::DeleteMemberByName(__RPC__in BSTR bstrName, DWORD grfdex)
{
	return _spDispatchEx ? _spDispatchEx->DeleteMemberByName(bstrName, grfdex) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::DeleteMemberByDispID(DISPID id)
{
	return _spDispatchEx ? _spDispatchEx->DeleteMemberByDispID(id) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::GetMemberProperties(DISPID id, DWORD grfdexFetch, __RPC__out DWORD *pgrfdex)
{
	return _spDispatchEx ? _spDispatchEx->GetMemberProperties(id, grfdexFetch, pgrfdex) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::GetMemberName(DISPID id, __RPC__deref_out_opt BSTR *pbstrName)
{
	return _spDispatchEx ? _spDispatchEx->GetMemberName(id, pbstrName) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::GetNextDispID(DWORD grfdex, DISPID id, __RPC__out DISPID *pid)
{
	return _spDispatchEx ? _spDispatchEx->GetNextDispID(grfdex, id, pid) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::GetNameSpaceParent(__RPC__deref_out_opt IUnknown **ppunk)
{
	return _spDispatchEx ? _spDispatchEx->GetNameSpaceParent(ppunk) : E_FAIL;
}

STDMETHODIMP ContentHelperContext::GetTypeInfoCount(__RPC__out UINT *pctinfo)
{
	return IDispatchImpl<IDispatch, &IID_IHTMLWindow2, &LIBID_MSHTML, 4, 0>::GetTypeInfoCount(pctinfo);
}

STDMETHODIMP ContentHelperContext::GetTypeInfo(UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo)
{
	return IDispatchImpl<IDispatch, &IID_IHTMLWindow2, &LIBID_MSHTML, 4, 0>::GetTypeInfo(iTInfo, lcid, ppTInfo);
}

STDMETHODIMP ContentHelperContext::GetIDsOfNames(__RPC__in REFIID riid, __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, __RPC__in_range(0, 16384) UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId)
{
	return IDispatchImpl<IDispatch, &IID_IHTMLWindow2, &LIBID_MSHTML, 4, 0>::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

STDMETHODIMP ContentHelperContext::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	return IDispatchImpl<IDispatch, &IID_IHTMLWindow2, &LIBID_MSHTML, 4, 0>::Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}