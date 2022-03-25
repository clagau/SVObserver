// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#pragma endregion Includes


namespace SvTrig
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
				m_pDestroyBuffers = (SVDigitizerDestroyBuffersPtr) ::GetProcAddress(m_handle, "SVDigitizerDestroyBuffers");
				m_pRegisterBufferInterface = (SVDigitizerRegisterBufferInterfacePtr) ::GetProcAddress(m_handle, "SVDigitizerRegisterBufferInterface");
				m_pUnregisterBufferInterface = (SVDigitizerUnregisterBufferInterfacePtr) ::GetProcAddress(m_handle, "SVDigitizerUnregisterBufferInterface");
				m_pStart = (SVDigitizerStartPtr) ::GetProcAddress(m_handle, "SVDigitizerStart");
				m_pStop = (SVDigitizerStopPtr) ::GetProcAddress(m_handle, "SVDigitizerStop");
				m_pInternalTrigger = (SVDigitizerInternalTriggerPtr) ::GetProcAddress(m_handle, "SVDigitizerInternalTrigger");

				m_pParameterGetList = (SVDigitizerParameterGetListPtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetList");
				m_pParameterGetName = (SVDigitizerParameterGetNamePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetName");
				m_pParameterGetValue = (SVDigitizerParameterGetValuePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterGetValue");
				m_pParameterSetValue = (SVDigitizerParameterSetValuePtr) ::GetProcAddress(m_handle, "SVDigitizerParameterSetValue");
				m_pSetParameters = (SVDigitizerSetParametersPtr) ::GetProcAddress(m_handle, "SVDigitizerSetParameters");

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
					nullptr != m_pInternalTrigger &&
					nullptr != m_pDestroyBuffers
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
		m_pInternalTrigger = nullptr;
		m_pDestroyBuffers = nullptr;
		m_pSetParameters = nullptr;
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
	_variant_t SVDigitizerLoadLibraryClass::GetLibraryName() const
	{
		_variant_t result;

		if (nullptr != m_handle)
		{
			WCHAR szPath[MAX_PATH];

			if (::GetModuleFileNameW(m_handle, szPath, MAX_PATH))
			{
				WCHAR szFname[_MAX_FNAME];

				_wsplitpath(szPath, nullptr, nullptr, szFname, nullptr);

				result = szFname;
			}
		}
		return result;
	}

	unsigned long SVDigitizerLoadLibraryClass::GetHandle(long index) const
	{
		unsigned long result{ 0UL };

		if (nullptr != m_pGetHandle)
		{
			result = m_pGetHandle(index);
		}
		return result;
	}

	unsigned long SVDigitizerLoadLibraryClass::GetCount() const
	{
		unsigned long result{ 0UL };

		if (nullptr != m_pGetCount)
		{
			result = m_pGetCount();
		}
		return result;
	}

	_variant_t SVDigitizerLoadLibraryClass::GetName(unsigned long digitizerHandle) const
	{
		_variant_t result;
		if (nullptr != m_pGetName)
		{
			result = m_pGetName(digitizerHandle);
		}
		return result;
	}

	unsigned long SVDigitizerLoadLibraryClass::GetBufferWidth(unsigned long digitizerHandle) const
	{
		unsigned long result{ 0UL };

		if (nullptr != m_pGetBufferWidth)
		{
			result = m_pGetBufferWidth(digitizerHandle);
		}
		return result;
	}

	unsigned long SVDigitizerLoadLibraryClass::GetBufferHeight(unsigned long digitizerHandle) const
	{
		unsigned long result{ 0UL };

		if (nullptr != m_pGetBufferHeight)
		{
			result = m_pGetBufferHeight(digitizerHandle);
		}
		return result;
	}

	int SVDigitizerLoadLibraryClass::GetBufferFormat(unsigned long digitizerHandle) const
	{
		int result{0};

		if (nullptr != m_pGetBufferFormat)
		{
			result = m_pGetBufferFormat(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::CreateBuffers(unsigned long digitizerHandle)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pCreateBuffers)
		{
			result = m_pCreateBuffers(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::DestroyBuffers(unsigned long digitizerHandle)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pDestroyBuffers)
		{
			result = m_pDestroyBuffers(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::RegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pRegisterBufferInterface)
		{
			result = m_pRegisterBufferInterface(digitizerHandle, pInterface);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::UnregisterBufferInterface(unsigned long digitizerHandle)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pUnregisterBufferInterface)
		{
			result = m_pUnregisterBufferInterface(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::Start(unsigned long digitizerHandle)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pStart)
		{
			result = m_pStart(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::Stop(unsigned long digitizerHandle)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pStop)
		{
			result = m_pStop(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::InternalTrigger(unsigned long digitizerHandle, LPCTSTR pAcquisitionFile) const
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pInternalTrigger)
		{
			result = m_pInternalTrigger(digitizerHandle, pAcquisitionFile);
		}
		return result;
	}

	_variant_t SVDigitizerLoadLibraryClass::ParameterGetName(unsigned long digitizerHandle, int parameterID) const
	{
		_variant_t result;

		if (nullptr != m_pParameterGetName)
		{
			result = m_pParameterGetName(digitizerHandle, parameterID);
		}
		return result;
	}

	_variant_t SVDigitizerLoadLibraryClass::ParameterGetValue(unsigned long digitizerHandle, int parameterID) const
	{
		_variant_t result;
		if (nullptr != m_pParameterGetValue)
		{
			result = m_pParameterGetValue(digitizerHandle, parameterID);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::ParameterSetValue(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue)
	{
		HRESULT result{ E_FAIL };

		if (nullptr != m_pParameterSetValue)
		{
			result = m_pParameterSetValue(digitizerHandle, parameterID, rValue);
		}
		return result;
	}

	_variant_t SVDigitizerLoadLibraryClass::ParameterGetList(unsigned long digitizerHandle) const
	{
		variant_t result;
		
		if (nullptr != m_pParameterGetList)
		{
			result = m_pParameterGetList(digitizerHandle);
		}
		return result;
	}

	HRESULT SVDigitizerLoadLibraryClass::SetParameters(unsigned long digitizerHandle, const SVDeviceParamCollection& rParameters)
	{
		HRESULT result{ E_FAIL };
		
		if (nullptr != m_pSetParameters)
		{
			result = m_pSetParameters(digitizerHandle, rParameters);
		}
		return result;
	}

	void SVDigitizerLoadLibraryClass::ScanForCameras()
	{
		if (nullptr != m_pScanForCameras)
		{
			m_pScanForCameras();
		}
	}

} //namespace SvTrig
