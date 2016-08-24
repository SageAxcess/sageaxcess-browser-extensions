#pragma once
#include <list>

class DocumentEventListenerCallback;

class DocumentEventListener
{
public:	
	enum EventType 
	{ 
		load, 
		unload, 
		focus,
		blur
	};

	class ICallback
	{
	public:	
		virtual HRESULT OnDocumentEvent(CComPtr<IDispatch> browserDispatch, EventType type, DISPPARAMS* pdispparams = NULL) = 0;
	};


	DocumentEventListener();
	virtual ~DocumentEventListener();

	void ReAttach();
	HRESULT Attach(CComPtr<IDispatch> browserDispatch, ICallback *callback);
	HRESULT Detach(CComPtr<IDispatch> browserDispatch);
	void DetachAll();
	bool IsAttached(CComPtr<IDispatch> browserDispatch);

private:	
	struct EventListener
	{
		CComPtr<IDispatch> _onLoadDispatch;
		CComPtr<IDispatch> _onUnloadDispatch;
		CComQIPtr<IHTMLWindow3> _window;
		CComPtr<IDispatch> _browserDispatch;
		ICallback* _callback;

		void AddCallbacks();
		void RemoveCallbacks();
		HRESULT Attach();
		HRESULT Detach();
	};
	
	CComQIPtr<IHTMLWindow3> GetWindowFromBrowser(CComQIPtr<IWebBrowser2> browser);

private:
	std::list<EventListener> _listeners;
};
