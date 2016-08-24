#include "stdafx.h"
#include "EventHandlersContainer.h"

EventHandlersContainer::EventHandlersContainer()
{

}

EventHandlersContainer::~EventHandlersContainer()
{

}

HRESULT EventHandlersContainer::SetDriver(const CString& eventName, IDispatch* driver)
{
	return NULL != _eventHandlersMap.Insert(eventName, driver) ? S_OK : S_FALSE;  
}

HRESULT EventHandlersContainer::InvokeDriver(const CString& eventName, DISPPARAMS* params)
{
	HRESULT hr = S_FALSE;

	POSITION posCur = _eventHandlersMap.FindFirstWithKey(eventName);
	while (posCur)
	{
		POSITION posNext = posCur;
		CComDispatchDriver spDispCallback = _eventHandlersMap.GetNextValueWithKey(posNext, eventName);

		ATLASSERT(spDispCallback != NULL);

		if (spDispCallback)
		{
			hr = spDispCallback->Invoke((DISPID) DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, 
				DISPATCH_METHOD, params, NULL, NULL, NULL);
			posCur = posNext;
		}
	}
	return hr;
}

HRESULT EventHandlersContainer::RemoveDriver(const CString& eventName, IDispatch* driver)
{
	HRESULT hr = S_FALSE;
	for (POSITION pos = _eventHandlersMap.FindFirstWithKey(eventName);
		pos;
		_eventHandlersMap.GetNextValueWithKey(pos, eventName))
	{
		if (_eventHandlersMap.GetValueAt(pos).IsEqualObject(driver))
		{
			_eventHandlersMap.RemoveAt(pos);
			hr = S_OK;
		}
	}		
	return hr;
}

HRESULT EventHandlersContainer::RemoveAllDrivers()
{
	_eventHandlersMap.RemoveAll();
	return S_OK;
}