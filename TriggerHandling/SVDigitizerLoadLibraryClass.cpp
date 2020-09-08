// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDigitizerLoadLibraryClass
// * .File Name       : $Workfile:   SVDigitizerLoadLibraryClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:24  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes


namespace SvTh
{
	SVDigitizerLoadLibraryClass::~SVDigitizerLoadLibraryClass()
	{
		Close();
	}

	/*
	This method opens the DLL and attaches all of the attributes to the addresses of the functions in the DLL.

	If the load and assignment was completed correctly, this method uses the p_pszCreate attribute to call the SVCreate function in the DLL.  The status returned from this call is returned by this method.

	If an error occurs, the Close method is called and this method will return the appropriate error code.
	*/
	HRESULT SVDigitizerLoadLibraryClass::Open(LPCTSTR p_szLibrary)
	{
		HRESULT l_hrOk = S_OK;

		if (nullptr == m_handle)
		{
			m_handle = ::LoadLibrary(p_szLibrary);
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);

			if (nullptr != m_handle)
			{
				m_pCreate = (SVCreatePtr) ::GetProcAddress(m_handle, "SVCreate");
				m_pDestroy = (SVDestroyPtr) ::GetProcAddress(m_handle, "SVDestroy");
				m_pGetCount = (SVDigitizerGetCountPtr) ::GetProcAddress(m_handle, "SVDigitizerGetCount");
				m_pGetHandle = (SVDigitizerGetHandlePtr) ::GetProcAddress(m_handle, "SVDigitizerGetHandle");
				m_pGetName = (SVDigitizerGetNamePtr) ::GetProcAddress(m_handle, "SVDigitizerGetName");
				m_pGetBufferWidth = (SVDigitizerGetBufferWidthPtr) ::GetProcAddress(m_handle, "SVDigitizerGetBufferWidth");
				m_pGetBufferHeight = (SVDigitizerGetBufferHeightPtr) ::GetProcAddress(m_handle, "SVDigitizerGetBufferHeight");
				m_pGetBufferFormat = (SVDigitizerGetBufferFormatPtr) ::GetProcAddress(m_handle, "SVDigitizerGetBufferFormat");
				m_pCreateBuffers = (SVDigitizerCreateBuffersPtr) ::GetProcAddress(m_handle, "SVDigitizerCreateBuffers");
				m_pRegisterBufferInterface = (SVDigitizerRegisterBufferInterfacePtr) ::GetProcAddress(m_handle, "SVDigitizerRegisterBufferInterface");
				m_pStart = (SVDigitizerStartPtr) ::GetProcAddress(m_handle, "SVDigitizerStart");
				m_pStop = (SVDigitizerStopPtr) ::GetProcAddress(m_handle, "SVDigitizerStop");
				m_pUnregisterBufferInterface = (SVDigitizerUnregisterBufferInterfacePtr) ::GetProcAddress(m_handle, "SVDigitizerUnregisterBufferInterface");
				m_pInternalTriggerEnable = (SVDigitizerInternalTriggerEnablePtr) ::GetProcAddress(m_handle, "SVDigitizerInternalTriggerEnable");
				m_pInternalTrigger = (SVDigitizerInternalTriggerPtr) ::GetProcAddress(m_handle, "SVDigitizerInternalTrigger");
				m_pDestroyBuffers = (SVDigitizerDestroyBuffersPtr) ::GetProcAddress(m_handle, "SVDigitizerDestroyBuffers");
				m_pSetParameters = (SVDigitizerSetParametersPtr) ::GetProcAddress(m_handle, "SVDigitizerSetParameters");
				m_pSetParameter = (SVDigitizerSetParameterPtr) ::GetProcAddress(m_handle, "SVDigitizerSetParameter");
				m_pGetParameter = (SVDigitizerGetParameterPtr) ::GetProcAddress(m_handle, "SVDigitizerGetParameter");

				m_pParameterGetList = (SVDigitizerParameterGetListPtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetList");
				m_pParameterGetName = (SVDigitizerParameterGetNamePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetName");
				m_pParameterGetValue = (SVDigitizerParameterGetValuePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetValue");
				m_pParameterSetValue = (SVDigitizerParameterSetValuePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterSetValue");

				m_pScanForCameras = (SVDigitizerScanForCamerasPtr) ::GetProcAddress(m_handle, "SVDigitizerScanForCameras");

				if (nullptr != m_pCreate &&
					nullptr != m_pDestroy &&
					nullptr != m_pGetCount &&
					nullptr != m_pGetHandle &&
					nullptr != m_pGetName &&
					nullptr != m_pGetBufferWidth &&
					nullptr != m_pGetBufferHeight &&
					nullptr != m_pGetBufferFormat &&
					nullptr != m_pCreateBuffers &&
					nullptr != m_pRegisterBufferInterface &&
					nullptr != m_pStart &&
					nullptr != m_pStop &&
					nullptr != m_pUnregisterBufferInterface &&
					nullptr != m_pInternalTriggerEnable &&
					nullptr != m_pInternalTrigger &&
					nullptr != m_pDestroyBuffers &&
					nullptr != m_pSetParameters &&
					nullptr != m_pSetParameter &&
					nullptr != m_pGetParameter
					)
				{
					l_hrOk = m_pCreate();
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}
		}

		if (S_OK != l_hrOk)
		{
			Close();
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvDestroy attribute to call the DLL SVDestroy method.  The DLL will be closed and all attributes will be set to their desired defaults.
	*/
	HRESULT SVDigitizerLoadLibraryClass::Close()
	{
		HRESULT l_hrOk = S_OK;

		if (nullptr != m_handle)
		{
			if (nullptr != m_pDestroy)
			{
				l_hrOk = m_pDestroy();
			}

			if (::FreeLibrary(m_handle))
			{
				// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
				Sleep(0);
				m_handle = nullptr;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}

		m_pCreate = nullptr;
		m_pDestroy = nullptr;
		m_pGetCount = nullptr;
		m_pGetHandle = nullptr;
		m_pGetName = nullptr;
		m_pGetBufferWidth = nullptr;
		m_pGetBufferHeight = nullptr;
		m_pGetBufferFormat = nullptr;
		m_pCreateBuffers = nullptr;
		m_pRegisterBufferInterface = nullptr;
		m_pStart = nullptr;
		m_pStop = nullptr;
		m_pUnregisterBufferInterface = nullptr;
		m_pInternalTriggerEnable = nullptr;
		m_pInternalTrigger = nullptr;
		m_pDestroyBuffers = nullptr;
		m_pSetParameters = nullptr;
		m_pSetParameter = nullptr;
		m_pGetParameter = nullptr;
		m_pParameterGetList = nullptr;
		m_pParameterGetName = nullptr;
		m_pParameterGetValue = nullptr;
		m_pParameterSetValue = nullptr;

		m_pScanForCameras = nullptr;

		return l_hrOk;
	}

	// cppcheck-suppress unusedFunction
	bool SVDigitizerLoadLibraryClass::IsLoaded() const
	{
		return (nullptr != m_handle);
	}

	// cppcheck-suppress unusedFunction
	HRESULT SVDigitizerLoadLibraryClass::GetLibraryName(BSTR *p_pbstrName)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_handle)
		{
			WCHAR szPath[MAX_PATH];

			if (::GetModuleFileNameW(m_handle, szPath, MAX_PATH))
			{
				l_hrOk = S_OK;
				WCHAR szFname[_MAX_FNAME];

				_wsplitpath(szPath, nullptr, nullptr, szFname, nullptr);

				_bstr_t fName(szFname);

				if (nullptr != *p_pbstrName)
				{
					::SysFreeString(*p_pbstrName);

					*p_pbstrName = nullptr;
				}

				// detach and assign it the the caller's variable
				*p_pbstrName = fName.Detach();
			}
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetCount attribute to call the DLL SVDigitizerGetCount function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetCount(unsigned long *p_pulCount)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetCount)
		{
			l_hrOk = m_pGetCount(p_pulCount);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetHandle attribute to call the DLL SVDigitizerGetHandle function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetHandle(SVHANDLE *p_phHandle, unsigned long p_ulIndex)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetHandle)
		{
			l_hrOk = m_pGetHandle(p_phHandle, p_ulIndex);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetName attribute to call the DLL SVDigitizerGetName function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetName(SVHANDLE p_hHandle, BSTR *p_pbstrName)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetName)
		{
			l_hrOk = m_pGetName(p_hHandle, p_pbstrName);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetBufferWidth attribute to call the DLL SVDigitizerGetBufferWidth function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetBufferWidth(SVHANDLE p_hHandle, unsigned long *p_pulWidth)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetBufferWidth)
		{
			l_hrOk = m_pGetBufferWidth(p_hHandle, p_pulWidth);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetBufferHeight attribute to call the DLL SVDigitizerGetBufferHeight function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetBufferHeight(SVHANDLE p_hHandle, unsigned long *p_pulHeight)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetBufferHeight)
		{
			l_hrOk = m_pGetBufferHeight(p_hHandle, p_pulHeight);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvGetBufferFormat attribute to call the DLL SVDigitizerGetBufferFormat function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetBufferFormat(SVHANDLE p_hHandle, int *p_piFormat)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pGetBufferFormat)
		{
			l_hrOk = m_pGetBufferFormat(p_hHandle, p_piFormat);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvCreateBuffers attribute to call the DLL SVDigitizerCreateBuffers function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::CreateBuffers(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pCreateBuffers)
		{
			l_hrOk = m_pCreateBuffers(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the m_pRegisterBufferInterface attribute to call the DLL SVDigitizerRegisterBufferInterface function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::RegisterBufferInterface(SVHANDLE p_hHandle, SVAcquisitionBufferInterface* pInterface)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pRegisterBufferInterface)
		{
			l_hrOk = m_pRegisterBufferInterface(p_hHandle, pInterface);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvStart attribute to call the DLL SVDigitizerStart function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::Start(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pStart)
		{
			l_hrOk = m_pStart(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvStop attribute to call the DLL SVDigitizerStop function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::Stop(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pStop)
		{
			l_hrOk = m_pStop(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the m_pUnregisterBufferInterface attribute to call the DLL SVDigitizerUnregisterBufferInterface function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::UnregisterBufferInterface(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pUnregisterBufferInterface)
		{
			l_hrOk = m_pUnregisterBufferInterface(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the m_pInternalTriggerEnable attribute to call the DLL SVDigitizerInternalTriggerEnable function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::InternalTriggerEnable(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pInternalTriggerEnable)
		{
			l_hrOk = m_pInternalTriggerEnable(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the m_pInternalTrigger attribute to call the DLL SVDigitizerInternalTrigger function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::InternalTrigger(SVHANDLE p_hHandle) const
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pInternalTrigger)
		{
			l_hrOk = m_pInternalTrigger(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvDestroyBuffers attribute to call the DLL SVDigitizerDestroyBuffers function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::DestroyBuffers(SVHANDLE p_hHandle)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pDestroyBuffers)
		{
			l_hrOk = m_pDestroyBuffers(p_hHandle);
		}

		return l_hrOk;
	}

	/*
	This method uses the p_psvSetParameters attribute to call the DLL SVDigitizerSetParameters function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::SetParameters(SVHANDLE p_hHandle, const SVDeviceParamCollection* p_pParameters)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pSetParameters)
		{
			l_hrOk = m_pSetParameters(p_hHandle, p_pParameters);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvSetParameter attribute to call the DLL SVDigitizerSetParameter function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::SetParameter(SVHANDLE p_hHandle, const SVDeviceParamWrapper* p_pParameter)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pSetParameter)
		{
			l_hrOk = m_pSetParameter(p_hHandle, p_pParameter);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvGetParameter attribute to call the DLL SVDigitizerGetParameter function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::GetParameter(SVHANDLE p_hHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pGetParameter)
		{
			l_hrOk = m_pGetParameter(p_hHandle, p_eParameter, p_ppParameter);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvParameterGetList attribute to call the DLL SVDigitizerParameterGetList function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::ParameterGetList(SVHANDLE p_hHandle, VARIANT *p_pvarName)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pParameterGetList)
		{
			l_hrOk = m_pParameterGetList(p_hHandle, p_pvarName);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvParameterGetName attribute to call the DLL SVDigitizerParameterGetName function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::ParameterGetName(SVHANDLE p_hHandle, int p_iParameterID, BSTR *p_pbstrName)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pParameterGetName)
		{
			l_hrOk = m_pParameterGetName(p_hHandle, p_iParameterID, p_pbstrName);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvParameterGetValue attribute to call the DLL SVDigitizerParameterGetValue function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::ParameterGetValue(SVHANDLE p_hHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarName)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pParameterGetValue)
		{
			l_hrOk = m_pParameterGetValue(p_hHandle, p_iParameterID, p_piParameterTypeID, p_pvarName);
		}
		return l_hrOk;
	}

	/*
	This method uses the p_psvParameterSetList attribute to call the DLL SVDigitizerParameterSetList function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
	*/
	HRESULT SVDigitizerLoadLibraryClass::ParameterSetValue(SVHANDLE p_hHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarName)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pParameterSetValue)
		{
			l_hrOk = m_pParameterSetValue(p_hHandle, p_iParameterID, p_iParameterTypeID, p_pvarName);
		}
		return l_hrOk;
	}

	// Optional Interface to Scan for existance of cameras
	HRESULT SVDigitizerLoadLibraryClass::ScanForCameras()
	{
		HRESULT l_hrOk = E_NOTIMPL;

		if (nullptr != m_pScanForCameras)
		{
			l_hrOk = m_pScanForCameras();
		}
		return l_hrOk;
	}

} //namespace SvTh
