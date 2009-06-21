// dllmain.h : Declaration of module class.

class CEsteidExtModule : public CAtlDllModuleT< CEsteidExtModule >
{
public :
	DECLARE_LIBID(LIBID_EsteidExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ESTEIDEXT, "{08BD50C2-0AFC-4ED0-BC78-78488EDF9E58}")
};

extern class CEsteidExtModule _AtlModule;
