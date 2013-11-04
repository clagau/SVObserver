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
: m_hmHandle(NULL)
, m_psvCreate(NULL)
, m_psvDestroy(NULL)
, m_psvGetCount(NULL)
, m_psvGetHandle(NULL)
, m_psvGetName(NULL)
, m_psvLoadCameraFiles(NULL)
, m_psvGetBufferWidth(NULL)
, m_psvGetBufferHeight(NULL)
, m_psvGetBufferFormat(NULL)
, m_psvCreateBuffers(NULL)
, m_psvRegisterBufferInterface(NULL)
, m_psvStart(NULL)
, m_psvStop(NULL)
, m_psvUnregisterBufferInterface(NULL)
, m_psvDestroyBuffers(NULL)
, m_psvUnloadCameraFile(NULL)
, m_psvSetParameters(NULL)
, m_psvSetParameter(NULL)
, m_psvGetParameter(NULL)
, m_psvDestroyParameter(NULL)
, m_psvParameterGetList(NULL)
, m_psvParameterGetName(NULL)
, m_psvParameterGetValue(NULL)
, m_psvParameterSetValue(NULL)
, m_psvScanForCameras(NULL)
, m_psvInternalTriggerEnable(NULL)
, m_psvInternalTrigger(NULL)
, m_psvInternalTriggerRegister(NULL)
, m_psvInternalTriggerUnregister(NULL)
, m_psvInternalTriggerUnregisterAll(NULL)
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

	if ( m_hmHandle == NULL )
	{
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( m_hmHandle != NULL )
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

			if ( m_psvCreate != NULL &&
			     m_psvDestroy != NULL &&
			     m_psvGetCount != NULL &&
			     m_psvGetHandle != NULL &&
			     m_psvGetName != NULL &&
			     m_psvLoadCameraFiles != NULL &&
			     m_psvGetBufferWidth != NULL &&
			     m_psvGetBufferHeight != NULL &&
			     m_psvGetBufferFormat != NULL &&
			     m_psvCreateBuffers != NULL &&
			     m_psvRegisterBufferInterface != NULL &&
			     m_psvStart != NULL &&
			     m_psvStop != NULL &&
			     m_psvUnregisterBufferInterface != NULL &&
				 m_psvInternalTriggerEnable != NULL &&
				 m_psvInternalTrigger != NULL &&
				 m_psvInternalTriggerRegister != NULL &&
				 m_psvInternalTriggerUnregister != NULL &&
				 m_psvInternalTriggerUnregisterAll != NULL &&
			     m_psvDestroyBuffers != NULL &&
			     m_psvUnloadCameraFile != NULL &&
			     m_psvSetParameters != NULL &&
			     m_psvSetParameter != NULL &&
			     m_psvGetParameter != NULL &&
			     m_psvDestroyParameter != NULL
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

	if ( l_hrOk != S_OK )
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

	if ( m_hmHandle != NULL )
	{
		if ( m_psvDestroy != NULL )
		{
			l_hrOk = m_psvDestroy();
		}

		if ( ::FreeLibrary( m_hmHandle ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			m_hmHandle = NULL;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	m_psvCreate = NULL;
	m_psvDestroy = NULL;
	m_psvGetCount = NULL;
	m_psvGetHandle = NULL;
	m_psvGetName = NULL;
	m_psvLoadCameraFiles = NULL;
	m_psvGetBufferWidth = NULL;
	m_psvGetBufferHeight = NULL;
	m_psvGetBufferFormat = NULL;
	m_psvCreateBuffers = NULL;
	m_psvRegisterBufferInterface = NULL;
	m_psvStart = NULL;
	m_psvStop = NULL;
	m_psvUnregisterBufferInterface = NULL;
	m_psvInternalTriggerEnable = NULL;
	m_psvInternalTrigger = NULL;
	m_psvInternalTriggerRegister = NULL;
	m_psvInternalTriggerUnregister = NULL;
	m_psvInternalTriggerUnregisterAll = NULL;
	m_psvDestroyBuffers = NULL;
	m_psvUnloadCameraFile = NULL;
	m_psvSetParameters = NULL;
	m_psvSetParameter = NULL;
	m_psvGetParameter = NULL;
	m_psvDestroyParameter = NULL;
	m_psvParameterGetList = NULL;
	m_psvParameterGetName = NULL;
	m_psvParameterGetValue = NULL;
	m_psvParameterSetValue = NULL;

	m_psvScanForCameras = NULL;

	return l_hrOk;
}

/*
This method return the loaded state of the DLL.
*/
inline bool SVDigitizerLoadLibraryClass::IsLoaded() const
{
	return (m_hmHandle != NULL);
}

/*
This method return the name of the DLL.
*/
inline HRESULT SVDigitizerLoadLibraryClass::GetLibraryName( BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_hmHandle != NULL )
	{
		WCHAR szPath[MAX_PATH]; 
		
		if ( ::GetModuleFileNameW( m_hmHandle, szPath, MAX_PATH  ) )
		{
			l_hrOk = S_OK;
			WCHAR szFname[_MAX_FNAME];

			_wsplitpath( szPath, NULL, NULL, szFname, NULL );

			_bstr_t fName( szFname );

			if ( *p_pbstrName != NULL )
			{
				::SysFreeString( *p_pbstrName );

				*p_pbstrName = NULL;
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

	if ( m_psvGetCount != NULL )
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

	if ( m_psvGetHandle != NULL )
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

	if ( m_psvGetName != NULL )
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

	if ( m_psvLoadCameraFiles != NULL )
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

	if ( m_psvGetBufferWidth != NULL )
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

	if ( m_psvGetBufferHeight != NULL )
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

	if ( m_psvGetBufferFormat != NULL )
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

	if ( m_psvCreateBuffers != NULL )
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

	if ( m_psvRegisterBufferInterface != NULL )
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

	if ( m_psvStart != NULL )
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

	if ( m_psvStop != NULL )
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

	if ( m_psvUnregisterBufferInterface != NULL )
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

	if ( m_psvInternalTriggerEnable != NULL )
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

	if ( m_psvInternalTrigger != NULL )
	{
		l_hrOk = m_psvInternalTrigger( p_hHandle );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerRegister attribute to call the DLL SVDigitizerInternalTriggerRegister function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerRegister( SVHANDLE p_hHandle, SVCallbackStruct& p_rCallback )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvInternalTriggerRegister != NULL )
	{
		l_hrOk = m_psvInternalTriggerRegister( p_hHandle, p_rCallback.m_pCallback, p_rCallback.m_pOwner, p_rCallback.m_pData );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerUnregister attribute to call the DLL SVDigitizerInternalTriggerUnregister function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerUnregister( SVHANDLE p_hHandle, SVCallbackStruct& p_rCallback )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvInternalTriggerUnregister != NULL )
	{
		l_hrOk = m_psvInternalTriggerUnregister( p_hHandle, p_rCallback.m_pCallback, p_rCallback.m_pOwner, p_rCallback.m_pData );
	}

	return l_hrOk;
}

/*
This method uses the m_psvInternalTriggerUnregisterAll attribute to call the DLL SVDigitizerInternalTriggerUnregisterAll function.  If the pointer to the method is invalid, an error code will be returned.  Otherwise, the status of the DLL function will be returned.
*/
inline HRESULT SVDigitizerLoadLibraryClass::InternalTriggerUnregisterAll( SVHANDLE p_hHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvInternalTriggerUnregisterAll != NULL )
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

	if ( m_psvDestroyBuffers != NULL )
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

	if ( m_psvUnloadCameraFile != NULL )
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
	if ( m_psvSetParameters != NULL )
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
	if ( m_psvSetParameter != NULL )
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
	if ( m_psvGetParameter != NULL )
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
	if ( m_psvDestroyParameter != NULL )
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
	if ( m_psvParameterGetList != NULL )
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
	if ( m_psvParameterGetName != NULL )
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
	if ( m_psvParameterGetValue != NULL )
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
	if ( m_psvParameterSetValue != NULL )
	{
		l_hrOk = m_psvParameterSetValue( p_hHandle, p_iParameterID, p_iParameterTypeID, p_pvarName );
	}
	return l_hrOk;
}

// Optional Interface to Scan for existance of cameras
inline HRESULT SVDigitizerLoadLibraryClass::ScanForCameras()
{
	HRESULT l_hrOk = E_NOTIMPL;

	if ( m_psvScanForCameras != NULL )
	{
		l_hrOk = m_psvScanForCameras();
	}
	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVDigitizerLoadLibraryClass.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   04 Oct 2012 09:05:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated constructor to initialize all class attributes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   15 Feb 2010 09:37:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  682
   SCR Title:  Upgrade SVObserver version for 5.31 release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated old and added missing source code comments to existing code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   29 Jul 2009 15:38:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branced changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   16 Jun 2009 17:31:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  650
   SCR Title:  Integrate Gigabit ethernet cameras
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added ScanForCameras interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   04 Feb 2009 13:34:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  634
   SCR Title:  Implement a File Acquistion Device
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added method to get module name
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   10 Sep 2008 11:09:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  612
   SCR Title:  Implement a timer trigger object
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added methods for Internal Triggering
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3.1.0   13 Jul 2009 13:26:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   29 Oct 2007 15:39:26   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added sleep after FreeLibrary and LoadLibrary
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   08 Nov 2005 15:18:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  516
   SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to include the new parameter functions for the DLL and new image formats for the Intek driver.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   17 Oct 2003 12:58:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated exposed function prototypes to change parameters from a class to individual items.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
