// dllmain.h : Declaration of module class.

class CEstEIDSigningPluginBHOModule : public CAtlDllModuleT< CEstEIDSigningPluginBHOModule >
{
public :
	DECLARE_LIBID(LIBID_EstEIDSigningPluginBHOLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ESTEIDSIGNINGPLUGINBHO, "{28F87E03-93E1-4B35-BADD-4462ED511892}")
};

extern class CEstEIDSigningPluginBHOModule _AtlModule;
