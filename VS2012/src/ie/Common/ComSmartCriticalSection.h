#pragma once

class CComSmartAutoCriticalSection 
{ 
public: 
	CComSmartAutoCriticalSection() 
	{ 
		if (SUCCEEDED(::CoInitialize(NULL))) 
		{ 
			::CoUninitialize(); 

			_bInMTA = FALSE; 
		} 
		else 
		{ 
			_bInMTA = TRUE; 
		} 

		if (_bInMTA) 
			::InitializeCriticalSection(&_csSafeAccess); 
	} 

	~CComSmartAutoCriticalSection() 
	{ 
		if (_bInMTA) 
			::DeleteCriticalSection(&_csSafeAccess); 
	} 

	void Lock() 
	{ 
		if (_bInMTA) 
			::EnterCriticalSection(&_csSafeAccess); 
	} 

	void Unlock() 
	{ 
		if (_bInMTA) 
			::LeaveCriticalSection(&_csSafeAccess); 
	} 

private: 
	BOOL              _bInMTA; 
	CRITICAL_SECTION  _csSafeAccess; 
}; 