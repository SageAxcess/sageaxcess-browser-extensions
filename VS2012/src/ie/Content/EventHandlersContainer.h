#pragma once

class EventHandlersContainer
{
public:
	EventHandlersContainer();
	virtual ~EventHandlersContainer();

private:
	EventHandlersContainer(const EventHandlersContainer&);
	EventHandlersContainer& operator=(const EventHandlersContainer& r);

public:
	HRESULT SetDriver(const CString& eventName, IDispatch* driver);
	HRESULT RemoveDriver(const CString& eventName, IDispatch* driver);
	HRESULT RemoveAllDrivers();
	HRESULT InvokeDriver(const CString& eventName, DISPPARAMS* params);

private:
	CRBMultiMap<const CString, CComDispatchDriver> _eventHandlersMap;  
};