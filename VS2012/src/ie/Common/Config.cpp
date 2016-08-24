#include "StdAfx.h"
#include "Config.h"
#include <algorithm>
#include <functional>
#include <Utils.h>

Config* Config::_pInstance = NULL;

static void Release(ScriptData* script) { delete script; }

CComSmartAutoCriticalSection Config::_ctorLock;

class ScriptDataFile : public ScriptData
{
public:
	ScriptDataFile(LPCTSTR fileName) : _fileName(fileName), _isRoot(TRUE), _isFrame(FALSE)
	{

	}

	ScriptDataFile(LPCTSTR fileName, BOOL isRoot, BOOL isFrame) : _fileName(fileName), _isRoot(isRoot), _isFrame(isFrame)
	{

	}

	virtual ~ScriptDataFile()
	{

	}

	virtual CString GetScript() const
	{
		CString filePath = _fileName;

		if (filePath.Find(_T(":")) == -1)
			filePath = utils::GetProgPath() + filePath;
		CString tData = utils::GetFileData(filePath);

		return CString(tData);
	}

	virtual CString GetName() const
	{
		return _fileName;
	}

	virtual BOOL IsRoot() const
	{
		return _isRoot;
	}

	virtual BOOL IsFrame() const
	{
		return _isFrame;
	}

private:
	ScriptDataFile() : _isRoot(FALSE), _isFrame(FALSE)
	{

	}

private:
	CString _fileName;
	BOOL _isRoot;
	BOOL _isFrame;
};

Config::Config() : _loaded(FALSE)
{
	commonInterfaceName = _T("helper");
	version = _T("1.0.0");
	name = _T("SageAxcess");
	description = _T("");
}

Config::~Config()
{
	std::for_each(contentScripts.begin(), contentScripts.end(),
		std::ptr_fun(&Release));
}

HRESULT Config::Load()
{   
	if (TRUE == _loaded)
		return S_OK;

	contentScripts.push_back(new ScriptDataFile(L"json2.min.js", TRUE, TRUE));
	contentScripts.push_back(new ScriptDataFile(L"content.js", TRUE, FALSE));
	contentScripts.push_back(new ScriptDataFile(L"jquery-1.11.3.min.js", TRUE, TRUE));
	contentScripts.push_back(new ScriptDataFile(L"forms.js", TRUE, TRUE));

	_loaded = TRUE;

	return S_OK;
}

