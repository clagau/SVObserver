// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDigitizerLoadLibraryClass
// * .File Name       : $Workfile:   SVDigitizerLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:20  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "TriggerHandling/TriggerBasics.h"
#include "SVOMFCLibrary/SVOMFCLibrary.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVAcquisitionBufferInterface.h"
#pragma endregion Includes

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVDigitizerGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetHandlePtr)( SVHANDLE *, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerGetNamePtr)( SVHANDLE, BSTR * );
typedef HRESULT (WINAPI *SVDigitizerLoadCameraFilesPtr)( SVHANDLE, SAFEARRAY* );
typedef HRESULT (WINAPI *SVDigitizerGetBufferWidthPtr)( SVHANDLE, unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetBufferHeightPtr)( SVHANDLE, unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetBufferFormatPtr)( SVHANDLE, int * );
typedef HRESULT (WINAPI *SVDigitizerCreateBuffersPtr)( SVHANDLE, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerRegisterBufferInterfacePtr)( SVHANDLE, SVAcquisitionBufferInterface * );
typedef HRESULT (WINAPI *SVDigitizerStartPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerStopPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnregisterBufferInterfacePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerEnablePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerRegisterPtr)( SVHANDLE, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo);
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerUnregisterPtr)( SVHANDLE, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo);
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerUnregisterAllPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnregisterStartAcquirePtr)( SVHANDLE, unsigned long, SvTh::TriggerCallbackInformation triggerCallbackInfo);
typedef HRESULT (WINAPI *SVDigitizerUnregisterStopAcquirePtr)( SVHANDLE, unsigned long, SvTh::TriggerCallbackInformation triggerCallbackInfo);
typedef HRESULT (WINAPI *SVDigitizerUnregisterAllAcquirePtr)( SVHANDLE, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerDestroyBuffersPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnloadCameraFilePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerSetParametersPtr)( SVHANDLE, const SVDeviceParamCollection* );
typedef HRESULT (WINAPI *SVDigitizerSetParameterPtr)( SVHANDLE, const SVDeviceParamWrapper* );
typedef HRESULT (WINAPI *SVDigitizerGetParameterPtr)( SVHANDLE, SVDeviceParamEnum, SVDeviceParamWrapper** );
typedef HRESULT (WINAPI *SVDigitizerDestroyParameterPtr)( SVHANDLE, SVDeviceParamWrapper* );
typedef HRESULT (WINAPI *SVDigitizerParameterGetListPtr)( SVHANDLE, VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerParameterGetNamePtr)( SVHANDLE, unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVDigitizerParameterGetValuePtr)( SVHANDLE, int, int *,VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerParameterSetValuePtr)( SVHANDLE, int, int, VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerScanForCamerasPtr)();

/*
This class is used to interface into the digitizer DLLs.  The methods of this class directly relate to the exported function of the DLL.
*/
class SVDigitizerLoadLibraryClass  
{
public:
	SVDigitizerLoadLibraryClass();
	~SVDigitizerLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close();
	
	bool IsLoaded() const;
	HRESULT GetLibraryName( BSTR *p_pbstrName );

	HRESULT GetCount( unsigned long *p_pulCount );
	HRESULT GetHandle( SVHANDLE *p_phHandle, unsigned long p_ulIndex );
	HRESULT GetName( SVHANDLE p_hHandle, BSTR *p_pbstrName );
	HRESULT LoadCameraFiles( SVHANDLE p_hHandle, SAFEARRAY* p_psaFileNames );
	HRESULT GetBufferWidth( SVHANDLE p_hHandle, unsigned long *p_pulWidth );
	HRESULT GetBufferHeight( SVHANDLE p_hHandle, unsigned long *p_pulHeight );
	HRESULT GetBufferFormat( SVHANDLE p_hHandle, int *p_piFormat );
	HRESULT CreateBuffers( SVHANDLE p_hHandle, unsigned long p_ulCount );
	HRESULT RegisterBufferInterface( SVHANDLE p_hHandle, SVAcquisitionBufferInterface* p_pInterface );
	HRESULT Start( SVHANDLE p_hHandle );
	HRESULT Stop( SVHANDLE p_hHandle );
	HRESULT UnregisterBufferInterface( SVHANDLE p_hHandle );
	
	HRESULT InternalTriggerEnable( SVHANDLE p_hHandle );
	HRESULT InternalTrigger( SVHANDLE p_hHandle );
	HRESULT InternalTriggerRegister( SVHANDLE p_hHandle, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo );
	HRESULT InternalTriggerUnregister( SVHANDLE p_hHandle, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo );
	HRESULT InternalTriggerUnregisterAll( SVHANDLE p_hHandle );
	
	HRESULT DestroyBuffers( SVHANDLE p_hHandle );
	HRESULT UnloadCameraFile( SVHANDLE p_hHandle );
	HRESULT SetParameters( SVHANDLE p_hHandle, const SVDeviceParamCollection* p_pParameters );
	HRESULT SetParameter( SVHANDLE p_hHandle, const SVDeviceParamWrapper* p_pParameter );
	HRESULT GetParameter( SVHANDLE p_hHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter );
	HRESULT DestroyParameter( SVHANDLE p_hHandle, SVDeviceParamWrapper* p_pParameter );
	HRESULT ParameterGetList( SVHANDLE p_hHandle, VARIANT *p_pvarName );
	HRESULT ParameterGetName( SVHANDLE p_hHandle, int p_iParameterID, BSTR *p_pbstrName );
	HRESULT ParameterGetValue( SVHANDLE p_hHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarName );
	HRESULT ParameterSetValue( SVHANDLE p_hHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarName );

	HRESULT ScanForCameras();

private:
	//This attribute holds the handle to the open DLL.
	HMODULE m_hmHandle;

	//This attribute holds the address to the SVCreate function in the DLL.
	SVCreatePtr m_psvCreate;
	//This attribute holds the address to the SVDestroy function in the DLL.
	SVDestroyPtr m_psvDestroy;
	//This attribute holds the address to the SVDigitizerGetCount function in the DLL.
	SVDigitizerGetCountPtr m_psvGetCount;
	//This attribute holds the address to the SVDigitizerGetHandle function in the DLL.
	SVDigitizerGetHandlePtr m_psvGetHandle;
	//This attribute holds the address to the SVDigitizerGetName function in the DLL.
	SVDigitizerGetNamePtr m_psvGetName;
	//This attribute holds the address to the SVDigitizerLoadCameraFiles function in the DLL.
	SVDigitizerLoadCameraFilesPtr m_psvLoadCameraFiles;
	//This attribute holds the address to the SVDigitizerGetBufferWidth function in the DLL.
	SVDigitizerGetBufferWidthPtr m_psvGetBufferWidth;
	//This attribute holds the address to the SVDigitizerGetBufferHeight function in the DLL.
	SVDigitizerGetBufferHeightPtr m_psvGetBufferHeight;
	//This attribute holds the address to the SVDigitizerGetBufferFormat function in the DLL.
	SVDigitizerGetBufferFormatPtr m_psvGetBufferFormat;
	//This attribute holds the address to the SVDigitizerCreateBuffers function in the DLL.
	SVDigitizerCreateBuffersPtr m_psvCreateBuffers;
	//This attribute holds the address to the SVDigitizerRegisterBufferInterface function in the DLL.
	SVDigitizerRegisterBufferInterfacePtr m_psvRegisterBufferInterface;
	//This attribute holds the address to the SVDigitizerStart function in the DLL.
	SVDigitizerStartPtr m_psvStart;
	//This attribute holds the address to the SVDigitizerStop function in the DLL.
	SVDigitizerStopPtr m_psvStop;
	//This attribute holds the address to the SVDigitizerUnregisterBufferInterface function in the DLL.
	SVDigitizerUnregisterBufferInterfacePtr m_psvUnregisterBufferInterface;
	//This attribute holds the address to the SVDigitizerInternalTriggerEnable function in the DLL.
	SVDigitizerInternalTriggerEnablePtr m_psvInternalTriggerEnable;
	//This attribute holds the address to the SVDigitizerInternalTrigger function in the DLL.
	SVDigitizerInternalTriggerPtr m_psvInternalTrigger;
	//This attribute holds the address to the SVDigitizerInternalTriggerRegister function in the DLL.
	SVDigitizerInternalTriggerRegisterPtr m_psvInternalTriggerRegister;
	//This attribute holds the address to the SVDigitizerInternalTriggerUnregister function in the DLL.
	SVDigitizerInternalTriggerUnregisterPtr m_psvInternalTriggerUnregister;
	//This attribute holds the address to the SVDigitizerInternalTriggerUnregisterAll function in the DLL.
	SVDigitizerInternalTriggerUnregisterAllPtr m_psvInternalTriggerUnregisterAll;
	//This attribute holds the address to the SVDigitizerDestroyBuffers function in the DLL.
	SVDigitizerDestroyBuffersPtr m_psvDestroyBuffers;
	//This attribute holds the address to the SVDigitizerUnloadCameraFile function in the DLL.
	SVDigitizerUnloadCameraFilePtr m_psvUnloadCameraFile;
	//This attribute holds the address to the SVDigitizerSetParameters function in the DLL.
	SVDigitizerSetParametersPtr m_psvSetParameters;
	//This attribute holds the address to the SVDigitizerSetParameter function in the DLL.
	SVDigitizerSetParameterPtr m_psvSetParameter;
	//This attribute holds the address to the SVDigitizerGetParameter function in the DLL.
	SVDigitizerGetParameterPtr m_psvGetParameter;
	//This attribute holds the address to the SVDigitizerDestroyParameter function in the DLL.
	SVDigitizerDestroyParameterPtr m_psvDestroyParameter;
	//This attribute holds the address to the SVDigitizerParameterGetList function in the DLL.
	SVDigitizerParameterGetListPtr m_psvParameterGetList;
	//This attribute holds the address to the SVDigitizerParameterGetName function in the DLL.
	SVDigitizerParameterGetNamePtr m_psvParameterGetName;
	//This attribute holds the address to the SVDigitizerParameterGetValue function in the DLL.
	SVDigitizerParameterGetValuePtr m_psvParameterGetValue;
	//This attribute holds the address to the SVDigitizerParameterSetValue function in the DLL.
	SVDigitizerParameterSetValuePtr m_psvParameterSetValue;
	//This attribute holds the address to the SVDigitizerScanForCameras function in the DLL.
	SVDigitizerScanForCamerasPtr m_psvScanForCameras;
};

#include "SVDigitizerLoadLibraryClass.inl"
