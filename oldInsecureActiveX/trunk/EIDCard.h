
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Mar 10 21:49:51 2007
 */
/* Compiler settings for C:\Projektid\SignActiveX\trunc\EIDCard.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __EIDCard_h__
#define __EIDCard_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISign_FWD_DEFINED__
#define __ISign_FWD_DEFINED__
typedef interface ISign ISign;
#endif 	/* __ISign_FWD_DEFINED__ */


#ifndef __Sign_FWD_DEFINED__
#define __Sign_FWD_DEFINED__

#ifdef __cplusplus
typedef class Sign Sign;
#else
typedef struct Sign Sign;
#endif /* __cplusplus */

#endif 	/* __Sign_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISign_INTERFACE_DEFINED__
#define __ISign_INTERFACE_DEFINED__

/* interface ISign */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISign;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E145248B-C030-4EF3-ABE9-79EAAE0AE8A5")
    ISign : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getInfo( 
            /* [retval][out] */ BSTR *infoText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getSigningCertificate( 
            /* [retval][out] */ BSTR *Certificate) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getSignedHash( 
            /* [in] */ BSTR sHash,
            /* [in] */ long selectedCert,
            /* [retval][out] */ BSTR *SignedHash) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_selectedCertNumber( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_selectedCertNumber( 
            /* [in] */ long newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISignVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISign * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISign * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISign * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISign * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISign * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISign * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISign * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getInfo )( 
            ISign * This,
            /* [retval][out] */ BSTR *infoText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getSigningCertificate )( 
            ISign * This,
            /* [retval][out] */ BSTR *Certificate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getSignedHash )( 
            ISign * This,
            /* [in] */ BSTR sHash,
            /* [in] */ long selectedCert,
            /* [retval][out] */ BSTR *SignedHash);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_selectedCertNumber )( 
            ISign * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_selectedCertNumber )( 
            ISign * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } ISignVtbl;

    interface ISign
    {
        CONST_VTBL struct ISignVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISign_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISign_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISign_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISign_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISign_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISign_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISign_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISign_getInfo(This,infoText)	\
    (This)->lpVtbl -> getInfo(This,infoText)

#define ISign_getSigningCertificate(This,Certificate)	\
    (This)->lpVtbl -> getSigningCertificate(This,Certificate)

#define ISign_getSignedHash(This,sHash,selectedCert,SignedHash)	\
    (This)->lpVtbl -> getSignedHash(This,sHash,selectedCert,SignedHash)

#define ISign_get_selectedCertNumber(This,pVal)	\
    (This)->lpVtbl -> get_selectedCertNumber(This,pVal)

#define ISign_put_selectedCertNumber(This,newVal)	\
    (This)->lpVtbl -> put_selectedCertNumber(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISign_getInfo_Proxy( 
    ISign * This,
    /* [retval][out] */ BSTR *infoText);


void __RPC_STUB ISign_getInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISign_getSigningCertificate_Proxy( 
    ISign * This,
    /* [retval][out] */ BSTR *Certificate);


void __RPC_STUB ISign_getSigningCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISign_getSignedHash_Proxy( 
    ISign * This,
    /* [in] */ BSTR sHash,
    /* [in] */ long selectedCert,
    /* [retval][out] */ BSTR *SignedHash);


void __RPC_STUB ISign_getSignedHash_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISign_get_selectedCertNumber_Proxy( 
    ISign * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISign_get_selectedCertNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISign_put_selectedCertNumber_Proxy( 
    ISign * This,
    /* [in] */ long newVal);


void __RPC_STUB ISign_put_selectedCertNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISign_INTERFACE_DEFINED__ */



#ifndef __EIDCARDLib_LIBRARY_DEFINED__
#define __EIDCARDLib_LIBRARY_DEFINED__

/* library EIDCARDLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_EIDCARDLib;

EXTERN_C const CLSID CLSID_Sign;

#ifdef __cplusplus

class DECLSPEC_UUID("FC5B7BD2-584A-4153-92D7-4C5840E4BC28")
Sign;
#endif
#endif /* __EIDCARDLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


