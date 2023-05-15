// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes


class SVAcquisitionBufferInterface;
class SVDeviceParamCollection;

namespace SvTrig
{
	typedef HRESULT (WINAPI *SVCreatePtr)(void);
	typedef HRESULT (WINAPI *SVDestroyPtr)(void);
	typedef unsigned long (WINAPI *SVDigitizerGetHandlePtr)(unsigned long);
	typedef unsigned long (WINAPI *SVDigitizerGetCountPtr)();
	typedef _variant_t (WINAPI *SVDigitizerGetNamePtr)(unsigned long);
	typedef unsigned long (WINAPI *SVDigitizerGetBufferWidthPtr)(unsigned long);
	typedef unsigned long (WINAPI *SVDigitizerGetBufferHeightPtr)(unsigned long);
	typedef int (WINAPI *SVDigitizerGetBufferFormatPtr)(unsigned long);
	typedef HRESULT (WINAPI *SVDigitizerCreateBuffersPtr)(unsigned long);
	typedef HRESULT (WINAPI *SVDigitizerRegisterBufferInterfacePtr)(unsigned long, SVAcquisitionBufferInterface*);
	typedef HRESULT (WINAPI *SVDigitizerStartPtr)(unsigned long);
	typedef HRESULT (WINAPI *SVDigitizerStopPtr)(unsigned long);
	typedef HRESULT (WINAPI *SVDigitizerUnregisterBufferInterfacePtr)(unsigned long);
	typedef HRESULT (WINAPI *SVDigitizerInternalTriggerPtr)(unsigned long, LPCTSTR, DWORD);
	typedef HRESULT (WINAPI *SVDigitizerDestroyBuffersPtr)(unsigned long);
	typedef _variant_t (WINAPI *SVDigitizerParameterGetNamePtr)(unsigned long, unsigned long);
	typedef _variant_t (WINAPI *SVDigitizerParameterGetValuePtr)(unsigned long, int);
	typedef HRESULT (WINAPI *SVDigitizerParameterSetValuePtr)(unsigned long, int, const _variant_t&);
	typedef _variant_t (WINAPI* SVDigitizerParameterGetListPtr)(unsigned long);
	typedef HRESULT (WINAPI* SVDigitizerSetParametersPtr)(unsigned long, const SVDeviceParamCollection&);
	typedef void (WINAPI *SVDigitizerScanForCamerasPtr)();

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
		_variant_t GetLibraryName() const;

		unsigned long GetHandle(long index) const;
		unsigned long GetCount() const;
		_variant_t GetName(unsigned long digitizerHandle) const;
		unsigned long GetBufferWidth(unsigned long digitizerHandle) const;
		unsigned long GetBufferHeight(unsigned long digitizerHandle) const;
		int GetBufferFormat(unsigned long digitizerHandle) const;
		HRESULT CreateBuffers(unsigned long digitizerHandle);
		HRESULT DestroyBuffers(unsigned long digitizerHandle);
		HRESULT RegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface);
		HRESULT UnregisterBufferInterface(unsigned long digitizerHandle);
		HRESULT Start(unsigned long digitizerHandle);
		HRESULT Stop(unsigned long digitizerHandle);

		HRESULT InternalTrigger(unsigned long digitizerHandle, LPCTSTR pAcquisitionFile, DWORD preAcqPause) const;

		_variant_t ParameterGetName(unsigned long digitizerHandle, int parameterID) const;
		_variant_t ParameterGetValue(unsigned long digitizerHandle, int parameterID) const;
		HRESULT ParameterSetValue(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue);
		_variant_t ParameterGetList(unsigned long digitizerHandle) const;
		HRESULT SetParameters(unsigned long digitizerHandle, const SVDeviceParamCollection& rParameters);

		void ScanForCameras();

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
		//This attribute holds the address to the SVDigitizerDestroyBuffers function in the DLL.
		SVDigitizerDestroyBuffersPtr m_pDestroyBuffers{ nullptr };
		//This attribute holds the address to the SVDigitizerRegisterBufferInterface function in the DLL.
		SVDigitizerRegisterBufferInterfacePtr m_pRegisterBufferInterface{ nullptr };
		//This attribute holds the address to the SVDigitizerUnregisterBufferInterface function in the DLL.
		SVDigitizerUnregisterBufferInterfacePtr m_pUnregisterBufferInterface{ nullptr };
		//This attribute holds the address to the SVDigitizerStart function in the DLL.
		SVDigitizerStartPtr m_pStart{ nullptr };
		//This attribute holds the address to the SVDigitizerStop function in the DLL.
		SVDigitizerStopPtr m_pStop{ nullptr };
		//This attribute holds the address to the SVDigitizerInternalTrigger function in the DLL.
		SVDigitizerInternalTriggerPtr m_pInternalTrigger{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetName function in the DLL.
		SVDigitizerParameterGetNamePtr m_pParameterGetName{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetValue function in the DLL.
		SVDigitizerParameterGetValuePtr m_pParameterGetValue{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterSetValue function in the DLL.
		SVDigitizerParameterSetValuePtr m_pParameterSetValue{ nullptr };
		//This attribute holds the address to the SVDigitizerParameterGetList function in the DLL.
		SVDigitizerParameterGetListPtr m_pParameterGetList{ nullptr };
		//This attribute holds the address to the SVDigitizerSetParameters function in the DLL.
		SVDigitizerSetParametersPtr m_pSetParameters{ nullptr };
		//This attribute holds the address to the SVDigitizerScanForCameras function in the DLL.
		SVDigitizerScanForCamerasPtr m_pScanForCameras{ nullptr };
	};

} //namespace SvTrig

