

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Jan 11 21:55:42 2016
 */
/* Compiler settings for CommonInterface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __CommonInterface_h__
#define __CommonInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IContentHelper_FWD_DEFINED__
#define __IContentHelper_FWD_DEFINED__
typedef interface IContentHelper IContentHelper;
#endif 	/* __IContentHelper_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_CommonInterface_0000_0000 */
/* [local] */ 

#if ( _MSC_VER >= 1020 )
#pragma once
#endif


extern RPC_IF_HANDLE __MIDL_itf_CommonInterface_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_CommonInterface_0000_0000_v0_0_s_ifspec;


#ifndef __CommonInterfaceLib_LIBRARY_DEFINED__
#define __CommonInterfaceLib_LIBRARY_DEFINED__

/* library CommonInterfaceLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CommonInterfaceLib;

#ifndef __IContentHelper_INTERFACE_DEFINED__
#define __IContentHelper_INTERFACE_DEFINED__

/* interface IContentHelper */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IContentHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BE32B491-A125-4D8F-90F3-0D9EC9444444")
    IContentHelper : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getXMLHttpRequest( 
            /* [retval][out] */ IDispatch **ppRequest) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE addListener( 
            /* [in] */ BSTR action,
            /* [in] */ IDispatch *pCallback) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE removeListener( 
            /* [in] */ BSTR action,
            /* [in] */ IDispatch *pCallback) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE sendMessage( 
            /* [in] */ BSTR action,
            /* [defaultvalue][in] */ IDispatch *pData = 0,
            /* [defaultvalue][in] */ IDispatch *pCallback = 0) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE addUsernameMask( 
            /* [in] */ VARIANT *pMask) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getUsername( 
            /* [retval][out] */ VARIANT *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContentHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContentHelper * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContentHelper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContentHelper * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IContentHelper * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IContentHelper * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IContentHelper * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IContentHelper * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getXMLHttpRequest )( 
            IContentHelper * This,
            /* [retval][out] */ IDispatch **ppRequest);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *addListener )( 
            IContentHelper * This,
            /* [in] */ BSTR action,
            /* [in] */ IDispatch *pCallback);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *removeListener )( 
            IContentHelper * This,
            /* [in] */ BSTR action,
            /* [in] */ IDispatch *pCallback);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *sendMessage )( 
            IContentHelper * This,
            /* [in] */ BSTR action,
            /* [defaultvalue][in] */ IDispatch *pData,
            /* [defaultvalue][in] */ IDispatch *pCallback);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *addUsernameMask )( 
            IContentHelper * This,
            /* [in] */ VARIANT *pMask);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getUsername )( 
            IContentHelper * This,
            /* [retval][out] */ VARIANT *pValue);
        
        END_INTERFACE
    } IContentHelperVtbl;

    interface IContentHelper
    {
        CONST_VTBL struct IContentHelperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContentHelper_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IContentHelper_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IContentHelper_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IContentHelper_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IContentHelper_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IContentHelper_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IContentHelper_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IContentHelper_getXMLHttpRequest(This,ppRequest)	\
    ( (This)->lpVtbl -> getXMLHttpRequest(This,ppRequest) ) 

#define IContentHelper_addListener(This,action,pCallback)	\
    ( (This)->lpVtbl -> addListener(This,action,pCallback) ) 

#define IContentHelper_removeListener(This,action,pCallback)	\
    ( (This)->lpVtbl -> removeListener(This,action,pCallback) ) 

#define IContentHelper_sendMessage(This,action,pData,pCallback)	\
    ( (This)->lpVtbl -> sendMessage(This,action,pData,pCallback) ) 

#define IContentHelper_addUsernameMask(This,pMask)	\
    ( (This)->lpVtbl -> addUsernameMask(This,pMask) ) 

#define IContentHelper_getUsername(This,pValue)	\
    ( (This)->lpVtbl -> getUsername(This,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IContentHelper_INTERFACE_DEFINED__ */

#endif /* __CommonInterfaceLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


