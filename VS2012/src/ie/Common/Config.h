#pragma once

#include <vector>
#include "constants.h"
#include "Utils.h"
#include "ComSmartCriticalSection.h"

class ScriptData
{
public:
	ScriptData() {} ;
	virtual ~ScriptData() {}
	virtual CString GetScript() const = 0;
	virtual CString GetName() const = 0;
	virtual BOOL IsRoot() const = 0;
	virtual BOOL IsFrame() const = 0;
};

class Config
{
public:
	static Config* Instance()
	{
		if (!_pInstance)
		{
			_ctorLock.Lock();
			if(!_pInstance)
			{
				_pInstance = new Config();
				_pInstance->Load();
			}
			_ctorLock.Unlock();
		}
		return _pInstance;
	}

	static void Drop()
	{
		if (_pInstance)
		{
			delete _pInstance;
			_pInstance = NULL;
		}
	}

private:
	Config();
	~Config();
	Config& operator=(const Config& rhs);
	explicit Config(const Config& src);

private:
    HRESULT Load();

public:	
	CString version;
	CString name;
    CString description;
	CString commonInterfaceName;

    std::vector<ScriptData*> contentScripts;

private:
	BOOL _loaded;
	static Config* _pInstance;
	static CComSmartAutoCriticalSection _ctorLock;
};



