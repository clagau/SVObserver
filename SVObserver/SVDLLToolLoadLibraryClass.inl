// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolLoadLibraryClass
// * .File Name       : $Workfile:   SVDLLToolLoadLibraryClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   21 Oct 2013 08:21:26  $
// ******************************************************************************

inline SVDLLToolLoadLibraryClass::SVDLLToolLoadLibraryClass()
{
	m_hmHandle = nullptr;

	m_pfnSimpleTest = nullptr;
	m_pfnGetToolName = nullptr;
	m_pfnGetToolVersion = nullptr;
	m_pfnRunTool = nullptr;
	m_pfnStartup = nullptr;
	m_pfnShutDown = nullptr;
	m_pfnInitializeRun = nullptr;
	m_pfnUninitializeRun = nullptr;
	m_pfnGetInputValueDefinitions = nullptr;
	m_pfnDestroyInputValueDefinitionStructures = nullptr;
	m_pfnDestroyResultValueDefinitionStructures = nullptr;
	m_pfnSetInputValues = nullptr;
	m_pfnGetResultValues = nullptr;
	m_pfnGetMessageString = nullptr;
	m_pfnValidateValueParameter = nullptr;
	m_pfnGetResultValueDefinitions = nullptr;
	m_pfnGetNumberOfInputImages = nullptr;
	m_pfnSetMILInputImages = nullptr;
	m_pfnSetHBITMAPInputImages = nullptr;
	m_pfnGetHBITMAPResultImages = nullptr;
	m_pfnSetMILResultImages = nullptr;
	m_pfnGetResultImageDefinitions = nullptr;
	m_pfnDestroyImageDefinitionStructure = nullptr;
}

inline SVDLLToolLoadLibraryClass::~SVDLLToolLoadLibraryClass()
{
	Close();
}

inline HRESULT SVDLLToolLoadLibraryClass::Open(LPCTSTR p_szLibrary, SVDllLoadLibraryCallback fnNotifyProgress )
{
	HRESULT Result( S_OK );

	SvStl::MessageContainer e;

	if ( nullptr == m_hmHandle )
	{
		if(PathFileExists(p_szLibrary))
		{
			// Check bitness
			if( CheckBitness( p_szLibrary ) != ImageFileMachineAMD64 )
			{
				SVString Message = SvUl_SF::Format(_T("External Tool %s is not 64Bit"), p_szLibrary);
				fnNotifyProgress( Message.c_str() );
			}
		}
		fnNotifyProgress( _T("Attempting LoadLibrary"));
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		Sleep(0);

		if ( nullptr != m_hmHandle )
		{
			m_pfnSimpleTest = (SimpleTestPtr)::GetProcAddress( m_hmHandle, "SVSimpleTest" );
			m_pfnGetToolName = (GetToolNamePtr)::GetProcAddress( m_hmHandle, "SVGetToolName" );
			m_pfnGetToolVersion = (GetToolVersionPtr)::GetProcAddress( m_hmHandle, "SVGetToolVersion" );
			m_pfnRunTool = (RunToolPtr)::GetProcAddress( m_hmHandle, "SVRunTool" );
			m_pfnStartup = (StartupPtr)::GetProcAddress( m_hmHandle, "SVStartup" );
			m_pfnShutDown = (ShutDownPtr)::GetProcAddress( m_hmHandle, "SVShutDown" );
			m_pfnInitializeRun = (InitializeRunPtr)::GetProcAddress( m_hmHandle, "SVInitializeRun" );
			m_pfnUninitializeRun = (UninitializeRunPtr)::GetProcAddress( m_hmHandle, "SVUninitializeRun" );
			m_pfnGetInputValueDefinitions = (GetInputValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "SVGetInputValueDefinitions" );
			m_pfnDestroyInputValueDefinitionStructures = (DestroyInputValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "SVDestroyInputValueDefinitionStructures" );
			m_pfnDestroyResultValueDefinitionStructures = (DestroyResultValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "SVDestroyResultValueDefinitionStructures" );
			m_pfnSetInputValues = (SetInputValuesPtr)::GetProcAddress( m_hmHandle, "SVSetInputValues" );
			m_pfnGetResultValues = (GetResultValuesPtr)::GetProcAddress( m_hmHandle, "SVGetResultValues" );
			m_pfnGetMessageString = (GetMessageStringPtr)::GetProcAddress( m_hmHandle, "SVGetErrorMessageString" );
			m_pfnValidateValueParameter = (ValidateValueParameterPtr)::GetProcAddress( m_hmHandle, "SVValidateValueParameter" );
			m_pfnGetResultValueDefinitions = (GetResultValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "SVGetResultValueDefinitions" );
			m_pfnGetNumberOfInputImages = (GetNumberOfInputImagesPtr)::GetProcAddress( m_hmHandle, "SVGetNumberOfInputImages" );
			m_pfnSetMILInputImages = (SetMILInputImagesPtr)::GetProcAddress( m_hmHandle, "SVSetMILInputImages" );
			m_pfnSetMILResultImages = (SetMILResultImagesPtr)::GetProcAddress( m_hmHandle, "SVSetMILResultImages" );
			m_pfnSetHBITMAPInputImages = (SetHBITMAPInputImagesPtr)::GetProcAddress( m_hmHandle, "SVSetHBITMAPInputImages" );
			m_pfnGetHBITMAPResultImages = (GetHBITMAPResultImagesPtr)::GetProcAddress( m_hmHandle, "SVGetHBITMAPResultImages" );
			m_pfnGetResultImageDefinitions = (GetResultImageDefinitionsPtr)::GetProcAddress( m_hmHandle, "SVGetResultImageDefinitions" );
			m_pfnDestroyImageDefinitionStructure = (DestroyImageDefinitionStructurePtr)::GetProcAddress( m_hmHandle, "SVDestroyImageDefinitionStructure" );

			// Backwards compatability
			if ( nullptr == m_pfnSimpleTest ) m_pfnSimpleTest = (SimpleTestPtr)::GetProcAddress( m_hmHandle, "SimpleTest" );
			if ( nullptr == m_pfnGetToolName ) m_pfnGetToolName = (GetToolNamePtr)::GetProcAddress( m_hmHandle, "GetToolName" );
			if ( nullptr == m_pfnGetToolVersion ) m_pfnGetToolVersion = (GetToolVersionPtr)::GetProcAddress( m_hmHandle, "GetToolVersion" );
			if ( nullptr == m_pfnRunTool ) m_pfnRunTool = (RunToolPtr)::GetProcAddress( m_hmHandle, "RunTool" );
			if ( nullptr == m_pfnStartup ) m_pfnStartup = (StartupPtr)::GetProcAddress( m_hmHandle, "Startup" );
			if ( nullptr == m_pfnShutDown ) m_pfnShutDown = (ShutDownPtr)::GetProcAddress( m_hmHandle, "ShutDown" );
			if ( nullptr == m_pfnInitializeRun ) m_pfnInitializeRun = (InitializeRunPtr)::GetProcAddress( m_hmHandle, "InitializeRun" );
			if ( nullptr == m_pfnUninitializeRun ) m_pfnUninitializeRun = (UninitializeRunPtr)::GetProcAddress( m_hmHandle, "UninitializeRun" );
			if ( nullptr == m_pfnGetInputValueDefinitions ) m_pfnGetInputValueDefinitions = (GetInputValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetInputValueDefinitions" );
			if ( nullptr == m_pfnDestroyInputValueDefinitionStructures ) m_pfnDestroyInputValueDefinitionStructures = (DestroyInputValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "DestroyInputValueDefinitionStructures" );
			if ( nullptr == m_pfnDestroyResultValueDefinitionStructures ) m_pfnDestroyResultValueDefinitionStructures = (DestroyResultValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "DestroyResultValueDefinitionStructures" );
			if ( nullptr == m_pfnSetInputValues ) m_pfnSetInputValues = (SetInputValuesPtr)::GetProcAddress( m_hmHandle, "SetInputValues" );
			if ( nullptr == m_pfnGetResultValues ) m_pfnGetResultValues = (GetResultValuesPtr)::GetProcAddress( m_hmHandle, "GetResultValues" );
			if ( nullptr == m_pfnGetMessageString ) m_pfnGetMessageString = (GetMessageStringPtr)::GetProcAddress( m_hmHandle, "GetErrorMessageString" );
			if ( nullptr == m_pfnValidateValueParameter ) m_pfnValidateValueParameter = (ValidateValueParameterPtr)::GetProcAddress( m_hmHandle, "ValidateValueParameter" );
			if ( nullptr == m_pfnGetResultValueDefinitions ) m_pfnGetResultValueDefinitions = (GetResultValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetResultValueDefinitions" );
			if ( nullptr == m_pfnGetNumberOfInputImages ) m_pfnGetNumberOfInputImages = (GetNumberOfInputImagesPtr)::GetProcAddress( m_hmHandle, "GetNumberOfInputImages" );
			if ( nullptr == m_pfnSetMILInputImages ) m_pfnSetMILInputImages = (SetMILInputImagesPtr)::GetProcAddress( m_hmHandle, "SetMILInputImages" );
			if ( nullptr == m_pfnSetMILResultImages ) m_pfnSetMILResultImages = (SetMILResultImagesPtr)::GetProcAddress( m_hmHandle, "SetMILResultImages" );
			if ( nullptr == m_pfnSetHBITMAPInputImages ) m_pfnSetHBITMAPInputImages = (SetHBITMAPInputImagesPtr)::GetProcAddress( m_hmHandle, "SetHBITMAPInputImages" );
			if ( nullptr == m_pfnGetHBITMAPResultImages ) m_pfnGetHBITMAPResultImages = (GetHBITMAPResultImagesPtr)::GetProcAddress( m_hmHandle, "GetHBITMAPResultImages" );
			if ( nullptr == m_pfnGetResultImageDefinitions ) m_pfnGetResultImageDefinitions = (GetResultImageDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetResultImageDefinitions" );
			if ( nullptr == m_pfnDestroyImageDefinitionStructure ) m_pfnDestroyImageDefinitionStructure = (DestroyImageDefinitionStructurePtr)::GetProcAddress( m_hmHandle, "DestroyImageDefinitionStructure" );



			if ( (
				nullptr != m_pfnSimpleTest &&
				nullptr != m_pfnGetToolName &&
				nullptr != m_pfnGetToolVersion &&
				nullptr != m_pfnRunTool &&
				nullptr != m_pfnStartup &&
				nullptr != m_pfnShutDown &&
				nullptr != m_pfnInitializeRun &&
				nullptr != m_pfnUninitializeRun &&
				nullptr != m_pfnGetInputValueDefinitions &&
				nullptr != m_pfnDestroyInputValueDefinitionStructures &&
				nullptr != m_pfnDestroyResultValueDefinitionStructures &&
				nullptr != m_pfnSetInputValues &&
				nullptr != m_pfnGetResultValues &&
				nullptr != m_pfnGetMessageString &&
				nullptr != m_pfnValidateValueParameter &&
				nullptr != m_pfnGetResultValueDefinitions &&
				nullptr != m_pfnGetNumberOfInputImages &&
				nullptr != m_pfnGetResultImageDefinitions &&
				nullptr != m_pfnDestroyImageDefinitionStructure )
				)
			{
				if(     m_pfnSetMILResultImages && m_pfnSetMILInputImages
					&& !m_pfnSetHBITMAPInputImages && !m_pfnGetHBITMAPResultImages )
				{
					m_bUseMil = true;
				}
				else if(    !m_pfnSetMILResultImages && !m_pfnSetMILInputImages
				         &&  m_pfnSetHBITMAPInputImages && m_pfnGetHBITMAPResultImages )
				{
					m_bUseMil = false;
				}
				else	// error in MIL/HBITMAP definitions
				{
					Result = -12372;
					e.setMessage( SvStl::MessageData( -12372, SvOi::Tid_ToolLoadError_ImageDef ) );

					// none are defined
					if( !m_pfnSetMILResultImages && !m_pfnSetMILInputImages 
						&& !m_pfnSetHBITMAPInputImages && !m_pfnGetHBITMAPResultImages )
					{
						e.addMessage( SvStl::MessageData( -12373, SvOi::Tid_ToolLoadError_NoImageDef ), false );
					}
					else if(     m_pfnSetMILResultImages && m_pfnSetMILInputImages
					         &&  m_pfnSetHBITMAPInputImages && m_pfnGetHBITMAPResultImages )
					{
						e.addMessage( SvStl::MessageData( -12375, SvOi::Tid_ToolLoadError_BothSetsImagesDef ), false );
					}
					else if(     (m_pfnSetMILResultImages && (m_pfnGetHBITMAPResultImages || m_pfnSetHBITMAPInputImages))
					         ||  (m_pfnSetHBITMAPInputImages &&  (m_pfnSetMILInputImages || m_pfnSetMILResultImages)))
					{
						e.addMessage( SvStl::MessageData( -12376, SvOi::Tid_ToolLoadError_BothImagesDef ), false );
					}
					else
					{
						e.addMessage( SvStl::MessageData( -12374, SvOi::Tid_ToolLoadError_InFunctionDef ), false );
					}
	
				}// end else	// error in MIL/HBITMAP definitions

				if ( S_OK == Result )
				{
					fnNotifyProgress( _T("Attempting SimpleTest"));

					try
					{
						long lTest = m_pfnSimpleTest( 1, 2 );
						if ( lTest != 2 )
						{
							Result = -12349;
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_SimpleTest ) );
						}
					}
					catch (...)
					{
						Result = -12384;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_SimpleTestException ) );
					}

				}
				if ( S_OK == Result )
				{
					fnNotifyProgress( _T("Attempting Startup"));
					try
					{
						Result = m_pfnStartup();
						if ( S_OK != Result )
						{
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_DllStartup ) );
						}
					}
					catch(...)
					{
						Result = -12385;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_DllStartupException ) );
					}
				}
				// Get Tool Name
				if( S_OK == Result )
				{
					fnNotifyProgress( _T("Attempting GetToolName"));
					BSTR bstName = nullptr;
					try
					{
						Result = m_pfnGetToolName( &bstName );
						if ( S_OK != Result )
						{
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_GetToolName ) );
						}
						else
						{
							fnNotifyProgress( SvUl_SF::createSVString( _bstr_t(bstName) ).c_str() );
						}
					}
					catch(...)
					{
						Result = -12386;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_GetToolNameException ) );
					}

				}
				// Get Tool Version
				if( S_OK == Result )
				{
					fnNotifyProgress( _T("Attempting GetToolVersion"));
					long lTmp=0;
					try
					{
						Result = m_pfnGetToolVersion( &lTmp );
						if ( S_OK != Result )
						{
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_GetToolVersion ) );
						}
						else
						{
							SVString Version = SvUl_SF::Format( _T("Version %d"), lTmp );
							fnNotifyProgress( Version.c_str() );
						}
					}
					catch(...)
					{
						Result = -12387;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError_GetToolVersionException ) );
					}
				}

			}
			else	// get proc address failure
			{
				Result = -12350;
				e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_Empty ) );

				if ( !m_pfnSimpleTest )                             e.addMessage( SvStl::MessageData( -12351, SvOi::Tid_GetProcAddressError_SimpleTest ), false );
				if ( !m_pfnGetToolName )                            e.addMessage( SvStl::MessageData( -12352, SvOi::Tid_GetProcAddressError_GetToolName ), false );
				if ( !m_pfnGetToolVersion )                         e.addMessage( SvStl::MessageData( -12353, SvOi::Tid_GetProcAddressError_GetToolVersion ), false );
				if ( !m_pfnRunTool )                                e.addMessage( SvStl::MessageData( -12354, SvOi::Tid_GetProcAddressError_RunTool ), false );
				if ( !m_pfnStartup )                                e.addMessage( SvStl::MessageData( -12355, SvOi::Tid_GetProcAddressError_Startup ), false );
				if ( !m_pfnShutDown )                               e.addMessage( SvStl::MessageData( -12356, SvOi::Tid_GetProcAddressError_ShutDown ), false );
				if ( !m_pfnInitializeRun )                          e.addMessage( SvStl::MessageData( -12357, SvOi::Tid_GetProcAddressError_InitializeRun ), false );
				if ( !m_pfnUninitializeRun )                        e.addMessage( SvStl::MessageData( -12358, SvOi::Tid_GetProcAddressError_UninitializeRun ), false );
				if ( !m_pfnGetInputValueDefinitions )               e.addMessage( SvStl::MessageData( -12359, SvOi::Tid_GetProcAddressError_GetInputValueDefinitions ), false );
				if ( !m_pfnDestroyInputValueDefinitionStructures )  e.addMessage( SvStl::MessageData( -12360, SvOi::Tid_GetProcAddressError_DestroyInputValueDefinitionStructures ), false );
				if ( !m_pfnDestroyResultValueDefinitionStructures ) e.addMessage( SvStl::MessageData( -12361, SvOi::Tid_GetProcAddressError_DestroyResultValueDefinitionStructures ), false );
				if ( !m_pfnSetInputValues )                         e.addMessage( SvStl::MessageData( -12362, SvOi::Tid_GetProcAddressError_SetInputValues ), false );
				if ( !m_pfnGetResultValues )                        e.addMessage( SvStl::MessageData( -12363, SvOi::Tid_GetProcAddressError_GetResultValues ), false );
				if ( !m_pfnGetMessageString )                       e.addMessage( SvStl::MessageData( -12364, SvOi::Tid_GetProcAddressError_GetMessageString ), false );
				if ( !m_pfnValidateValueParameter )                 e.addMessage( SvStl::MessageData( -12365, SvOi::Tid_GetProcAddressError_ValidateValueParameter ), false );
				if ( !m_pfnGetResultValueDefinitions )              e.addMessage( SvStl::MessageData( -12366, SvOi::Tid_GetProcAddressError_GetResultValueDefinitions ), false );
				if ( !m_pfnGetNumberOfInputImages )                 e.addMessage( SvStl::MessageData( -12367, SvOi::Tid_GetProcAddressError_GetNumberOfInputImages ), false );
				if ( !m_pfnGetResultImageDefinitions )              e.addMessage( SvStl::MessageData( -12368, SvOi::Tid_GetProcAddressError_GetResultImageDefinitions ), false );
				if ( !m_pfnDestroyImageDefinitionStructure )        e.addMessage( SvStl::MessageData( -12369, SvOi::Tid_GetProcAddressError_DestroyImageDefinitionStructure ), false );
			}
		}// end if ( nullptr != m_hmHandle )
		else	// can't load library
		{
			Result = -12378;
			e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), SvOi::Tid_ToolLoadError ) );
		}
	}

	if ( S_OK != Result )
	{
		Close();
		throw e;
	}

	return Result;
}

inline bool SVDLLToolLoadLibraryClass::UseMil()
{
	return m_bUseMil;
}

inline BOOL SVDLLToolLoadLibraryClass::IsHandleNull()
{
	if ( nullptr == m_hmHandle )
	{
		return true;
	}
	return false;
}


inline HRESULT SVDLLToolLoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_hmHandle )
	{
		if ( nullptr != m_pfnShutDown )
		{
			try
			{
				l_hrOk = m_pfnShutDown();
			}
			catch(...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
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

	m_pfnSimpleTest = nullptr;
	m_pfnGetToolName = nullptr;
	m_pfnGetToolVersion = nullptr;
	m_pfnRunTool = nullptr;
	m_pfnStartup = nullptr;
	m_pfnShutDown = nullptr;
	m_pfnInitializeRun = nullptr;
	m_pfnUninitializeRun = nullptr;
	m_pfnGetInputValueDefinitions = nullptr;
	m_pfnDestroyInputValueDefinitionStructures = nullptr;
	m_pfnDestroyResultValueDefinitionStructures = nullptr;
	m_pfnSetInputValues = nullptr;
	m_pfnGetResultValues = nullptr;
	m_pfnGetMessageString = nullptr;
	m_pfnValidateValueParameter = nullptr;
	m_pfnGetResultValueDefinitions = nullptr;
	m_pfnGetNumberOfInputImages = nullptr;
	m_pfnSetMILInputImages = nullptr;
	m_pfnSetHBITMAPInputImages = nullptr;
	m_pfnGetHBITMAPResultImages = nullptr;
	m_pfnSetMILResultImages = nullptr;
	m_pfnGetResultImageDefinitions = nullptr;
	m_pfnDestroyImageDefinitionStructure = nullptr;

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetToolName( BSTR* pbstrToolName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetToolName )
	{
		try
		{
			l_hrOk = m_pfnGetToolName( pbstrToolName );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetToolVersion (long* plVersionNumber)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetToolVersion )
	{
		try
		{
			l_hrOk = m_pfnGetToolVersion( plVersionNumber );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::RunTool (GUID tool, long* plStatus)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnRunTool )
	{
		try
		{
			l_hrOk = m_pfnRunTool( tool, plStatus );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}

		if( S_OK != l_hrOk )
		{
			ASSERT (0);
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), l_hrOk));

			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (l_hrOk), SvOi::Tid_RunTool_Exception, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::Startup ()
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnStartup )
	{
		try
		{
			l_hrOk = m_pfnStartup( );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::ShutDown ()
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnShutDown )
	{
		try
		{
			l_hrOk = m_pfnShutDown( );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::InitializeRun ( GUID tool, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavImputValues)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnInitializeRun )
	{
		try
		{
			l_hrOk = m_pfnInitializeRun( tool, lImageArraySize, paStructs, lValueArraySize, pavImputValues );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}

		if( S_OK != l_hrOk )
		{
			ASSERT (0);
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), l_hrOk));
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (l_hrOk), SvOi::Tid_SVInitializeRun_Exception, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::UninitializeRun ( GUID tool )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnUninitializeRun )
	{
		try
		{
			l_hrOk = m_pfnUninitializeRun( tool );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}
	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetInputValueDefinitions (long* plArraySize, InputValueDefinitionStruct** ppaStructs)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetInputValueDefinitions )
	{
		try
		{
			l_hrOk = m_pfnGetInputValueDefinitions( plArraySize, ppaStructs );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::DestroyInputValueDefinitionStructures (InputValueDefinitionStruct* paStructs)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnDestroyInputValueDefinitionStructures )
	{
		try
		{
			l_hrOk = m_pfnDestroyInputValueDefinitionStructures( paStructs );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::DestroyResultValueDefinitionStructures (ResultValueDefinitionStruct* paStructs)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnDestroyResultValueDefinitionStructures )
	{
		try
		{
			l_hrOk = m_pfnDestroyResultValueDefinitionStructures( paStructs );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::SetInputValues (GUID tool, long lArraySize, VARIANT* paInputValues)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnSetInputValues )
	{
		try
		{
			l_hrOk = m_pfnSetInputValues( tool, lArraySize, paInputValues);
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetResultValues (GUID tool, long lArraySize, VARIANT* paResultValues)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetResultValues )
	{
		try
		{
			l_hrOk = m_pfnGetResultValues( tool, lArraySize, paResultValues );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetMessageString (unsigned long ulErrorNumber,BSTR* pbstrErrorMessage) const
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetMessageString )
	{
		try
		{
			l_hrOk = m_pfnGetMessageString( ulErrorNumber, pbstrErrorMessage );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::ValidateValueParameter (GUID tool, long lParameterNumber, VARIANT vParameterValue)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnValidateValueParameter )
	{
		try
		{
			l_hrOk = m_pfnValidateValueParameter( tool, lParameterNumber, vParameterValue );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetResultValueDefinitions (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetResultValueDefinitions )
	{
		try
		{
			l_hrOk = m_pfnGetResultValueDefinitions( plArraySize, ppaResultValues );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetNumberOfInputImages (long* plNumberOfInputImages)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetNumberOfInputImages )
	{
		try
		{
			l_hrOk = m_pfnGetNumberOfInputImages( plNumberOfInputImages );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::SetMILInputImages (GUID tool, long lArraySize, long* paMILhandles)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnSetMILInputImages )
	{
		try
		{
			l_hrOk = m_pfnSetMILInputImages( tool, lArraySize, paMILhandles );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::SetHBITMAPInputImages (GUID tool, HBITMAP* paHandles)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnSetHBITMAPInputImages )
	{
		try
		{
			l_hrOk = m_pfnSetHBITMAPInputImages( tool, paHandles );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetHBITMAPResultImages (GUID tool, long lArraySize, HBITMAP* paHandles)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetHBITMAPResultImages )
	{
		try
		{
			l_hrOk = m_pfnGetHBITMAPResultImages( tool, lArraySize, paHandles );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}
	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::SetMILResultImages (GUID tool, long lArraySize, long* paMILhandles)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnSetMILResultImages )
	{
		try
		{
			l_hrOk = m_pfnSetMILResultImages( tool, lArraySize, paMILhandles );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetResultImageDefinitions (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnGetResultImageDefinitions )
	{
		try
		{
			l_hrOk = m_pfnGetResultImageDefinitions( tool, plArraySize, ppaImageDefinitions );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::DestroyImageDefinitionStructure ( SVImageDefinitionStruct* paStructs)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_pfnDestroyImageDefinitionStructure )
	{
		try
		{
			l_hrOk = m_pfnDestroyImageDefinitionStructure( paStructs );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}
	}

	return l_hrOk;
}

inline bool SVDLLToolLoadLibraryClass::IsOpen()
{
	return nullptr != m_hmHandle;
}

