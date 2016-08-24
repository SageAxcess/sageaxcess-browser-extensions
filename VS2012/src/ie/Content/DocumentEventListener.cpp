#include "stdafx.h"
#include "DocumentEventListener.h"
#include "DocumentEventListenerCallback.h"

DocumentEventListener::DocumentEventListener()
{
	
}

DocumentEventListener::~DocumentEventListener()
{
	DetachAll();
}

CComQIPtr<IHTMLWindow3> DocumentEventListener::GetWindowFromBrowser( CComQIPtr<IWebBrowser2> browser )
{
	HRESULT hr = E_FAIL;
	CComQIPtr<IHTMLWindow3> res;
	CComQIPtr<IHTMLDocument2> doc;
	CComQIPtr<IDispatch> docDisp;
	CComQIPtr<IHTMLWindow2> win;

	//get parent _window for current browser
	if (browser)
	{
		hr = browser->get_Document(&docDisp);
		if(SUCCEEDED(hr))
		{
			doc = docDisp;
			if (doc)
			{
				hr = doc->get_parentWindow(&win);
				if(SUCCEEDED(hr))
				{
					res = win;
				}
			}
		}
	}
	return res;
}

HRESULT DocumentEventListener::EventListener::Attach()
{	
	AddCallbacks();

	CComQIPtr<IDispatch> dispLoad = _onLoadDispatch;
	CComQIPtr<IDispatch> dispUnload = _onUnloadDispatch;

	VARIANT_BOOL res = VARIANT_FALSE;

	HRESULT hr = _window->attachEvent(CComBSTR(_T("onload")), dispLoad, &res);
	hr = _window->attachEvent(CComBSTR(_T("onunload")), dispUnload, &res);		
	
	return hr;
}


bool DocumentEventListener::IsAttached(CComPtr<IDispatch> browserDispatch)
{
	//check if we have already attached to this dispatch
	bool res = false;
	std::list<EventListener>::iterator it = _listeners.begin();
	for(it = _listeners.begin(); it != _listeners.end(); ++it)
	{
		EventListener &listener = *it;
		if(listener._browserDispatch.IsEqualObject(browserDispatch)) 
		{
			res = true;
			break;
		}
	}
	return res;
}

HRESULT DocumentEventListener::Attach(CComPtr<IDispatch> browserDispatch, ICallback *callback)
{
	HRESULT hr = S_FALSE;

	if(!IsAttached(browserDispatch))
	{
		EventListener listener;
		CComQIPtr<IWebBrowser2> browser = browserDispatch;
		listener._window = GetWindowFromBrowser(browser);
				
		VARIANT_BOOL res = VARIANT_FALSE;

		if(listener._window)
		{
			CComObject<DocumentEventListenerCallback>* loadPropertyback = 0;
			hr = CComObject<DocumentEventListenerCallback>::CreateInstance(&loadPropertyback);
			if(loadPropertyback) 
				loadPropertyback->AddRef();

			CComObject<DocumentEventListenerCallback>* unloadPropertyback = 0;
			hr = CComObject<DocumentEventListenerCallback>::CreateInstance(&unloadPropertyback);
			if(unloadPropertyback) 
				unloadPropertyback->AddRef();

			listener._onLoadDispatch.Attach(loadPropertyback);
			listener._onUnloadDispatch.Attach(unloadPropertyback);

			listener._callback = callback;
			listener._browserDispatch = browserDispatch;
		
			hr = listener.Attach();

			_listeners.push_back(listener);
		}
	}

	return hr;
}

HRESULT DocumentEventListener::EventListener::Detach()
{
	HRESULT hr = S_FALSE;
	if(_window)
	{
		RemoveCallbacks();

		CComQIPtr<IDispatch> dispLoad = _onLoadDispatch;
		CComQIPtr<IDispatch> dispUnload = _onUnloadDispatch;

		hr = _window->detachEvent(CComBSTR(_T("onload")), dispLoad);
		hr = _window->detachEvent(CComBSTR(_T("onunload")), dispUnload);
	}
	return hr;
}


HRESULT DocumentEventListener::Detach(CComPtr<IDispatch> browserDispatch)
{
	HRESULT hr = E_FAIL;
	std::list<EventListener>::iterator it = _listeners.begin();
	for(it = _listeners.begin(); it != _listeners.end(); ++it)
	{
		EventListener &listener = *it;
		if(listener._browserDispatch.IsEqualObject(browserDispatch))
		{
			HRESULT hr = listener.Detach();
			_listeners.erase(it);
			break;
		}		
	}
	
	return hr;
}

void DocumentEventListener::ReAttach()
{
	std::list<EventListener>::iterator it = _listeners.begin();
	for(it = _listeners.begin(); it != _listeners.end();)
	{		
		HRESULT hr = (*it).Detach();
		if(SUCCEEDED(hr))
		{
			hr = (*it).Attach();
		}
		
		if(FAILED(hr))
		{
			it = _listeners.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void DocumentEventListener::DetachAll()
{
	std::list<EventListener>::iterator it = _listeners.begin();
	for(it = _listeners.begin(); it != _listeners.end(); ++it)
	{		
		(*it).Detach();		
	}

	_listeners.clear();
}

void DocumentEventListener::EventListener::AddCallbacks()
{
	DocumentEventListenerCallback* load = (DocumentEventListenerCallback*)((IDispatch*)_onLoadDispatch);
	if(load) 
		load->AddCallback(_callback, DocumentEventListener::load, _browserDispatch);

	DocumentEventListenerCallback* unload = (DocumentEventListenerCallback*)((IDispatch*)_onUnloadDispatch);
	if(unload) 
		unload->AddCallback(_callback, DocumentEventListener::unload, _browserDispatch);
}


void DocumentEventListener::EventListener::RemoveCallbacks()
{
	DocumentEventListenerCallback* load = (DocumentEventListenerCallback*)((IDispatch*)_onLoadDispatch);
	if(load) 
		load->AddCallback(0, DocumentEventListener::load, _browserDispatch);

	DocumentEventListenerCallback* unload = (DocumentEventListenerCallback*)((IDispatch*)_onUnloadDispatch);
	if(unload) 
		unload->AddCallback(0, DocumentEventListener::unload, _browserDispatch);
}


