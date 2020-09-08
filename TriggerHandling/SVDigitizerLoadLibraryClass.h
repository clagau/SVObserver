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
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#pragma endregion Includes

namespace SvTh
{
	typedef HRESULT(WINAPI *SVCreatePtr)(void);
	typedef HRESULT(WINAPI *SVDestroyPtr)(void);
	typedef HRESULT(WINAPI *SVDigitizerGetCountPtr)(unsigned long *);
	typedef HRESULT(WINAPI *SVDigitizerGetHandlePtr)(SVHANDLE *, unsigned long);
	typedef HRESULT(WINAPI *SVDigitizerGetNamePtr)(SVHANDLE, BSTR *);
	typedef HRESULT(WINAPI *SVDigitizerGetBufferWidthPtr)(SVHANDLE, unsigned long *);
	typedef HRESULT(WINAPI *SVDigitizerGetBufferHeightPtr)(SVHANDLE, unsigned long *);
	typedef HRESULT(WINAPI *SVDigitizerGetBufferFormatPtr)(SVHANDLE, int *);
	typedef HRESULT(WINAPI *SVDigitizerCreateBuffersPtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerRegisterBufferInterfacePtr)(SVHANDLE, SVAcquisitionBufferInterface*);
	typedef HRESULT(WINAPI *SVDigitizerStartPtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerStopPtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerUnregisterBufferInterfacePtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerInternalTriggerEnablePtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerInternalTriggerPtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerDestroyBuffersPtr)(SVHANDLE);
	typedef HRESULT(WINAPI *SVDigitizerSetParametersPtr)(SVHANDLE, const SVDeviceParamCollection*);
	typedef HRESULT(WINAPI *SVDigitizerSetParameterPtr)(SVHANDLE, const SVDeviceParamWrapper*);
	typedef HRESULT(WINAPI *SVDigitizerGetParameterPtr)(SVHANDLE, SVDeviceParamEnum, SVDeviceParamWrapper**);
	typedef HRESULT(WINAPI *SVDigitizerParameterGetListPtr)(SVHANDLE, VARIANT *);
	typedef HRESULT(WINAPI *SVDigitizerParameterGetNamePtr)(SVHANDLE, unsigned long, BSTR *);
	typedef HRESULT(WINAPI *SVDigitizerParameterGetValuePtr)(SVHANDLE, int, int *, VARIANT *);
	typedef HRESULT(WINAPI *SVDigitizerParameterSetValuePtr)(SVHANDLE, int, int, VARIANT *);
	typedef HRESULT(WINAPI *SVDigitizerScanForCamerasPtr)();

	/*
	This class is used to interface into the digitizer DLLs.  The methods of this class directly relate to the exported function of the DLL.
	*/
	class SVDigitizerLoadLibraryClass
	{
	public:
		SVDigitizerLoadLibraryClass() = default;
		~SVDigitizerLoadLibraryClass();

		HRESULT Open(LPCTSTR p_szLibrary);
		HRESULT Close();

		bool IsLoaded() const;
		HRESULT GetLibraryName(BSTR *p_pbstrName);

		HRESULT GetCount(unsigned long *p_pulCount);
		HRESULT GetHandle(SVHANDLE *p_phHandle, unsigned long p_ulIndex);
		HRESULT GetName(SVHANDLE p_hHandle, BSTR *p_pbstrName);
		HRESULT GetBufferWidth(SVHANDLE p_hHandle, unsigned long *p_pulWidth);
		HRESULT GetBufferHeight(SVHANDLE p_hHandle, unsigned long *p_pulHeight);
		HRESULT GetBufferFormat(SVHANDLE p_hHandle, int *p_piFormat);
		HRESULT CreateBuffers(SVHANDLE p_hHandle);
		HRESULT RegisterBufferInterface(SVHANDLE p_hHandle, SVAcquisitionBufferInterface* _pInterface);
		HRESULT Start(SVHANDLE p_hHandle);
		HRESULT Stop(SVHANDLE p_hHandle);
		HRESULT UnregisterBufferInterface(SVHANDLE p_hHandle);

		HRESULT InternalTriggerEnable(SVHANDLE p_hHandle);
		HRESULT InternalTrigger(SVHANDLE p_hHandle) const;

		HRESULT DestroyBuffers(SVHANDLE p_hHandle);
		HRESULT SetParameters(SVHANDLE p_hHandle, const SVDeviceParamCollection* p_pParameters);
		HRESULT SetParameter(SVHANDLE p_hHandle, const SVDeviceParamWrapper* p_pParameter);
		HRESULT GetParameter(SVHANDLE p_hHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter);
		HRESULT ParameterGetList(SVHANDLE p_hHandle, VARIANT *p_pvarName);
		HRESULT ParameterGetName(SVHANDLE p_hHandle, int p_iParameterID, BSTR *p_pbstrName);
		HRESULT ParameterGetValue(SVHANDLE p_hHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarName);
		HRESULT ParameterSetValue(SVHANDLE p_hHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarName);

		HRESULT ScanForCameras();

	private:
		//This attribute holds the handle to the open DLL.
		HMODULE m_handle{ nullptr };

		//This attribute holds the address to the SVCreate function in the DLL.
		SVCreatePtr m_pCreate{ nullptr };
		//This attribute holds the address to the SVDestroy function in the DLL.
		SVDestroyPtr m_pDestroy{ nullptr };
		//This attribute holds the address to the SVDigitizerGetCount function in the DLL.
		SVDigitizerGetCountPtr m_pGetCount{ nullptr };
		//This attribute holds the address to the SVDigitizerGetHandle function in the DLL.
		SVDigitizerGetHandlePtr m_pGetHandle{ nullptr };
		//This attribute holds the address to the SVDigitizerGetName function in the DLL.
		SVDigitizerGetNamePtr m_pGetName{ nullptr };
		//This attribute holds the address to the SVDigitizerGetBufferWidth function in the DLL.
		SVDigitizerGetBufferWidthPtr m_pGetBufferWidth{ nullptr };
		//This attribute holds the address to the SVDigitizerGetBufferHeight function in the DLL.
		SVDigitizerGetBufferHeightPtr m_pGetBufferHeight{ nullptr };
		//This attribute holds the address to the SVDigitizerGetBufferFormat function in the DLL.
		SVDigitizerGetBufferFormatPtr m_pGetBufferFormat{ nullptr };
		//This attribute holds the address to the SVDigitizerCreateBuffers function in the DLL.
		SVDigitizerCreateBuffersPtr m_pCreateBuffers{ nullptr };
		//This attribute holds the address to the SVDigitizerRegisterBufferInterface function in the DLL.
		SVDigitizerRegisterBufferInterfacePtr m_pRegisterBufferInterface{ nullptr };
		//This attribute holds the address to the SVDigitizerStart function in the DLL.
		SVDigitizerStartPtr m_pStart{ nullptr };
		//This attribute holds the address to the SVDigitizerStop function in the DLL.
		SVDigitizerStopPtr m_pStop{ nullptr };
		//This attribute holds the address to the SVDigitizerUnregisterBufferInterface function in the DLL.
		SVDigitizerUnregisterBufferInterfacePtr m_pUnregisterBufferInterface{ nullptr };
		//This attribute holds the address to the SVDigitizerInternalTriggerEnable function in the DLL.
		SVDigitizerInternalTriggerEnablePtr m_pInternalTriggerEnable{ nullptr };
		//This attribute holds the address to the SVDigitizerInternalTrigger function in the DLL.
		SVDigitizerInternalTriggerPtr m_pInternalTrigger{ nullptr };
		//This attribute holds the address to the SVDigitizerDestroyBuffers function in the DLL.
		SVDigitizerDestroyBuffersPtr m_pDestroyBuffers{ nullptr };
		//This attribute holds the address to the SVDigitizerSetParameters function in the DLL.
		SVDigitizerSetParametersPtr m_pSetParameters{ nullptr };
		//This attribute holds the address to the SVDigitizerSetParameter function in the DLL.
		SVDigitizerSetParameterPtr m_pSetParameter{ nullptr };
		//This attribute holds the address to the SVDigitizerGetParameter function in the DLL.
		SVDigitizerGetParameterPtr m_pGetParameter{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetList function in the DLL.
		SVDigitizerParameterGetListPtr m_pParameterGetList{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetName function in the DLL.
		SVDigitizerParameterGetNamePtr m_pParameterGetName{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetValue function in the DLL.
		SVDigitizerParameterGetValuePtr m_pParameterGetValue{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterSetValue function in the DLL.
		SVDigitizerParameterSetValuePtr m_pParameterSetValue{ nullptr };
		//This attribute holds the address to the SVDigitizerScanForCameras function in the DLL.
		SVDigitizerScanForCamerasPtr m_pScanForCameras{ nullptr };
	};

} //namespace SvTh

