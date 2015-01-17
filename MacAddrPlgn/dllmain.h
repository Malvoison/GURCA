// dllmain.h : Declaration of module class.

class CMacAddrPlgnModule : public ATL::CAtlDllModuleT< CMacAddrPlgnModule >
{
public :
	DECLARE_LIBID(LIBID_MacAddrPlgnLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MACADDRPLGN, "{C7A47441-6AD7-4E81-BBF9-59A5D2923525}")
};

extern class CMacAddrPlgnModule _AtlModule;
