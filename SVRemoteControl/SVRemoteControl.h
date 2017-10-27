

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Oct 26 12:26:40 2017
 */
/* Compiler settings for SVRemoteControl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


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

#ifndef __SVRemoteControl_h__
#define __SVRemoteControl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISVValueObject_FWD_DEFINED__
#define __ISVValueObject_FWD_DEFINED__
typedef interface ISVValueObject ISVValueObject;
#endif 	/* __ISVValueObject_FWD_DEFINED__ */


#ifndef __ISVValueObjectList_FWD_DEFINED__
#define __ISVValueObjectList_FWD_DEFINED__
typedef interface ISVValueObjectList ISVValueObjectList;
#endif 	/* __ISVValueObjectList_FWD_DEFINED__ */


#ifndef __ISVImageObject_FWD_DEFINED__
#define __ISVImageObject_FWD_DEFINED__
typedef interface ISVImageObject ISVImageObject;
#endif 	/* __ISVImageObject_FWD_DEFINED__ */


#ifndef __ISVImageObjectList_FWD_DEFINED__
#define __ISVImageObjectList_FWD_DEFINED__
typedef interface ISVImageObjectList ISVImageObjectList;
#endif 	/* __ISVImageObjectList_FWD_DEFINED__ */


#ifndef __ISVErrorObject_FWD_DEFINED__
#define __ISVErrorObject_FWD_DEFINED__
typedef interface ISVErrorObject ISVErrorObject;
#endif 	/* __ISVErrorObject_FWD_DEFINED__ */


#ifndef __ISVErrorObjectList_FWD_DEFINED__
#define __ISVErrorObjectList_FWD_DEFINED__
typedef interface ISVErrorObjectList ISVErrorObjectList;
#endif 	/* __ISVErrorObjectList_FWD_DEFINED__ */


#ifndef __ISVProductItems_FWD_DEFINED__
#define __ISVProductItems_FWD_DEFINED__
typedef interface ISVProductItems ISVProductItems;
#endif 	/* __ISVProductItems_FWD_DEFINED__ */


#ifndef __ISVDataDefObject_FWD_DEFINED__
#define __ISVDataDefObject_FWD_DEFINED__
typedef interface ISVDataDefObject ISVDataDefObject;
#endif 	/* __ISVDataDefObject_FWD_DEFINED__ */


#ifndef __ISVDataDefObjectList_FWD_DEFINED__
#define __ISVDataDefObjectList_FWD_DEFINED__
typedef interface ISVDataDefObjectList ISVDataDefObjectList;
#endif 	/* __ISVDataDefObjectList_FWD_DEFINED__ */


#ifndef __ISVInputMatchItem_FWD_DEFINED__
#define __ISVInputMatchItem_FWD_DEFINED__
typedef interface ISVInputMatchItem ISVInputMatchItem;
#endif 	/* __ISVInputMatchItem_FWD_DEFINED__ */


#ifndef __ISVInputMatchList_FWD_DEFINED__
#define __ISVInputMatchList_FWD_DEFINED__
typedef interface ISVInputMatchList ISVInputMatchList;
#endif 	/* __ISVInputMatchList_FWD_DEFINED__ */


#ifndef __ISVRemoteCtrl_FWD_DEFINED__
#define __ISVRemoteCtrl_FWD_DEFINED__
typedef interface ISVRemoteCtrl ISVRemoteCtrl;
#endif 	/* __ISVRemoteCtrl_FWD_DEFINED__ */


#ifndef __ISVInputBroker_FWD_DEFINED__
#define __ISVInputBroker_FWD_DEFINED__
typedef interface ISVInputBroker ISVInputBroker;
#endif 	/* __ISVInputBroker_FWD_DEFINED__ */


#ifndef __ISVOutputBroker_FWD_DEFINED__
#define __ISVOutputBroker_FWD_DEFINED__
typedef interface ISVOutputBroker ISVOutputBroker;
#endif 	/* __ISVOutputBroker_FWD_DEFINED__ */


#ifndef __ISVFailStatusStream_FWD_DEFINED__
#define __ISVFailStatusStream_FWD_DEFINED__
typedef interface ISVFailStatusStream ISVFailStatusStream;
#endif 	/* __ISVFailStatusStream_FWD_DEFINED__ */


#ifndef ___ISVFailStatusStreamEvents_FWD_DEFINED__
#define ___ISVFailStatusStreamEvents_FWD_DEFINED__
typedef interface _ISVFailStatusStreamEvents _ISVFailStatusStreamEvents;
#endif 	/* ___ISVFailStatusStreamEvents_FWD_DEFINED__ */


#ifndef ___ISVOutputBrokerEvents_FWD_DEFINED__
#define ___ISVOutputBrokerEvents_FWD_DEFINED__
typedef interface _ISVOutputBrokerEvents _ISVOutputBrokerEvents;
#endif 	/* ___ISVOutputBrokerEvents_FWD_DEFINED__ */


#ifndef ___ISVRemoteCtrlEvents_FWD_DEFINED__
#define ___ISVRemoteCtrlEvents_FWD_DEFINED__
typedef interface _ISVRemoteCtrlEvents _ISVRemoteCtrlEvents;
#endif 	/* ___ISVRemoteCtrlEvents_FWD_DEFINED__ */


#ifndef __SVRemoteCtrl_FWD_DEFINED__
#define __SVRemoteCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVRemoteCtrl SVRemoteCtrl;
#else
typedef struct SVRemoteCtrl SVRemoteCtrl;
#endif /* __cplusplus */

#endif 	/* __SVRemoteCtrl_FWD_DEFINED__ */


#ifndef __SVValueObject_FWD_DEFINED__
#define __SVValueObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVValueObject SVValueObject;
#else
typedef struct SVValueObject SVValueObject;
#endif /* __cplusplus */

#endif 	/* __SVValueObject_FWD_DEFINED__ */


#ifndef __SVValueObjectList_FWD_DEFINED__
#define __SVValueObjectList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVValueObjectList SVValueObjectList;
#else
typedef struct SVValueObjectList SVValueObjectList;
#endif /* __cplusplus */

#endif 	/* __SVValueObjectList_FWD_DEFINED__ */


#ifndef __SVErrorObject_FWD_DEFINED__
#define __SVErrorObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVErrorObject SVErrorObject;
#else
typedef struct SVErrorObject SVErrorObject;
#endif /* __cplusplus */

#endif 	/* __SVErrorObject_FWD_DEFINED__ */


#ifndef __SVErrorObjectList_FWD_DEFINED__
#define __SVErrorObjectList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVErrorObjectList SVErrorObjectList;
#else
typedef struct SVErrorObjectList SVErrorObjectList;
#endif /* __cplusplus */

#endif 	/* __SVErrorObjectList_FWD_DEFINED__ */


#ifndef __SVDataDefObject_FWD_DEFINED__
#define __SVDataDefObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVDataDefObject SVDataDefObject;
#else
typedef struct SVDataDefObject SVDataDefObject;
#endif /* __cplusplus */

#endif 	/* __SVDataDefObject_FWD_DEFINED__ */


#ifndef __SVDataDefObjectList_FWD_DEFINED__
#define __SVDataDefObjectList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVDataDefObjectList SVDataDefObjectList;
#else
typedef struct SVDataDefObjectList SVDataDefObjectList;
#endif /* __cplusplus */

#endif 	/* __SVDataDefObjectList_FWD_DEFINED__ */


#ifndef __SVProductItems_FWD_DEFINED__
#define __SVProductItems_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVProductItems SVProductItems;
#else
typedef struct SVProductItems SVProductItems;
#endif /* __cplusplus */

#endif 	/* __SVProductItems_FWD_DEFINED__ */


#ifndef __SVInputMatchItem_FWD_DEFINED__
#define __SVInputMatchItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVInputMatchItem SVInputMatchItem;
#else
typedef struct SVInputMatchItem SVInputMatchItem;
#endif /* __cplusplus */

#endif 	/* __SVInputMatchItem_FWD_DEFINED__ */


#ifndef __SVInputMatchList_FWD_DEFINED__
#define __SVInputMatchList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVInputMatchList SVInputMatchList;
#else
typedef struct SVInputMatchList SVInputMatchList;
#endif /* __cplusplus */

#endif 	/* __SVInputMatchList_FWD_DEFINED__ */


#ifndef __SVInputBroker_FWD_DEFINED__
#define __SVInputBroker_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVInputBroker SVInputBroker;
#else
typedef struct SVInputBroker SVInputBroker;
#endif /* __cplusplus */

#endif 	/* __SVInputBroker_FWD_DEFINED__ */


#ifndef __SVOutputBroker_FWD_DEFINED__
#define __SVOutputBroker_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVOutputBroker SVOutputBroker;
#else
typedef struct SVOutputBroker SVOutputBroker;
#endif /* __cplusplus */

#endif 	/* __SVOutputBroker_FWD_DEFINED__ */


#ifndef __SVFailStatusStream_FWD_DEFINED__
#define __SVFailStatusStream_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVFailStatusStream SVFailStatusStream;
#else
typedef struct SVFailStatusStream SVFailStatusStream;
#endif /* __cplusplus */

#endif 	/* __SVFailStatusStream_FWD_DEFINED__ */


#ifndef __SVImageObject_FWD_DEFINED__
#define __SVImageObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVImageObject SVImageObject;
#else
typedef struct SVImageObject SVImageObject;
#endif /* __cplusplus */

#endif 	/* __SVImageObject_FWD_DEFINED__ */


#ifndef __SVImageObjectList_FWD_DEFINED__
#define __SVImageObjectList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SVImageObjectList SVImageObjectList;
#else
typedef struct SVImageObjectList SVImageObjectList;
#endif /* __cplusplus */

#endif 	/* __SVImageObjectList_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_SVRemoteControl_0000_0000 */
/* [local] */ 

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("D5A134BC-7BC5-487e-B6D3-8600BB6E9368") 
enum SVObserverStates
    {	NoConfiguration	= 0,
	ConfigurationLoading	= 0x1,
	ConfigurationLoaded	= 0x2,
	OnlinePending	= 0x4,
	Online	= 0x8,
	RunningTest	= 0x10,
	Running	= 0x20,
	RegressionTest	= 0x40,
	SetupMode	= 0x80,
	SavingConfiguration	= 0x100,
	Stopping	= 0x200,
	RaidFailure	= 0x400,
	RemoteControlBlocked	= 0x1000
    } 	SVObserverStatesEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("2240AEA1-CB39-4bac-9B39-69DEF79041A7") 
enum SVObserverModes
    {	Available	= 0,
	RunMode	= 1,
	StopMode	= 2,
	RegressionMode	= 3,
	TestMode	= 4,
	EditMode	= 5,
	EditMoveMode	= 6,
	ModeChanging	= 100
    } 	SVObserverModesEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("A884D10E-A72F-44F1-B588-789D993B37AF") 
enum SVProductFilterModes
    {	LastInspected	= 1,
	LastReject	= 2
    } 	SVProductFilterModesEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("6B759CE1-04AD-4498-8DD4-5BEF00B36418") 
enum SVShutdownOptions
    {	Graceful	= 1,
	Restart	= 2
    } 	SVShutdownOptionsEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("E5330415-2690-437a-A3AA-21F58B62AFD4") 
enum SVImageFormats
    {	IDisp	= 0,
	BMP	= 1,
	PNG	= 2,
	JPEG	= 3,
	TIFF	= 4,
	lossy	= 0x10,
	fetch	= 0x20,
	Mask	= 0xf
    } 	SVImageFormatsEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("F92DBBFE-049D-459D-ADAB-F268B9B34DC6") 
enum SVNotificationTypes
    {	UnknownNotificationType	= 0,
	CurrentMode	= 1,
	LastModified	= 2,
	Connected	= 3,
	Disconnected	= 4,
	MessageNotification	= 5
    } 	SVNotificationTypesEnum;

typedef /* [helpstring][v1_enum][uuid] */  DECLSPEC_UUID("E28985D7-752A-4969-9444-7548DD3951E0") 
enum SVDataDefinitionListType
    {	UnknownDataDefinition	= 0,
	SelectedValues	= 1,
	SelectedImages	= 2,
	SelectedValuesAndSelectedImages	= 3,
	AllValues	= 4,
	AllValuesAndSelectedImages	= 6,
	AllImages	= 8,
	SelectedValuesAndAllImages	= 9,
	AllValuesAndAllImages	= 12
    } 	SVDataDefinitionListTypeEnum;



extern RPC_IF_HANDLE __MIDL_itf_SVRemoteControl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_SVRemoteControl_0000_0000_v0_0_s_ifspec;

#ifndef __ISVValueObject_INTERFACE_DEFINED__
#define __ISVValueObject_INTERFACE_DEFINED__

/* interface ISVValueObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVValueObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FC0896BF-1933-4776-9BE9-EB5927C75FFF")
    ISVValueObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerCount( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Status( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVValueObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVValueObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVValueObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVValueObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVValueObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVValueObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVValueObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVValueObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVValueObject * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVValueObject * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVValueObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISVValueObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISVValueObject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TriggerCount )( 
            ISVValueObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TriggerCount )( 
            ISVValueObject * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Status )( 
            ISVValueObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Status )( 
            ISVValueObject * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVValueObject * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVValueObject * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVValueObject * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } ISVValueObjectVtbl;

    interface ISVValueObject
    {
        CONST_VTBL struct ISVValueObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVValueObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVValueObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVValueObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVValueObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVValueObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVValueObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVValueObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVValueObject_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVValueObject_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVValueObject_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVValueObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISVValueObject_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISVValueObject_get_TriggerCount(This,pVal)	\
    ( (This)->lpVtbl -> get_TriggerCount(This,pVal) ) 

#define ISVValueObject_put_TriggerCount(This,newVal)	\
    ( (This)->lpVtbl -> put_TriggerCount(This,newVal) ) 

#define ISVValueObject_get_Status(This,pVal)	\
    ( (This)->lpVtbl -> get_Status(This,pVal) ) 

#define ISVValueObject_put_Status(This,newVal)	\
    ( (This)->lpVtbl -> put_Status(This,newVal) ) 

#define ISVValueObject_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVValueObject_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVValueObject_Add(This,Item)	\
    ( (This)->lpVtbl -> Add(This,Item) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVValueObject_INTERFACE_DEFINED__ */


#ifndef __ISVValueObjectList_INTERFACE_DEFINED__
#define __ISVValueObjectList_INTERFACE_DEFINED__

/* interface ISVValueObjectList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVValueObjectList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9F5BD020-120E-4224-BCC8-D0584273ED85")
    ISVValueObjectList : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISVValueObject *p_Item) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVValueObjectListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVValueObjectList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVValueObjectList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVValueObjectList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVValueObjectList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVValueObjectList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVValueObjectList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVValueObjectList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVValueObjectList * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVValueObjectList * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVValueObjectList * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVValueObjectList * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVValueObjectList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVValueObjectList * This,
            /* [in] */ ISVValueObject *p_Item);
        
        END_INTERFACE
    } ISVValueObjectListVtbl;

    interface ISVValueObjectList
    {
        CONST_VTBL struct ISVValueObjectListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVValueObjectList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVValueObjectList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVValueObjectList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVValueObjectList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVValueObjectList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVValueObjectList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVValueObjectList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVValueObjectList_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVValueObjectList_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVValueObjectList_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVValueObjectList_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVValueObjectList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVValueObjectList_Add(This,p_Item)	\
    ( (This)->lpVtbl -> Add(This,p_Item) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVValueObjectList_INTERFACE_DEFINED__ */


#ifndef __ISVImageObject_INTERFACE_DEFINED__
#define __ISVImageObject_INTERFACE_DEFINED__

/* interface ISVImageObject */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVImageObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C14E1A1C-CBAC-4048-8D6A-71CE1E039861")
    ISVImageObject : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Status( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerCount( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetImage( 
            /* [in] */ VARIANT_BOOL overlays,
            /* [in] */ DOUBLE zoom,
            /* [in] */ SVImageFormatsEnum format,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetImage( 
            /* [in] */ VARIANT img) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageFormat( 
            /* [retval][out] */ SVImageFormatsEnum *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ImageFormat( 
            /* [in] */ SVImageFormatsEnum newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVImageObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVImageObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVImageObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVImageObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVImageObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVImageObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVImageObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVImageObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISVImageObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISVImageObject * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Status )( 
            ISVImageObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Status )( 
            ISVImageObject * This,
            /* [in] */ LONG newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TriggerCount )( 
            ISVImageObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TriggerCount )( 
            ISVImageObject * This,
            /* [in] */ LONG newVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetImage )( 
            ISVImageObject * This,
            /* [in] */ VARIANT_BOOL overlays,
            /* [in] */ DOUBLE zoom,
            /* [in] */ SVImageFormatsEnum format,
            /* [retval][out] */ VARIANT *result);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            ISVImageObject * This,
            /* [in] */ VARIANT img);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageFormat )( 
            ISVImageObject * This,
            /* [retval][out] */ SVImageFormatsEnum *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ImageFormat )( 
            ISVImageObject * This,
            /* [in] */ SVImageFormatsEnum newVal);
        
        END_INTERFACE
    } ISVImageObjectVtbl;

    interface ISVImageObject
    {
        CONST_VTBL struct ISVImageObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVImageObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVImageObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVImageObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVImageObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVImageObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVImageObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVImageObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVImageObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISVImageObject_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISVImageObject_get_Status(This,pVal)	\
    ( (This)->lpVtbl -> get_Status(This,pVal) ) 

#define ISVImageObject_put_Status(This,newVal)	\
    ( (This)->lpVtbl -> put_Status(This,newVal) ) 

#define ISVImageObject_get_TriggerCount(This,pVal)	\
    ( (This)->lpVtbl -> get_TriggerCount(This,pVal) ) 

#define ISVImageObject_put_TriggerCount(This,newVal)	\
    ( (This)->lpVtbl -> put_TriggerCount(This,newVal) ) 

#define ISVImageObject_GetImage(This,overlays,zoom,format,result)	\
    ( (This)->lpVtbl -> GetImage(This,overlays,zoom,format,result) ) 

#define ISVImageObject_SetImage(This,img)	\
    ( (This)->lpVtbl -> SetImage(This,img) ) 

#define ISVImageObject_get_ImageFormat(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageFormat(This,pVal) ) 

#define ISVImageObject_put_ImageFormat(This,newVal)	\
    ( (This)->lpVtbl -> put_ImageFormat(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVImageObject_INTERFACE_DEFINED__ */


#ifndef __ISVImageObjectList_INTERFACE_DEFINED__
#define __ISVImageObjectList_INTERFACE_DEFINED__

/* interface ISVImageObjectList */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVImageObjectList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD87ECE3-5AE2-4987-8F3E-852C01CD691A")
    ISVImageObjectList : public IDispatch
    {
    public:
        virtual /* [restricted][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISVImageObject *object) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVImageObjectListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVImageObjectList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVImageObjectList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVImageObjectList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVImageObjectList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVImageObjectList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVImageObjectList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVImageObjectList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [restricted][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVImageObjectList * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVImageObjectList * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVImageObjectList * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVImageObjectList * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVImageObjectList * This,
            /* [in] */ LONG index);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVImageObjectList * This,
            /* [in] */ ISVImageObject *object);
        
        END_INTERFACE
    } ISVImageObjectListVtbl;

    interface ISVImageObjectList
    {
        CONST_VTBL struct ISVImageObjectListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVImageObjectList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVImageObjectList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVImageObjectList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVImageObjectList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVImageObjectList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVImageObjectList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVImageObjectList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVImageObjectList_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVImageObjectList_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVImageObjectList_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVImageObjectList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVImageObjectList_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVImageObjectList_Add(This,object)	\
    ( (This)->lpVtbl -> Add(This,object) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVImageObjectList_INTERFACE_DEFINED__ */


#ifndef __ISVErrorObject_INTERFACE_DEFINED__
#define __ISVErrorObject_INTERFACE_DEFINED__

/* interface ISVErrorObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVErrorObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("197A36AC-3B8E-4722-9B82-E5B2FE2C533C")
    ISVErrorObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Status( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVErrorObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVErrorObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVErrorObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVErrorObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVErrorObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVErrorObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVErrorObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVErrorObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISVErrorObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISVErrorObject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Status )( 
            ISVErrorObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Status )( 
            ISVErrorObject * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } ISVErrorObjectVtbl;

    interface ISVErrorObject
    {
        CONST_VTBL struct ISVErrorObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVErrorObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVErrorObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVErrorObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVErrorObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVErrorObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVErrorObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVErrorObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVErrorObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISVErrorObject_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISVErrorObject_get_Status(This,pVal)	\
    ( (This)->lpVtbl -> get_Status(This,pVal) ) 

#define ISVErrorObject_put_Status(This,newVal)	\
    ( (This)->lpVtbl -> put_Status(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVErrorObject_INTERFACE_DEFINED__ */


#ifndef __ISVErrorObjectList_INTERFACE_DEFINED__
#define __ISVErrorObjectList_INTERFACE_DEFINED__

/* interface ISVErrorObjectList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVErrorObjectList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33E96A56-1B83-441f-AFAF-1F4B4653687E")
    ISVErrorObjectList : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISVErrorObject *p_Item) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVErrorObjectListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVErrorObjectList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVErrorObjectList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVErrorObjectList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVErrorObjectList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVErrorObjectList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVErrorObjectList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVErrorObjectList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVErrorObjectList * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVErrorObjectList * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVErrorObjectList * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVErrorObjectList * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVErrorObjectList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVErrorObjectList * This,
            /* [in] */ ISVErrorObject *p_Item);
        
        END_INTERFACE
    } ISVErrorObjectListVtbl;

    interface ISVErrorObjectList
    {
        CONST_VTBL struct ISVErrorObjectListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVErrorObjectList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVErrorObjectList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVErrorObjectList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVErrorObjectList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVErrorObjectList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVErrorObjectList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVErrorObjectList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVErrorObjectList_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVErrorObjectList_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVErrorObjectList_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVErrorObjectList_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVErrorObjectList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVErrorObjectList_Add(This,p_Item)	\
    ( (This)->lpVtbl -> Add(This,p_Item) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVErrorObjectList_INTERFACE_DEFINED__ */


#ifndef __ISVProductItems_INTERFACE_DEFINED__
#define __ISVProductItems_INTERFACE_DEFINED__

/* interface ISVProductItems */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVProductItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A4786BF3-5F52-4273-BB03-B5C4F37FFA17")
    ISVProductItems : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ ISVValueObjectList **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Values( 
            /* [in] */ ISVValueObjectList *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Images( 
            /* [retval][out] */ ISVImageObjectList **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Images( 
            /* [in] */ ISVImageObjectList *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Errors( 
            /* [retval][out] */ ISVErrorObjectList **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Errors( 
            /* [in] */ ISVErrorObjectList *newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVProductItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVProductItems * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVProductItems * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVProductItems * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVProductItems * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVProductItems * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVProductItems * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVProductItems * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            ISVProductItems * This,
            /* [retval][out] */ ISVValueObjectList **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Values )( 
            ISVProductItems * This,
            /* [in] */ ISVValueObjectList *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Images )( 
            ISVProductItems * This,
            /* [retval][out] */ ISVImageObjectList **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Images )( 
            ISVProductItems * This,
            /* [in] */ ISVImageObjectList *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Errors )( 
            ISVProductItems * This,
            /* [retval][out] */ ISVErrorObjectList **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Errors )( 
            ISVProductItems * This,
            /* [in] */ ISVErrorObjectList *newVal);
        
        END_INTERFACE
    } ISVProductItemsVtbl;

    interface ISVProductItems
    {
        CONST_VTBL struct ISVProductItemsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVProductItems_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVProductItems_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVProductItems_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVProductItems_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVProductItems_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVProductItems_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVProductItems_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVProductItems_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define ISVProductItems_put_Values(This,newVal)	\
    ( (This)->lpVtbl -> put_Values(This,newVal) ) 

#define ISVProductItems_get_Images(This,pVal)	\
    ( (This)->lpVtbl -> get_Images(This,pVal) ) 

#define ISVProductItems_put_Images(This,newVal)	\
    ( (This)->lpVtbl -> put_Images(This,newVal) ) 

#define ISVProductItems_get_Errors(This,pVal)	\
    ( (This)->lpVtbl -> get_Errors(This,pVal) ) 

#define ISVProductItems_put_Errors(This,newVal)	\
    ( (This)->lpVtbl -> put_Errors(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVProductItems_INTERFACE_DEFINED__ */


#ifndef __ISVDataDefObject_INTERFACE_DEFINED__
#define __ISVDataDefObject_INTERFACE_DEFINED__

/* interface ISVDataDefObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVDataDefObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72D176A0-88E1-4546-88AE-1E2CD244071F")
    ISVDataDefObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Writable( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Writable( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Published( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Published( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DataType( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DataType( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ VARIANT Item) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVDataDefObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVDataDefObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVDataDefObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVDataDefObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVDataDefObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVDataDefObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVDataDefObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVDataDefObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVDataDefObject * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVDataDefObject * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVDataDefObject * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISVDataDefObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISVDataDefObject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Writable )( 
            ISVDataDefObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Writable )( 
            ISVDataDefObject * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Published )( 
            ISVDataDefObject * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Published )( 
            ISVDataDefObject * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataType )( 
            ISVDataDefObject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataType )( 
            ISVDataDefObject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVDataDefObject * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVDataDefObject * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVDataDefObject * This,
            /* [in] */ VARIANT Item);
        
        END_INTERFACE
    } ISVDataDefObjectVtbl;

    interface ISVDataDefObject
    {
        CONST_VTBL struct ISVDataDefObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVDataDefObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVDataDefObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVDataDefObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVDataDefObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVDataDefObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVDataDefObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVDataDefObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVDataDefObject_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVDataDefObject_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVDataDefObject_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVDataDefObject_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define ISVDataDefObject_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define ISVDataDefObject_get_Writable(This,pVal)	\
    ( (This)->lpVtbl -> get_Writable(This,pVal) ) 

#define ISVDataDefObject_put_Writable(This,newVal)	\
    ( (This)->lpVtbl -> put_Writable(This,newVal) ) 

#define ISVDataDefObject_get_Published(This,pVal)	\
    ( (This)->lpVtbl -> get_Published(This,pVal) ) 

#define ISVDataDefObject_put_Published(This,newVal)	\
    ( (This)->lpVtbl -> put_Published(This,newVal) ) 

#define ISVDataDefObject_get_DataType(This,pVal)	\
    ( (This)->lpVtbl -> get_DataType(This,pVal) ) 

#define ISVDataDefObject_put_DataType(This,newVal)	\
    ( (This)->lpVtbl -> put_DataType(This,newVal) ) 

#define ISVDataDefObject_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVDataDefObject_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVDataDefObject_Add(This,Item)	\
    ( (This)->lpVtbl -> Add(This,Item) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVDataDefObject_INTERFACE_DEFINED__ */


#ifndef __ISVDataDefObjectList_INTERFACE_DEFINED__
#define __ISVDataDefObjectList_INTERFACE_DEFINED__

/* interface ISVDataDefObjectList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVDataDefObjectList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AE059C37-CB27-475B-BFC4-939A29C2FC73")
    ISVDataDefObjectList : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISVDataDefObject *p_Item) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVDataDefObjectListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVDataDefObjectList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVDataDefObjectList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVDataDefObjectList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVDataDefObjectList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVDataDefObjectList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVDataDefObjectList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVDataDefObjectList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVDataDefObjectList * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVDataDefObjectList * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVDataDefObjectList * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVDataDefObjectList * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVDataDefObjectList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVDataDefObjectList * This,
            /* [in] */ ISVDataDefObject *p_Item);
        
        END_INTERFACE
    } ISVDataDefObjectListVtbl;

    interface ISVDataDefObjectList
    {
        CONST_VTBL struct ISVDataDefObjectListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVDataDefObjectList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVDataDefObjectList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVDataDefObjectList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVDataDefObjectList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVDataDefObjectList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVDataDefObjectList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVDataDefObjectList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVDataDefObjectList_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define ISVDataDefObjectList_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVDataDefObjectList_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVDataDefObjectList_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVDataDefObjectList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVDataDefObjectList_Add(This,p_Item)	\
    ( (This)->lpVtbl -> Add(This,p_Item) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVDataDefObjectList_INTERFACE_DEFINED__ */


#ifndef __ISVInputMatchItem_INTERFACE_DEFINED__
#define __ISVInputMatchItem_INTERFACE_DEFINED__

/* interface ISVInputMatchItem */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVInputMatchItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("36963812-AE89-4B9A-92E8-2489C50930CE")
    ISVInputMatchItem : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MatchString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MatchString( 
            /* [in] */ BSTR Val) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TxnIdentifier( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TxnIdentifier( 
            /* [in] */ long Val) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVInputMatchItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVInputMatchItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVInputMatchItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVInputMatchItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVInputMatchItem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVInputMatchItem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVInputMatchItem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVInputMatchItem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MatchString )( 
            ISVInputMatchItem * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MatchString )( 
            ISVInputMatchItem * This,
            /* [in] */ BSTR Val);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TxnIdentifier )( 
            ISVInputMatchItem * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TxnIdentifier )( 
            ISVInputMatchItem * This,
            /* [in] */ long Val);
        
        END_INTERFACE
    } ISVInputMatchItemVtbl;

    interface ISVInputMatchItem
    {
        CONST_VTBL struct ISVInputMatchItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVInputMatchItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVInputMatchItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVInputMatchItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVInputMatchItem_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVInputMatchItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVInputMatchItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVInputMatchItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVInputMatchItem_get_MatchString(This,pVal)	\
    ( (This)->lpVtbl -> get_MatchString(This,pVal) ) 

#define ISVInputMatchItem_put_MatchString(This,Val)	\
    ( (This)->lpVtbl -> put_MatchString(This,Val) ) 

#define ISVInputMatchItem_get_TxnIdentifier(This,pVal)	\
    ( (This)->lpVtbl -> get_TxnIdentifier(This,pVal) ) 

#define ISVInputMatchItem_put_TxnIdentifier(This,Val)	\
    ( (This)->lpVtbl -> put_TxnIdentifier(This,Val) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVInputMatchItem_INTERFACE_DEFINED__ */


#ifndef __ISVInputMatchList_INTERFACE_DEFINED__
#define __ISVInputMatchList_INTERFACE_DEFINED__

/* interface ISVInputMatchList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVInputMatchList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7E31A82C-6046-451A-8D64-26AC2FA48713")
    ISVInputMatchList : public IDispatch
    {
    public:
        virtual /* [restricted][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pEnum) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISVInputMatchItem *pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItem( 
            /* [in] */ BSTR matchString,
            /* [in] */ long txn_Identifier) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVInputMatchListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVInputMatchList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVInputMatchList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVInputMatchList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVInputMatchList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVInputMatchList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVInputMatchList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVInputMatchList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [restricted][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            ISVInputMatchList * This,
            /* [retval][out] */ IUnknown **pEnum);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            ISVInputMatchList * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISVInputMatchList * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            ISVInputMatchList * This,
            /* [in] */ LONG index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVInputMatchList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVInputMatchList * This,
            /* [in] */ ISVInputMatchItem *pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            ISVInputMatchList * This,
            /* [in] */ BSTR matchString,
            /* [in] */ long txn_Identifier);
        
        END_INTERFACE
    } ISVInputMatchListVtbl;

    interface ISVInputMatchList
    {
        CONST_VTBL struct ISVInputMatchListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVInputMatchList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVInputMatchList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVInputMatchList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVInputMatchList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVInputMatchList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVInputMatchList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVInputMatchList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVInputMatchList_get__NewEnum(This,pEnum)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pEnum) ) 

#define ISVInputMatchList_get_Item(This,index,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,index,pVal) ) 

#define ISVInputMatchList_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define ISVInputMatchList_Remove(This,index)	\
    ( (This)->lpVtbl -> Remove(This,index) ) 

#define ISVInputMatchList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ISVInputMatchList_Add(This,pItem)	\
    ( (This)->lpVtbl -> Add(This,pItem) ) 

#define ISVInputMatchList_AddItem(This,matchString,txn_Identifier)	\
    ( (This)->lpVtbl -> AddItem(This,matchString,txn_Identifier) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVInputMatchList_INTERFACE_DEFINED__ */


#ifndef __ISVRemoteCtrl_INTERFACE_DEFINED__
#define __ISVRemoteCtrl_INTERFACE_DEFINED__

/* interface ISVRemoteCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVRemoteCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1457A846-AF0D-4C2D-AAF4-DD8400B117D9")
    ISVRemoteCtrl : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ LONG_PTR *pHWND) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_ServerAddress( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_VPAddress( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_ServerType( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][readonly][id][propget] */ HRESULT STDMETHODCALLTYPE get_Connected( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageScale( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ImageScale( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginGetConfig( 
            /* [in] */ BSTR fileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinishGetConfig( 
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginPutConfig( 
            /* [in] */ BSTR fileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinishPutConfig( 
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginPutDeviceFile( 
            /* [in] */ BSTR sourcePath,
            /* [in] */ BSTR destinationPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinishPutDeviceFile( 
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginSetDeviceMode( 
            /* [in] */ SVObserverModesEnum desiredMode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinishSetDeviceMode( 
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConfig( 
            /* [in] */ BSTR filePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceConfigReport( 
            /* [retval][out] */ BSTR *report) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceMode( 
            /* [retval][out] */ LONG *mode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceOfflineCount( 
            /* [retval][out] */ LONG *count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItems( 
            /* [in] */ VARIANT itemNames,
            /* [retval][out] */ ISVProductItems **items) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetState( 
            /* [retval][out] */ LONG *state) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutConfig( 
            /* [in] */ BSTR filePath,
            /* [in] */ LONG Timeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [out] */ BSTR *svobserver_ver,
            /* [out] */ BSTR *svremotecontrol_ver,
            /* [out] */ BSTR *runrejectserver_ver) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutDeviceFile( 
            /* [in] */ BSTR sourcePath,
            /* [in] */ BSTR destinationPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDeviceMode( 
            /* [in] */ SVObserverModesEnum desiredMode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItems( 
            /* [in] */ ISVProductItems *items,
            /* [retval][out] */ ISVProductItems **faults) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ BSTR address,
            /* [defaultvalue][in] */ LONG Timeout = 1500) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterMonitorList( 
            /* [in] */ BSTR listName,
            /* [in] */ BSTR ppqName,
            /* [in] */ int rejectDepth,
            /* [in] */ VARIANT productItemList,
            /* [in] */ VARIANT rejectCondList,
            /* [in] */ VARIANT failStatusList,
            /* [retval][out] */ ISVErrorObjectList **errors) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryProductList( 
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryRejectCondList( 
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryFailStatusList( 
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProduct( 
            /* [in] */ BSTR listName,
            /* [in] */ LONG triggerCount,
            /* [in] */ LONG imageScale,
            /* [retval][out] */ ISVProductItems **currentViewItems) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReject( 
            /* [in] */ BSTR listName,
            /* [in] */ LONG triggerCount,
            /* [in] */ LONG imageScale,
            /* [retval][out] */ ISVProductItems **currentViewItems) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDataDefinitionList( 
            /* [in] */ BSTR inspectionName,
            /* [in] */ SVDataDefinitionListTypeEnum listType,
            /* [retval][out] */ ISVDataDefObjectList **entries) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ActivateMonitorList( 
            /* [in] */ BSTR listName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeactivateMonitorList( 
            /* [in] */ BSTR listName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFailStatus( 
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *values) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProductFilter( 
            /* [in] */ BSTR listName,
            /* [in] */ SVProductFilterModesEnum mode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProductFilter( 
            /* [in] */ BSTR listName,
            /* [retval][out] */ LONG *mode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Shutdown( 
            /* [defaultvalue][in] */ SVShutdownOptionsEnum options = Graceful) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryMonitorListNames( 
            /* [retval][out] */ VARIANT *listNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInspectionNames( 
            /* [retval][out] */ VARIANT *listNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMonitorListProperties( 
            /* [in] */ BSTR listName,
            /* [out] */ LONG *RejectDepth,
            /* [out] */ VARIANT_BOOL *isActive,
            /* [out] */ BSTR *ppqName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMaxRejectQeueDepth( 
            /* [retval][out] */ LONG *maxRejectDepth) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVRemoteCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVRemoteCtrl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVRemoteCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVRemoteCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVRemoteCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVRemoteCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVRemoteCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVRemoteCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG_PTR *pHWND);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ServerAddress )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VPAddress )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ServerType )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][readonly][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Connected )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageScale )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ImageScale )( 
            ISVRemoteCtrl * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginGetConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR fileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinishGetConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginPutConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR fileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinishPutConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginPutDeviceFile )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR sourcePath,
            /* [in] */ BSTR destinationPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinishPutDeviceFile )( 
            ISVRemoteCtrl * This,
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginSetDeviceMode )( 
            ISVRemoteCtrl * This,
            /* [in] */ SVObserverModesEnum desiredMode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinishSetDeviceMode )( 
            ISVRemoteCtrl * This,
            /* [in] */ LONG Timeout,
            /* [in] */ VARIANT_BOOL CancelAfterTimeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR filePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceConfigReport )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ BSTR *report);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceMode )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG *mode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceOfflineCount )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG *count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItems )( 
            ISVRemoteCtrl * This,
            /* [in] */ VARIANT itemNames,
            /* [retval][out] */ ISVProductItems **items);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetState )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG *state);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutConfig )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR filePath,
            /* [in] */ LONG Timeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            ISVRemoteCtrl * This,
            /* [out] */ BSTR *svobserver_ver,
            /* [out] */ BSTR *svremotecontrol_ver,
            /* [out] */ BSTR *runrejectserver_ver);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutDeviceFile )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR sourcePath,
            /* [in] */ BSTR destinationPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDeviceMode )( 
            ISVRemoteCtrl * This,
            /* [in] */ SVObserverModesEnum desiredMode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItems )( 
            ISVRemoteCtrl * This,
            /* [in] */ ISVProductItems *items,
            /* [retval][out] */ ISVProductItems **faults);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            ISVRemoteCtrl * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR address,
            /* [defaultvalue][in] */ LONG Timeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterMonitorList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [in] */ BSTR ppqName,
            /* [in] */ int rejectDepth,
            /* [in] */ VARIANT productItemList,
            /* [in] */ VARIANT rejectCondList,
            /* [in] */ VARIANT failStatusList,
            /* [retval][out] */ ISVErrorObjectList **errors);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryProductList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryRejectCondList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryFailStatusList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *itemNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProduct )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [in] */ LONG triggerCount,
            /* [in] */ LONG imageScale,
            /* [retval][out] */ ISVProductItems **currentViewItems);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReject )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [in] */ LONG triggerCount,
            /* [in] */ LONG imageScale,
            /* [retval][out] */ ISVProductItems **currentViewItems);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDataDefinitionList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR inspectionName,
            /* [in] */ SVDataDefinitionListTypeEnum listType,
            /* [retval][out] */ ISVDataDefObjectList **entries);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ActivateMonitorList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeactivateMonitorList )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFailStatus )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [retval][out] */ VARIANT *values);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProductFilter )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [in] */ SVProductFilterModesEnum mode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProductFilter )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [retval][out] */ LONG *mode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Shutdown )( 
            ISVRemoteCtrl * This,
            /* [defaultvalue][in] */ SVShutdownOptionsEnum options);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryMonitorListNames )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ VARIANT *listNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetInspectionNames )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ VARIANT *listNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMonitorListProperties )( 
            ISVRemoteCtrl * This,
            /* [in] */ BSTR listName,
            /* [out] */ LONG *RejectDepth,
            /* [out] */ VARIANT_BOOL *isActive,
            /* [out] */ BSTR *ppqName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMaxRejectQeueDepth )( 
            ISVRemoteCtrl * This,
            /* [retval][out] */ LONG *maxRejectDepth);
        
        END_INTERFACE
    } ISVRemoteCtrlVtbl;

    interface ISVRemoteCtrl
    {
        CONST_VTBL struct ISVRemoteCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVRemoteCtrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVRemoteCtrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVRemoteCtrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVRemoteCtrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVRemoteCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVRemoteCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVRemoteCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVRemoteCtrl_get_HWND(This,pHWND)	\
    ( (This)->lpVtbl -> get_HWND(This,pHWND) ) 

#define ISVRemoteCtrl_get_ServerAddress(This,pVal)	\
    ( (This)->lpVtbl -> get_ServerAddress(This,pVal) ) 

#define ISVRemoteCtrl_get_VPAddress(This,pVal)	\
    ( (This)->lpVtbl -> get_VPAddress(This,pVal) ) 

#define ISVRemoteCtrl_get_ServerType(This,pVal)	\
    ( (This)->lpVtbl -> get_ServerType(This,pVal) ) 

#define ISVRemoteCtrl_get_Connected(This,pVal)	\
    ( (This)->lpVtbl -> get_Connected(This,pVal) ) 

#define ISVRemoteCtrl_get_ImageScale(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageScale(This,pVal) ) 

#define ISVRemoteCtrl_put_ImageScale(This,newVal)	\
    ( (This)->lpVtbl -> put_ImageScale(This,newVal) ) 

#define ISVRemoteCtrl_BeginGetConfig(This,fileName)	\
    ( (This)->lpVtbl -> BeginGetConfig(This,fileName) ) 

#define ISVRemoteCtrl_FinishGetConfig(This,Timeout,CancelAfterTimeout)	\
    ( (This)->lpVtbl -> FinishGetConfig(This,Timeout,CancelAfterTimeout) ) 

#define ISVRemoteCtrl_BeginPutConfig(This,fileName)	\
    ( (This)->lpVtbl -> BeginPutConfig(This,fileName) ) 

#define ISVRemoteCtrl_FinishPutConfig(This,Timeout,CancelAfterTimeout)	\
    ( (This)->lpVtbl -> FinishPutConfig(This,Timeout,CancelAfterTimeout) ) 

#define ISVRemoteCtrl_BeginPutDeviceFile(This,sourcePath,destinationPath)	\
    ( (This)->lpVtbl -> BeginPutDeviceFile(This,sourcePath,destinationPath) ) 

#define ISVRemoteCtrl_FinishPutDeviceFile(This,Timeout,CancelAfterTimeout)	\
    ( (This)->lpVtbl -> FinishPutDeviceFile(This,Timeout,CancelAfterTimeout) ) 

#define ISVRemoteCtrl_BeginSetDeviceMode(This,desiredMode)	\
    ( (This)->lpVtbl -> BeginSetDeviceMode(This,desiredMode) ) 

#define ISVRemoteCtrl_FinishSetDeviceMode(This,Timeout,CancelAfterTimeout)	\
    ( (This)->lpVtbl -> FinishSetDeviceMode(This,Timeout,CancelAfterTimeout) ) 

#define ISVRemoteCtrl_GetConfig(This,filePath)	\
    ( (This)->lpVtbl -> GetConfig(This,filePath) ) 

#define ISVRemoteCtrl_GetDeviceConfigReport(This,report)	\
    ( (This)->lpVtbl -> GetDeviceConfigReport(This,report) ) 

#define ISVRemoteCtrl_GetDeviceMode(This,mode)	\
    ( (This)->lpVtbl -> GetDeviceMode(This,mode) ) 

#define ISVRemoteCtrl_GetDeviceOfflineCount(This,count)	\
    ( (This)->lpVtbl -> GetDeviceOfflineCount(This,count) ) 

#define ISVRemoteCtrl_GetItems(This,itemNames,items)	\
    ( (This)->lpVtbl -> GetItems(This,itemNames,items) ) 

#define ISVRemoteCtrl_GetState(This,state)	\
    ( (This)->lpVtbl -> GetState(This,state) ) 

#define ISVRemoteCtrl_PutConfig(This,filePath,Timeout)	\
    ( (This)->lpVtbl -> PutConfig(This,filePath,Timeout) ) 

#define ISVRemoteCtrl_GetVersion(This,svobserver_ver,svremotecontrol_ver,runrejectserver_ver)	\
    ( (This)->lpVtbl -> GetVersion(This,svobserver_ver,svremotecontrol_ver,runrejectserver_ver) ) 

#define ISVRemoteCtrl_PutDeviceFile(This,sourcePath,destinationPath)	\
    ( (This)->lpVtbl -> PutDeviceFile(This,sourcePath,destinationPath) ) 

#define ISVRemoteCtrl_SetDeviceMode(This,desiredMode)	\
    ( (This)->lpVtbl -> SetDeviceMode(This,desiredMode) ) 

#define ISVRemoteCtrl_SetItems(This,items,faults)	\
    ( (This)->lpVtbl -> SetItems(This,items,faults) ) 

#define ISVRemoteCtrl_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define ISVRemoteCtrl_Connect(This,address,Timeout)	\
    ( (This)->lpVtbl -> Connect(This,address,Timeout) ) 

#define ISVRemoteCtrl_RegisterMonitorList(This,listName,ppqName,rejectDepth,productItemList,rejectCondList,failStatusList,errors)	\
    ( (This)->lpVtbl -> RegisterMonitorList(This,listName,ppqName,rejectDepth,productItemList,rejectCondList,failStatusList,errors) ) 

#define ISVRemoteCtrl_QueryProductList(This,listName,itemNames)	\
    ( (This)->lpVtbl -> QueryProductList(This,listName,itemNames) ) 

#define ISVRemoteCtrl_QueryRejectCondList(This,listName,itemNames)	\
    ( (This)->lpVtbl -> QueryRejectCondList(This,listName,itemNames) ) 

#define ISVRemoteCtrl_QueryFailStatusList(This,listName,itemNames)	\
    ( (This)->lpVtbl -> QueryFailStatusList(This,listName,itemNames) ) 

#define ISVRemoteCtrl_GetProduct(This,listName,triggerCount,imageScale,currentViewItems)	\
    ( (This)->lpVtbl -> GetProduct(This,listName,triggerCount,imageScale,currentViewItems) ) 

#define ISVRemoteCtrl_GetReject(This,listName,triggerCount,imageScale,currentViewItems)	\
    ( (This)->lpVtbl -> GetReject(This,listName,triggerCount,imageScale,currentViewItems) ) 

#define ISVRemoteCtrl_GetDataDefinitionList(This,inspectionName,listType,entries)	\
    ( (This)->lpVtbl -> GetDataDefinitionList(This,inspectionName,listType,entries) ) 

#define ISVRemoteCtrl_ActivateMonitorList(This,listName)	\
    ( (This)->lpVtbl -> ActivateMonitorList(This,listName) ) 

#define ISVRemoteCtrl_DeactivateMonitorList(This,listName)	\
    ( (This)->lpVtbl -> DeactivateMonitorList(This,listName) ) 

#define ISVRemoteCtrl_GetFailStatus(This,listName,values)	\
    ( (This)->lpVtbl -> GetFailStatus(This,listName,values) ) 

#define ISVRemoteCtrl_SetProductFilter(This,listName,mode)	\
    ( (This)->lpVtbl -> SetProductFilter(This,listName,mode) ) 

#define ISVRemoteCtrl_GetProductFilter(This,listName,mode)	\
    ( (This)->lpVtbl -> GetProductFilter(This,listName,mode) ) 

#define ISVRemoteCtrl_Shutdown(This,options)	\
    ( (This)->lpVtbl -> Shutdown(This,options) ) 

#define ISVRemoteCtrl_QueryMonitorListNames(This,listNames)	\
    ( (This)->lpVtbl -> QueryMonitorListNames(This,listNames) ) 

#define ISVRemoteCtrl_GetInspectionNames(This,listNames)	\
    ( (This)->lpVtbl -> GetInspectionNames(This,listNames) ) 

#define ISVRemoteCtrl_GetMonitorListProperties(This,listName,RejectDepth,isActive,ppqName)	\
    ( (This)->lpVtbl -> GetMonitorListProperties(This,listName,RejectDepth,isActive,ppqName) ) 

#define ISVRemoteCtrl_GetMaxRejectQeueDepth(This,maxRejectDepth)	\
    ( (This)->lpVtbl -> GetMaxRejectQeueDepth(This,maxRejectDepth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVRemoteCtrl_INTERFACE_DEFINED__ */


#ifndef __ISVInputBroker_INTERFACE_DEFINED__
#define __ISVInputBroker_INTERFACE_DEFINED__

/* interface ISVInputBroker */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVInputBroker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F5F4EB9E-EC1A-4E7C-914E-4BA064F11740")
    ISVInputBroker : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteCtrl( 
            /* [retval][out] */ ISVRemoteCtrl **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteCtrl( 
            /* [in] */ ISVRemoteCtrl *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeoutInSeconds( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeoutInSeconds( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryListNames( 
            /* [retval][out] */ SAFEARRAY * *ppNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryListItems( 
            /* [in] */ BSTR ListName,
            /* [retval][out] */ ISVInputMatchList **ppItems) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( 
            /* [in] */ BSTR ListName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR ListName,
            /* [in] */ ISVInputMatchList *pItems) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVInputBrokerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVInputBroker * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVInputBroker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVInputBroker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVInputBroker * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVInputBroker * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVInputBroker * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVInputBroker * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteCtrl )( 
            ISVInputBroker * This,
            /* [retval][out] */ ISVRemoteCtrl **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteCtrl )( 
            ISVInputBroker * This,
            /* [in] */ ISVRemoteCtrl *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeoutInSeconds )( 
            ISVInputBroker * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeoutInSeconds )( 
            ISVInputBroker * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryListNames )( 
            ISVInputBroker * This,
            /* [retval][out] */ SAFEARRAY * *ppNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryListItems )( 
            ISVInputBroker * This,
            /* [in] */ BSTR ListName,
            /* [retval][out] */ ISVInputMatchList **ppItems);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ISVInputBroker * This,
            /* [in] */ BSTR ListName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            ISVInputBroker * This,
            /* [in] */ BSTR ListName,
            /* [in] */ ISVInputMatchList *pItems);
        
        END_INTERFACE
    } ISVInputBrokerVtbl;

    interface ISVInputBroker
    {
        CONST_VTBL struct ISVInputBrokerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVInputBroker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVInputBroker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVInputBroker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVInputBroker_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVInputBroker_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVInputBroker_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVInputBroker_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVInputBroker_get_RemoteCtrl(This,pVal)	\
    ( (This)->lpVtbl -> get_RemoteCtrl(This,pVal) ) 

#define ISVInputBroker_put_RemoteCtrl(This,newVal)	\
    ( (This)->lpVtbl -> put_RemoteCtrl(This,newVal) ) 

#define ISVInputBroker_get_TimeoutInSeconds(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeoutInSeconds(This,pVal) ) 

#define ISVInputBroker_put_TimeoutInSeconds(This,newVal)	\
    ( (This)->lpVtbl -> put_TimeoutInSeconds(This,newVal) ) 

#define ISVInputBroker_QueryListNames(This,ppNames)	\
    ( (This)->lpVtbl -> QueryListNames(This,ppNames) ) 

#define ISVInputBroker_QueryListItems(This,ListName,ppItems)	\
    ( (This)->lpVtbl -> QueryListItems(This,ListName,ppItems) ) 

#define ISVInputBroker_Clear(This,ListName)	\
    ( (This)->lpVtbl -> Clear(This,ListName) ) 

#define ISVInputBroker_Add(This,ListName,pItems)	\
    ( (This)->lpVtbl -> Add(This,ListName,pItems) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVInputBroker_INTERFACE_DEFINED__ */


#ifndef __ISVOutputBroker_INTERFACE_DEFINED__
#define __ISVOutputBroker_INTERFACE_DEFINED__

/* interface ISVOutputBroker */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVOutputBroker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1C8780A-1C0C-4DE9-B54C-A5014046792D")
    ISVOutputBroker : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteCtrl( 
            /* [retval][out] */ ISVRemoteCtrl **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteCtrl( 
            /* [in] */ ISVRemoteCtrl *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeoutInSeconds( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeoutInSeconds( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ BSTR ListName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( 
            /* [in] */ BSTR ListName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryListNames( 
            /* [retval][out] */ SAFEARRAY * *ppNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryListItems( 
            /* [in] */ BSTR ListName,
            /* [retval][out] */ SAFEARRAY * *ppNames) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVOutputBrokerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVOutputBroker * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVOutputBroker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVOutputBroker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVOutputBroker * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVOutputBroker * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVOutputBroker * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVOutputBroker * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteCtrl )( 
            ISVOutputBroker * This,
            /* [retval][out] */ ISVRemoteCtrl **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteCtrl )( 
            ISVOutputBroker * This,
            /* [in] */ ISVRemoteCtrl *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeoutInSeconds )( 
            ISVOutputBroker * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeoutInSeconds )( 
            ISVOutputBroker * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            ISVOutputBroker * This,
            /* [in] */ BSTR ListName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ISVOutputBroker * This,
            /* [in] */ BSTR ListName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryListNames )( 
            ISVOutputBroker * This,
            /* [retval][out] */ SAFEARRAY * *ppNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryListItems )( 
            ISVOutputBroker * This,
            /* [in] */ BSTR ListName,
            /* [retval][out] */ SAFEARRAY * *ppNames);
        
        END_INTERFACE
    } ISVOutputBrokerVtbl;

    interface ISVOutputBroker
    {
        CONST_VTBL struct ISVOutputBrokerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVOutputBroker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVOutputBroker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVOutputBroker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVOutputBroker_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVOutputBroker_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVOutputBroker_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVOutputBroker_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVOutputBroker_get_RemoteCtrl(This,pVal)	\
    ( (This)->lpVtbl -> get_RemoteCtrl(This,pVal) ) 

#define ISVOutputBroker_put_RemoteCtrl(This,newVal)	\
    ( (This)->lpVtbl -> put_RemoteCtrl(This,newVal) ) 

#define ISVOutputBroker_get_TimeoutInSeconds(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeoutInSeconds(This,pVal) ) 

#define ISVOutputBroker_put_TimeoutInSeconds(This,newVal)	\
    ( (This)->lpVtbl -> put_TimeoutInSeconds(This,newVal) ) 

#define ISVOutputBroker_Start(This,ListName)	\
    ( (This)->lpVtbl -> Start(This,ListName) ) 

#define ISVOutputBroker_Stop(This,ListName)	\
    ( (This)->lpVtbl -> Stop(This,ListName) ) 

#define ISVOutputBroker_QueryListNames(This,ppNames)	\
    ( (This)->lpVtbl -> QueryListNames(This,ppNames) ) 

#define ISVOutputBroker_QueryListItems(This,ListName,ppNames)	\
    ( (This)->lpVtbl -> QueryListItems(This,ListName,ppNames) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVOutputBroker_INTERFACE_DEFINED__ */


#ifndef __ISVFailStatusStream_INTERFACE_DEFINED__
#define __ISVFailStatusStream_INTERFACE_DEFINED__

/* interface ISVFailStatusStream */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISVFailStatusStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7D5743E5-6576-455C-B7AD-2189B4A28ACD")
    ISVFailStatusStream : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteCtrl( 
            /* [retval][out] */ ISVRemoteCtrl **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteCtrl( 
            /* [in] */ ISVRemoteCtrl *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeoutInSeconds( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeoutInSeconds( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ BSTR ListName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( 
            /* [in] */ BSTR ListName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISVFailStatusStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISVFailStatusStream * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISVFailStatusStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISVFailStatusStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISVFailStatusStream * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISVFailStatusStream * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISVFailStatusStream * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISVFailStatusStream * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteCtrl )( 
            ISVFailStatusStream * This,
            /* [retval][out] */ ISVRemoteCtrl **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteCtrl )( 
            ISVFailStatusStream * This,
            /* [in] */ ISVRemoteCtrl *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeoutInSeconds )( 
            ISVFailStatusStream * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeoutInSeconds )( 
            ISVFailStatusStream * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            ISVFailStatusStream * This,
            /* [in] */ BSTR ListName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ISVFailStatusStream * This,
            /* [in] */ BSTR ListName);
        
        END_INTERFACE
    } ISVFailStatusStreamVtbl;

    interface ISVFailStatusStream
    {
        CONST_VTBL struct ISVFailStatusStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISVFailStatusStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISVFailStatusStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISVFailStatusStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISVFailStatusStream_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISVFailStatusStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISVFailStatusStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISVFailStatusStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISVFailStatusStream_get_RemoteCtrl(This,pVal)	\
    ( (This)->lpVtbl -> get_RemoteCtrl(This,pVal) ) 

#define ISVFailStatusStream_put_RemoteCtrl(This,newVal)	\
    ( (This)->lpVtbl -> put_RemoteCtrl(This,newVal) ) 

#define ISVFailStatusStream_get_TimeoutInSeconds(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeoutInSeconds(This,pVal) ) 

#define ISVFailStatusStream_put_TimeoutInSeconds(This,newVal)	\
    ( (This)->lpVtbl -> put_TimeoutInSeconds(This,newVal) ) 

#define ISVFailStatusStream_Start(This,ListName)	\
    ( (This)->lpVtbl -> Start(This,ListName) ) 

#define ISVFailStatusStream_Stop(This,ListName)	\
    ( (This)->lpVtbl -> Stop(This,ListName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISVFailStatusStream_INTERFACE_DEFINED__ */



#ifndef __SVRemoteControlLib_LIBRARY_DEFINED__
#define __SVRemoteControlLib_LIBRARY_DEFINED__

/* library SVRemoteControlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SVRemoteControlLib;

#ifndef ___ISVFailStatusStreamEvents_DISPINTERFACE_DEFINED__
#define ___ISVFailStatusStreamEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISVFailStatusStreamEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISVFailStatusStreamEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("13801904-F3B5-43C7-98E5-AF28EE3A7155")
    _ISVFailStatusStreamEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISVFailStatusStreamEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISVFailStatusStreamEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISVFailStatusStreamEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISVFailStatusStreamEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISVFailStatusStreamEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISVFailStatusStreamEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISVFailStatusStreamEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISVFailStatusStreamEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISVFailStatusStreamEventsVtbl;

    interface _ISVFailStatusStreamEvents
    {
        CONST_VTBL struct _ISVFailStatusStreamEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISVFailStatusStreamEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISVFailStatusStreamEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISVFailStatusStreamEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISVFailStatusStreamEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISVFailStatusStreamEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISVFailStatusStreamEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISVFailStatusStreamEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISVFailStatusStreamEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___ISVOutputBrokerEvents_DISPINTERFACE_DEFINED__
#define ___ISVOutputBrokerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISVOutputBrokerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISVOutputBrokerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("19A3CAAF-348F-4A47-98D7-1382FB67FAD7")
    _ISVOutputBrokerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISVOutputBrokerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISVOutputBrokerEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISVOutputBrokerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISVOutputBrokerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISVOutputBrokerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISVOutputBrokerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISVOutputBrokerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISVOutputBrokerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISVOutputBrokerEventsVtbl;

    interface _ISVOutputBrokerEvents
    {
        CONST_VTBL struct _ISVOutputBrokerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISVOutputBrokerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISVOutputBrokerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISVOutputBrokerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISVOutputBrokerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISVOutputBrokerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISVOutputBrokerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISVOutputBrokerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISVOutputBrokerEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___ISVRemoteCtrlEvents_DISPINTERFACE_DEFINED__
#define ___ISVRemoteCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISVRemoteCtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISVRemoteCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("89A7446B-ACCE-483B-8C2A-19B882C23011")
    _ISVRemoteCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISVRemoteCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISVRemoteCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISVRemoteCtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISVRemoteCtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISVRemoteCtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISVRemoteCtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISVRemoteCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISVRemoteCtrlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISVRemoteCtrlEventsVtbl;

    interface _ISVRemoteCtrlEvents
    {
        CONST_VTBL struct _ISVRemoteCtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISVRemoteCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISVRemoteCtrlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISVRemoteCtrlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISVRemoteCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISVRemoteCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISVRemoteCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISVRemoteCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISVRemoteCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SVRemoteCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("E4094670-241C-4625-A900-AD8BD44CED08")
SVRemoteCtrl;
#endif

EXTERN_C const CLSID CLSID_SVValueObject;

#ifdef __cplusplus

class DECLSPEC_UUID("959F3092-2981-4563-95FB-229F894F6444")
SVValueObject;
#endif

EXTERN_C const CLSID CLSID_SVValueObjectList;

#ifdef __cplusplus

class DECLSPEC_UUID("FBEC601D-59CE-4011-8F3A-FB0B15C07F01")
SVValueObjectList;
#endif

EXTERN_C const CLSID CLSID_SVErrorObject;

#ifdef __cplusplus

class DECLSPEC_UUID("B06B4F17-A3A6-47f6-B1C6-9AF0A69D2EF3")
SVErrorObject;
#endif

EXTERN_C const CLSID CLSID_SVErrorObjectList;

#ifdef __cplusplus

class DECLSPEC_UUID("720F2679-B08E-4629-B87E-17D6CACB470F")
SVErrorObjectList;
#endif

EXTERN_C const CLSID CLSID_SVDataDefObject;

#ifdef __cplusplus

class DECLSPEC_UUID("9EA40F77-F8AB-4D92-BAEA-24C67E3AFE62")
SVDataDefObject;
#endif

EXTERN_C const CLSID CLSID_SVDataDefObjectList;

#ifdef __cplusplus

class DECLSPEC_UUID("D85F32AD-CDD7-4A09-9A19-EFD0181F3075")
SVDataDefObjectList;
#endif

EXTERN_C const CLSID CLSID_SVProductItems;

#ifdef __cplusplus

class DECLSPEC_UUID("2C408BE7-AB83-4E34-AA23-B195FCB82632")
SVProductItems;
#endif

EXTERN_C const CLSID CLSID_SVInputMatchItem;

#ifdef __cplusplus

class DECLSPEC_UUID("38168345-B330-4297-AC07-9EFD1A2D510E")
SVInputMatchItem;
#endif

EXTERN_C const CLSID CLSID_SVInputMatchList;

#ifdef __cplusplus

class DECLSPEC_UUID("8C59EC64-1165-4343-BF20-1FFC6BF52D4C")
SVInputMatchList;
#endif

EXTERN_C const CLSID CLSID_SVInputBroker;

#ifdef __cplusplus

class DECLSPEC_UUID("66C35D87-850D-4D2D-87F4-8F341D0527CF")
SVInputBroker;
#endif

EXTERN_C const CLSID CLSID_SVOutputBroker;

#ifdef __cplusplus

class DECLSPEC_UUID("63DB739C-8E97-4207-887D-BCE9CEC3023C")
SVOutputBroker;
#endif

EXTERN_C const CLSID CLSID_SVFailStatusStream;

#ifdef __cplusplus

class DECLSPEC_UUID("3F460BC8-ADBA-470F-8844-5D5A8BA5C62D")
SVFailStatusStream;
#endif

EXTERN_C const CLSID CLSID_SVImageObject;

#ifdef __cplusplus

class DECLSPEC_UUID("F2222858-76DD-4DB9-9D4B-6B2AF75700C1")
SVImageObject;
#endif

EXTERN_C const CLSID CLSID_SVImageObjectList;

#ifdef __cplusplus

class DECLSPEC_UUID("19BA3FD5-A318-4FD7-950B-5B537B8D61DE")
SVImageObjectList;
#endif
#endif /* __SVRemoteControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


