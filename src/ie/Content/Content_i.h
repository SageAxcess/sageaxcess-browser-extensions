

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Jan 11 21:55:58 2016
 */
/* Compiler settings for Content.idl:
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


#ifndef __Content_i_h__
#define __Content_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __BrowserHelper_FWD_DEFINED__
#define __BrowserHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrowserHelper BrowserHelper;
#else
typedef struct BrowserHelper BrowserHelper;
#endif /* __cplusplus */

#endif 	/* __BrowserHelper_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "CommonInterface.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ContentLib_LIBRARY_DEFINED__
#define __ContentLib_LIBRARY_DEFINED__

/* library ContentLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ContentLib;

EXTERN_C const CLSID CLSID_BrowserHelper;

#ifdef __cplusplus

class DECLSPEC_UUID("C8EDF43C-09F4-453B-8823-CB70E525031C")
BrowserHelper;
#endif
#endif /* __ContentLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


