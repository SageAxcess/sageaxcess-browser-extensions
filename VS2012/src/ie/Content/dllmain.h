// dllmain.h : Declaration of module class.

class Config;

class CContentModule : public CAtlDllModuleT<CContentModule>
{
	typedef CAtlDllModuleT<CContentModule> baseClass;
	Config* m_pConfig;

private:
	HRESULT AddConfigReplacements(IRegistrarBase* pRegistrar, Config* pConfig);

public :
	DECLARE_LIBID(LIBID_ContentLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_Content, "{AB96F519-1B1A-4731-AA5C-59F9A3E2AFB6}")

	CContentModule() : m_pConfig(NULL)
	{
	}

	void SetRegistrationConfig(Config* pConfig)
	{
		m_pConfig = pConfig;
	}

	virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* pRegistrar) throw()
	{
		AddConfigReplacements(pRegistrar, m_pConfig);
		return baseClass::AddCommonRGSReplacements(pRegistrar);
	}
};

extern class CContentModule _AtlModule;
extern HINSTANCE g_hInstance;