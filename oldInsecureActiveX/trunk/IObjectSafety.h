// Option bit definitions for IObjectSafety:
#define    INTERFACESAFE_FOR_UNTRUSTED_CALLER    0x00000001
        // Caller of interface may be untrusted.
#define    INTERFACESAFE_FOR_UNTRUSTED_DATA    0x00000002
        // Data passed into interface may be untrusted.

// {CB5BDC81-93C1-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IObjectSafety, 0xcb5bdc81, 0x93c1, 0x11cf, 0x8f, 0x20, 
    0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

interface IObjectSafety : public IUnknown
{
    public:
        virtual HRESULT __stdcall GetInterfaceSafetyOptions( 
            /* [in]  */ REFIID riid,
            /* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
            /* [out] */ DWORD __RPC_FAR *pdwEnabledOptions) = 0;

        virtual HRESULT __stdcall SetInterfaceSafetyOptions( 
            /* [in] */ REFIID riid,
            /* [in] */ DWORD dwOptionSetMask,
            /* [in] */ DWORD dwEnabledOptions) = 0;

};