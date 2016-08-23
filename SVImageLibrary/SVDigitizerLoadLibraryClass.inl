// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDigitizerLoadLibraryClass
// * .File Name       : $Workfile:   SVDigitizerLoadLibraryClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:24  $
// ******************************************************************************

/*
This constructor initializes all of the local attributes to the desired default values.
*/
inline SVDigitizerLoadLibraryClass::SVDigitizerLoadLibraryClass()
: m_hmHandle(nullptr)
, m_psvCreate(nullptr)
, m_psvDestroy(nullptr)
, m_psvGetCount(nullptr)
, m_psvGetHandle(nullptr)
, m_psvGetName(nullptr)
, m_psvLoadCameraFiles(nullptr)
, m_psvGetBufferWidth(nullptr)
, m_psvGetBufferHeight(nullptr)
, m_psvGetBufferFormat(nullptr)
, m_psvCreateBuffers(nullptr)
, m_psvRegisterBufferInterface(nullptr)
, m_psvStart(nullptr)
, m_psvStop(nullptr)
, m_psvUnregisterBufferInterface(nullptr)
, m_psvDestroyBuffers(nullptr)
, m_psvUnloadCameraFile(nullptr)
, m_psvSetParameters(nullptr)
, m_psvSetParameter(nullptr)
, m_psvGetParameter(nullptr)
, m_psvDestroyParameter(nullptr)
, m_psvParameterGetList(nullptr)
, m_psvParameterGetName(nullptr)
, m_psvParameterGetValue(nullptr)
, m_psvParameterSetValue(nullptr)
, m_psvScanForCameras(nullptr)
, m_psvInternalTriggerEnable(nullptr)
, m_psvInternalTrigger(nullptr)
, m_psvInternalTriggerRegister(nullptr)
, m_psvInternalTriggerUnregister(nullptr)
, m_psvInternalTriggerUnregisterAll(nullptr)
{
}

/*
This method calls the Close method to clean-up the local resources before the object is destroyed.
*/
inline SVDigitizerLoadLibraryClass::~SVDigitizerLoadLibraryClass()
{
	Close();
}

/*
This method opens the DLL and attaches all of the attributes to the addresses of the functions in the DLL.  

If the load and assignment was completed correctly, this method uses the p_pszCreate attribute to call the SVCreate function in the DLL.  The status returned from this call is returned by this method.

If an error occurs, the Close method is called and this method will return the appropriate error code.
*/
inline HRESULT SVDigitizerLoadLibraryClass::Open(LPCTSTR p_szLibrary)
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == m_hmHandle )
	{
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( nullptr != m_hmHandle )
		{
			m_psvCreate = (SVCreatePtr) ::GetProcAddress( m_hmHandle, "SVCreate" );
			m_psvDestroy = (SVDestroyPtr) ::GetProcAddress( m_hmHandle, "SVDestroy" );
			m_psvGetCount = (SVDigitizerGetCountPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetCount" );
			m_psvGetHandle = (SVDigitizerGetHandlePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetHandle" );
			m_psvGetName = (SVDigitizerGetNamePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetName" );
			m_psvLoadCameraFiles = (SVDigitizerLoadCameraFilesPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerLoadCameraFiles" );
			m_psvGetBufferWidth = (SVDigitizerGetBufferWidthPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetBufferWidth" );
			m_psvGetBufferHeight = (SVDigitizerGetBufferHeightPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetBufferHeight" );
			m_psvGetBufferFormat = (SVDigitizerGetBufferFormatPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetBufferFormat" );
			m_psvCreateBuffers = (SVDigitizerCreateBuffersPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerCreateBuffers" );
			m_psvRegisterBufferInterface = (SVDigitizerRegisterBufferInterfacePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerRegisterBufferInterface" );
			m_psvStart = (SVDigitizerStartPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerStart" );
			m_psvStop = (SVDigitizerStopPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerStop" );
			m_psvUnregisterBufferInterface = (SVDigitizerUnregisterBufferInterfacePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerUnregisterBufferInterface" );
			m_psvInternalTriggerEnable = (SVDigitizerInternalTriggerEnablePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerInternalTriggerEnable" );
			m_psvInternalTrigger = (SVDigitizerInternalTriggerPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerInternalTrigger" );
			m_psvInternalTriggerRegister = (SVDigitizerInternalTriggerRegisterPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerInternalTriggerRegister" );
			m_psvInternalTriggerUnregister = (SVDigitizerInternalTriggerUnregisterPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerInternalTriggerUnregister" );
			m_psvInternalTriggerUnregisterAll = (SVDigitizerInternalTriggerUnregisterAllPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerInternalTriggerUnregisterAll" );
			m_psvDestroyBuffers = (SVDigitizerDestroyBuffersPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerDestroyBuffers" );
			m_psvUnloadCameraFile = (SVDigitizerUnloadCameraFilePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerUnloadCameraFile" );
			m_psvSetParameters = (SVDigitizerSetParametersPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerSetParameters" );
			m_psvSetParameter = (SVDigitizerSetParameterPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerSetParameter" );
			m_psvGetParameter = (SVDigitizerGetParameterPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerGetParameter" );
			m_psvDestroyParameter = (SVDigitizerDestroyParameterPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerDestroyParameter" );

			m_psvParameterGetList = (SVDigitizerParameterGetListPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerParameterGetList" );
			m_psvParameterGetName = (SVDigitizerParameterGetNamePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerParameterGetName" );
			m_psvParameterGetValue = (SVDigitizerParameterGetValuePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerParameterGetValue" );
			m_psvParameterSetValue = (SVDigitizerParameterSetValuePtr) ::GetProcAddress( m_hmHandle, "SVDigitizerParameterSetValue" );

			m_psvScanForCameras = (SVDigitizerScanForCamerasPtr) ::GetProcAddress( m_hmHandle, "SVDigitizerScanForCameras" );

			if ( nullptr != m_psvCreate &&
			     nullptr != m_psvDestroy &&
			     nullptr != m_psvGetCount &&
			     nullptr != m_psvGetHandle &&
			     nullptr != m_psvGetName &&
			     nullptr != m_psvLoadCameraFiles &&
			     nullptr != m_psvGetBufferWidth &&
			     nullptr != m_psvGetBufferHeight &&
			     nullptr != m_psvGetBufferFormat &&
			     nullptr != m_psvCreateBuffers &&
			     nullptr != m_psvRegisterBufferInterface &&
			     nullptr != m_psvStart &&
			     nullptr != m_psvStop &&
			     nullptr != m_psvUnregisterBufferInterface &&
				 nullptr != m_psvInternalTriggerEnable &&
				 nullptr != m_psvInternalTrigger &&
				 nullptr != m_psvInternalTriggerRegister &&
				 nullptr != m_psvInternalTriggerUnregister &&
				 nullptr != m_psvInternalTriggerUnregisterAll &&
			     nullptr != m_psvDestroyBuffers &&
			     nullptr != m_psvUnloadCameraFile &&
			     nullptr != m_psvSetParameters &&
			     nullptr != m_psvSetParameter &&
			     nullptr != m_psvGetParameter &&
			     nullptr != m_psvDestroyParameter
				 )
			{
				l_hrOk = m_psvCreate();
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( S_OK != l_hrOk )
	{
		Close();
	}

	return l_hrOk;
}

/*
This method uses the p_psvDestroy attribute to call the DLL SVDestroy method.  The DLL will be closed and all attributes will be set to their desired defaults.
*/
inline HRESULT SVDigitizerLoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr != m_hmHandle )
	{
		if ( nullptr != m_psvDestroy )
		{
			l_hrOk = m_psvDestroy();
		}

		if ( ::FreeLibrary( m_hmHandle ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			m_hmHandle = nullptr;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	m_psvCreate = nullptr;
	m_psvDestroy = nullptr;
	m_psvGetCount = nullptr;
	m_psvGetHandle = nullptr;
	m_psvGetName = nullptr;
	m_psvLoadCameraFiles = nullptr;
	m_psvGetBufferWidth = nullptr;
	m_psvGetBufferHeight = nullptr;
	m_psvGetBufferFormat = nullptr;
	m_psvCreateBuffers = nullptr;
	m_psvRegisterBufferInterface = nullptr;
	m_psvStart = nullptr;
	m_psvStop = nullptr;
	m_psvUnregisterBufferInterface = nullptr;
	m_psvInternalTriggerEnable = nullptr;
	m_psvInternalTrigger = nullptr;
	m_psvInternalTriggerRegister = nullptr;
	m_psvInternalTriggerUnregister = nullptr;
	m_psvInternalTriggerUnregisterAll = nullptr;
	m_psvDestroyBuffers = nullptr;
	m_psvUnloadCameraFile = nullptr;
	m_psvSetParameters = nullptr;
	m_psvSetParameter = nullptr;
	m_psvGetParameter = nullptr;
	m_psvDestroyParameter = nullptr;
	m_psvParameterGetList = nullptr;
	m_psvParameterGetName = nullptr;
	m_psvParameterGetValue = nullptr;
	m_psvParameterSetValue = nullptr;

	m_psvScanForCameras = nullptr;

	return l_hrOk;
}

/*
This method return the loaded state of the DLL.
*/
inline bool SVDigitizerLoadLibraryClass::IsLoaded() const
{
	return (nullptr != m_hmHandle);
}

/*
This method return the name of the DLL.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetLibraryName( BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_hmHandle )
	{
		WCHAR szPath[MAX_PATH]; 
		
		if ( ::GetModuleFileNameW( m_hmHandle, szPath, MAX_PATH  ) )
		{
			l_hrOk = S_OK;
			WCHAR szFname[_MAX_FNAME];

			_wsplitpath( szPath, nullptr, nullptr, szFname, nullptr );

			_bstr_t fName( szFname );

			if ( nullptr != *p_pbstrName )
			{
				::SysFreeString( *p_pbstrName );

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
inline HRESULT SVDigitizerLoadLibraryClass::GetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetCount )
	{
		l_hrOk = m_psvGetCount( p_pulCount );
	}

	return l_hrOk;
}

/*
This method uses the p_psvGetHandle attribute to call the DLL SVDigitizerGetHandle function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetHandle( SVHANDLE *p_phHandle, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetHandle )
	{
		l_hrOk = m_psvGetHandle( p_phHandle, p_ulIndex );
	}

	return l_hrOk;
}

/*
This method uses the p_psvGetName attribute to call the DLL SVDigitizerGetName function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetName( SVHANDLE p_hHandle, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetName )
	{
		l_hrOk = m_psvGetName( p_hHandle, p_pbstrName );
	}

	return l_hrOk;
}

/*
This method uses the p_psvLoadCameraFiles attribute to call the DLL SVDigitizerLoadCameraFiles function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::LoadCameraFiles( SVHANDLE p_hHandle, SAFEARRAY* p_psaFileNames )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvLoadCameraFiles )
	{
		l_hrOk = m_psvLoadCameraFiles( p_hHandle, p_psaFileNames );
	}

	return l_hrOk;
}

/*
This method uses the p_psvGetBufferWidth attribute to call the DLL SVDigitizerGetBufferWidth function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetBufferWidth( SVHANDLE p_hHandle, unsigned long *p_pulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetBufferWidth )
	{
		l_hrOk = m_psvGetBufferWidth( p_hHandle, p_pulWidth );
	}

	return l_hrOk;
}

/*
This method uses the p_psvGetBufferHeight attribute to call the DLL SVDigitizerGetBufferHeight function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetBufferHeight( SVHANDLE p_hHandle, unsigned long *p_pulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetBufferHeight )
	{
		l_hrOk = m_psvGetBufferHeight( p_hHandle, p_pulHeight );
	}

	return l_hrOk;
}

/*
This method uses the p_psvGetBufferFormat attribute to call the DLL SVDigitizerGetBufferFormat function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetBufferFormat( SVHANDLE p_hHandle, int *p_piFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetBufferFormat )
	{
		l_hrOk = m_psvGetBufferFormat( p_hHandle, p_piFormat );
	}

	return l_hrOk;
}

/*
This method uses the p_psvCreateBuffers attribute to call the DLL SVDigitizerCreateBuffers function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::CreateBuffers( SVHANDLE p_hHandle, unsigned long p_ulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvCreateBuffers )
	{
		l_hrOk = m_psvCreateBuffers( p_hHandle, p_ulCount );
	}

	return l_hrOk;
}

/*
This method uses the m_psvRegisterBufferInterface attribute to call the DLL SVDigitizerRegisterBufferInterface function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::RegisterBufferInterface( SVHANDLE p_hHandle, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvRegisterBufferInterface )
	{
		l_hrOk = m_psvRegisterBufferInterface( p_hHandle, p_pInterface );
	}

	return l_hrOk;
}

/*
This method uses the p_psvStart attribute to call the DLL SVDigitizerStart function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::Start( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvStart )
	{
		l_hrOk = m_psvStart( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the p_psvStop attribute to call the DLL SVDigitizerStop function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::Stop( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvStop )
	{
		l_hrOk = m_psvStop( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the m_psvUnregisterBufferInterface attribute to call the DLL SVDigitizerUnregisterBufferInterface function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::UnregisterBufferInterface( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvUnregisterBufferInterface )
	{
		l_hrOk = m_psvUnregisterBufferInterface( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerEnable attribute to call the DLL SVDigitizerInternalTriggerEnable function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerEnable( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvInternalTriggerEnable )
	{
		l_hrOk = m_psvInternalTriggerEnable( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTrigger attribute to call the DLL SVDigitizerInternalTrigger function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTrigger( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvInternalTrigger )
	{
		l_hrOk = m_psvInternalTrigger( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerRegister attribute to call the DLL SVDigitizerInternalTriggerRegister function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerRegister( SVHANDLE p_hHandle, const SvTh::TriggerDispatcher& rDispatcher )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvInternalTriggerRegister )
	{
		l_hrOk = m_psvInternalTriggerRegister( p_hHandle, rDispatcher);
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerUnregister attribute to call the DLL SVDigitizerInternalTriggerUnregister function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerUnregister( SVHANDLE p_hHandle, const SvTh::TriggerDispatcher& rDispatcher )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvInternalTriggerUnregister )
	{
		l_hrOk = m_psvInternalTriggerUnregister( p_hHandle, rDispatcher);
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerUnregisterAll attribute to call the DLL SVDigitizerInternalTriggerUnregisterAll function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerUnregisterAll( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvInternalTriggerUnregisterAll )
	{
		l_hrOk = m_psvInternalTriggerUnregisterAll( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the p_psvDestroyBuffers attribute to call the DLL SVDigitizerDestroyBuffers function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::DestroyBuffers( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvDestroyBuffers )
	{
		l_hrOk = m_psvDestroyBuffers( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the p_psvUnloadCameraFile attribute to call the DLL SVDigitizerUnloadCameraFile function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::UnloadCameraFile( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvUnloadCameraFile )
	{
		l_hrOk = m_psvUnloadCameraFile( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the p_psvSetParameters attribute to call the DLL SVDigitizerSetParameters function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::SetParameters( SVHANDLE p_hHandle, const SVDeviceParamCollection* p_pParameters )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvSetParameters )
	{
		l_hrOk = m_psvSetParameters( p_hHandle, p_pParameters );
	}
	return l_hrOk;
}

/*
This method uses the p_psvSetParameter attribute to call the DLL SVDigitizerSetParameter function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::SetParameter( SVHANDLE p_hHandle, const SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvSetParameter )
	{
		l_hrOk = m_psvSetParameter( p_hHandle, p_pParameter );
	}
	return l_hrOk;
}

/*
This method uses the p_psvGetParameter attribute to call the DLL SVDigitizerGetParameter function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetParameter( SVHANDLE p_hHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvGetParameter )
	{
		l_hrOk = m_psvGetParameter( p_hHandle, p_eParameter, p_ppParameter );
	}
	return l_hrOk;
}

/*
This method uses the p_psvDestroyParameter attribute to call the DLL SVDigitizerDestroyParameter function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::DestroyParameter( SVHANDLE p_hHandle, SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvDestroyParameter )
	{
		l_hrOk = m_psvDestroyParameter( p_hHandle, p_pParameter );
	}
	return l_hrOk;
}

/*
This method uses the p_psvParameterGetList attribute to call the DLL SVDigitizerParameterGetList function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::ParameterGetList( SVHANDLE p_hHandle, VARIANT *p_pvarName )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvParameterGetList )
	{
		l_hrOk = m_psvParameterGetList( p_hHandle, p_pvarName );
	}
	return l_hrOk;
}

/*
This method uses the p_psvParameterGetName attribute to call the DLL SVDigitizerParameterGetName function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::ParameterGetName( SVHANDLE p_hHandle, int p_iParameterID, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvParameterGetName )
	{
		l_hrOk = m_psvParameterGetName( p_hHandle, p_iParameterID, p_pbstrName );
	}
	return l_hrOk;
}

/*
This method uses the p_psvParameterGetValue attribute to call the DLL SVDigitizerParameterGetValue function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::ParameterGetValue( SVHANDLE p_hHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarName )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvParameterGetValue )
	{
		l_hrOk = m_psvParameterGetValue( p_hHandle, p_iParameterID, p_piParameterTypeID, p_pvarName );
	}
	return l_hrOk;
}

/*
This method uses the p_psvParameterSetList attribute to call the DLL SVDigitizerParameterSetList function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::ParameterSetValue( SVHANDLE p_hHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarName )
{
	HRESULT l_hrOk = S_FALSE;
	if ( nullptr != m_psvParameterSetValue )
	{
		l_hrOk = m_psvParameterSetValue( p_hHandle, p_iParameterID, p_iParameterTypeID, p_pvarName );
	}
	return l_hrOk;
}

// Optional Interface to Scan for existance of cameras
inline HRESULT SVDigitizerLoadLibraryClass::ScanForCameras()
{
	HRESULT l_hrOk = E_NOTIMPL;

	if ( nullptr != m_psvScanForCameras )
	{
		l_hrOk = m_psvScanForCameras();
	}
	return l_hrOk;
}

