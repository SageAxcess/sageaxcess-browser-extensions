#pragma once
#include "DocumentEventListener.h"

using namespace ATL;

// DocumentEventListenerCallback

class ATL_NO_VTABLE DocumentEventListenerCallback :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<IDispatch>
{
private:
	DocumentEventListener::ICallback* _callback;
	DocumentEventListener::EventType _param;
	CComPtr<IDispatch> _dispatch;

public:
	DocumentEventListenerCallback()
	{

	}

	virtual ~DocumentEventListenerCallback()
	{

	}

BEGIN_COM_MAP(DocumentEventListenerCallback)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, 
					  VARIANT* pvarResult, EXCEPINFO* pexcepinfo,  UINT* puArgErr)
	{
		//dispid == 0 => system want to invoke function
		if(dispidMember == DISPID_VALUE)
		{
			OnEvent(pdispparams);
			return S_OK;
		}
		else 
		{
			return _tih.Invoke((IDispatch*)this, dispidMember, riid, lcid,
						wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
		}
	}

	HRESULT FinalConstruct()
	{
		_callback = 0;
		_param = DocumentEventListener::load;
		return S_OK;
	}

	void FinalRelease()
	{
	
	}

public:

	void AddCallback(DocumentEventListener::ICallback* callback, DocumentEventListener::EventType param, CComPtr<IDispatch> dispatch)
	{
		_callback = callback;
		_param = param;
		_dispatch = dispatch;
	}

private:
	void OnEvent(DISPPARAMS* pdispparams = NULL)
	{
		//send callback
		if(_callback) 
		{
			_callback->OnDocumentEvent(_dispatch, _param, pdispparams);
		}
	}
};
