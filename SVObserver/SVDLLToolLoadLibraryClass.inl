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

#pragma region Includes
#include "SVMessage\SVMessage.h"
#include "SVOMFCLibrary\SVOMFCLibraryGlobals.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

inline SVDLLToolLoadLibraryClass::SVDLLToolLoadLibraryClass()
{
	m_hmHandle = NULL;

	m_pfnSimpleTest = NULL;
	m_pfnGetToolName = NULL;
	m_pfnGetToolVersion = NULL;
	m_pfnRunTool = NULL;
	m_pfnStartup = NULL;
	m_pfnShutDown = NULL;
	m_pfnInitializeRun = NULL;
	m_pfnUninitializeRun = NULL;
	m_pfnGetInputValueDefinitions = NULL;
	m_pfnDestroyInputValueDefinitionStructures = NULL;
	m_pfnDestroyResultValueDefinitionStructures = NULL;
	m_pfnSetInputValues = NULL;
	m_pfnGetResultValues = NULL;
	m_pfnGetMessageString = NULL;
	m_pfnValidateValueParameter = NULL;
	m_pfnGetResultValueDefinitions = NULL;
	m_pfnGetNumberOfInputImages = NULL;
	m_pfnSetMILInputImages = NULL;
	m_pfnSetHBITMAPInputImages = NULL;
	m_pfnGetHBITMAPResultImages = NULL;
	m_pfnSetMILResultImages = NULL;
	m_pfnGetResultImageDefinitions = NULL;
	m_pfnDestroyImageDefinitionStructure = NULL;
}

inline SVDLLToolLoadLibraryClass::~SVDLLToolLoadLibraryClass()
{
	Close();
}

inline HRESULT SVDLLToolLoadLibraryClass::Open(LPCTSTR p_szLibrary, SVDllLoadLibraryCallback fnNotifyProgress )
{
	HRESULT Result( S_OK );

	SvStl::MessageContainer e;

	if ( m_hmHandle == NULL )
	{
		if(PathFileExists(p_szLibrary))
		{
			// Check bitness
#ifdef _WIN64
			if( CheckBitness( p_szLibrary ) != ImageFileMachineAMD64 )
			{
				CString l_strTmp;
				l_strTmp.Format(_T("External Tool %s is not 64Bit"),p_szLibrary);
				fnNotifyProgress( l_strTmp);
			}
#else
			if( CheckBitness( p_szLibrary ) != ImageFileMachineI386 )
			{
				CString l_strTmp;
				l_strTmp.Format(_T("External Tool %s is not 32Bit"),p_szLibrary);
				fnNotifyProgress( l_strTmp);
			}
#endif
		}
		fnNotifyProgress( _T("Attempting LoadLibrary"));
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		Sleep(0);

		if ( m_hmHandle != NULL )
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
			if ( m_pfnSimpleTest == NULL ) m_pfnSimpleTest = (SimpleTestPtr)::GetProcAddress( m_hmHandle, "SimpleTest" );
			if ( m_pfnGetToolName == NULL ) m_pfnGetToolName = (GetToolNamePtr)::GetProcAddress( m_hmHandle, "GetToolName" );
			if ( m_pfnGetToolVersion == NULL ) m_pfnGetToolVersion = (GetToolVersionPtr)::GetProcAddress( m_hmHandle, "GetToolVersion" );
			if ( m_pfnRunTool == NULL ) m_pfnRunTool = (RunToolPtr)::GetProcAddress( m_hmHandle, "RunTool" );
			if ( m_pfnStartup == NULL ) m_pfnStartup = (StartupPtr)::GetProcAddress( m_hmHandle, "Startup" );
			if ( m_pfnShutDown == NULL ) m_pfnShutDown = (ShutDownPtr)::GetProcAddress( m_hmHandle, "ShutDown" );
			if ( m_pfnInitializeRun == NULL ) m_pfnInitializeRun = (InitializeRunPtr)::GetProcAddress( m_hmHandle, "InitializeRun" );
			if ( m_pfnUninitializeRun == NULL ) m_pfnUninitializeRun = (UninitializeRunPtr)::GetProcAddress( m_hmHandle, "UninitializeRun" );
			if ( m_pfnGetInputValueDefinitions == NULL ) m_pfnGetInputValueDefinitions = (GetInputValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetInputValueDefinitions" );
			if ( m_pfnDestroyInputValueDefinitionStructures == NULL ) m_pfnDestroyInputValueDefinitionStructures = (DestroyInputValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "DestroyInputValueDefinitionStructures" );
			if ( m_pfnDestroyResultValueDefinitionStructures == NULL ) m_pfnDestroyResultValueDefinitionStructures = (DestroyResultValueDefinitionStructuresPtr)::GetProcAddress( m_hmHandle, "DestroyResultValueDefinitionStructures" );
			if ( m_pfnSetInputValues == NULL ) m_pfnSetInputValues = (SetInputValuesPtr)::GetProcAddress( m_hmHandle, "SetInputValues" );
			if ( m_pfnGetResultValues == NULL ) m_pfnGetResultValues = (GetResultValuesPtr)::GetProcAddress( m_hmHandle, "GetResultValues" );
			if ( m_pfnGetMessageString == NULL ) m_pfnGetMessageString = (GetMessageStringPtr)::GetProcAddress( m_hmHandle, "GetErrorMessageString" );
			if ( m_pfnValidateValueParameter == NULL ) m_pfnValidateValueParameter = (ValidateValueParameterPtr)::GetProcAddress( m_hmHandle, "ValidateValueParameter" );
			if ( m_pfnGetResultValueDefinitions == NULL ) m_pfnGetResultValueDefinitions = (GetResultValueDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetResultValueDefinitions" );
			if ( m_pfnGetNumberOfInputImages == NULL ) m_pfnGetNumberOfInputImages = (GetNumberOfInputImagesPtr)::GetProcAddress( m_hmHandle, "GetNumberOfInputImages" );
			if ( m_pfnSetMILInputImages == NULL ) m_pfnSetMILInputImages = (SetMILInputImagesPtr)::GetProcAddress( m_hmHandle, "SetMILInputImages" );
			if ( m_pfnSetMILResultImages == NULL ) m_pfnSetMILResultImages = (SetMILResultImagesPtr)::GetProcAddress( m_hmHandle, "SetMILResultImages" );
			if ( m_pfnSetHBITMAPInputImages == NULL ) m_pfnSetHBITMAPInputImages = (SetHBITMAPInputImagesPtr)::GetProcAddress( m_hmHandle, "SetHBITMAPInputImages" );
			if ( m_pfnGetHBITMAPResultImages == NULL ) m_pfnGetHBITMAPResultImages = (GetHBITMAPResultImagesPtr)::GetProcAddress( m_hmHandle, "GetHBITMAPResultImages" );
			if ( m_pfnGetResultImageDefinitions == NULL ) m_pfnGetResultImageDefinitions = (GetResultImageDefinitionsPtr)::GetProcAddress( m_hmHandle, "GetResultImageDefinitions" );
			if ( m_pfnDestroyImageDefinitionStructure == NULL ) m_pfnDestroyImageDefinitionStructure = (DestroyImageDefinitionStructurePtr)::GetProcAddress( m_hmHandle, "DestroyImageDefinitionStructure" );



			if ( (
				m_pfnSimpleTest != NULL &&
				m_pfnGetToolName != NULL &&
				m_pfnGetToolVersion != NULL &&
				m_pfnRunTool != NULL &&
				m_pfnStartup != NULL &&
				m_pfnShutDown != NULL &&
				m_pfnInitializeRun != NULL &&
				m_pfnUninitializeRun != NULL &&
				m_pfnGetInputValueDefinitions != NULL &&
				m_pfnDestroyInputValueDefinitionStructures != NULL &&
				m_pfnDestroyResultValueDefinitionStructures != NULL &&
				m_pfnSetInputValues != NULL &&
				m_pfnGetResultValues != NULL &&
				m_pfnGetMessageString != NULL &&
				m_pfnValidateValueParameter != NULL &&
				m_pfnGetResultValueDefinitions != NULL &&
				m_pfnGetNumberOfInputImages != NULL &&
				m_pfnGetResultImageDefinitions != NULL &&
				m_pfnDestroyImageDefinitionStructure != NULL )
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
					e.setMessage( SvStl::MessageData( -12372, _T("Error in MIL/HBITMAP function definitions.") ) );

					// none are defined
					if( !m_pfnSetMILResultImages && !m_pfnSetMILInputImages 
						&& !m_pfnSetHBITMAPInputImages && !m_pfnGetHBITMAPResultImages )
					{
						e.addMessage( SvStl::MessageData( -12373, _T("Neither MIL nor HBITMAP image functions are defined.") ), false );
					}
					else if(     m_pfnSetMILResultImages && m_pfnSetMILInputImages
					         &&  m_pfnSetHBITMAPInputImages && m_pfnGetHBITMAPResultImages )
					{
						e.addMessage( SvStl::MessageData( -12375, _T("Both sets of MIL and HBITMAP image functions are defined.") ), false );
					}
					else if(     (m_pfnSetMILResultImages && (m_pfnGetHBITMAPResultImages || m_pfnSetHBITMAPInputImages))
					         ||  (m_pfnSetHBITMAPInputImages &&  (m_pfnSetMILInputImages || m_pfnSetMILResultImages)))
					{
						e.addMessage( SvStl::MessageData( -12376, _T("Both MIL and HBITMAP image functions are defined.") ), false );
					}
					else
					{
						e.addMessage( SvStl::MessageData( -12374, _T("Error in MIL/HBITMAP image functions definitions.") ), false );
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
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), _T("SimpleTest failed.") ) );
						}
					}
					catch (...)
					{
						Result = -12384;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("SimpleTest failed - exception.") ) );
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
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("DLL Startup function failed.") ) );
						}
					}
					catch(...)
					{
						Result = -12385;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result), _T("DLL Startup function failed.- exception") ) );
					}
				}
				// Get Tool Name
				if( S_OK == Result )
				{
					fnNotifyProgress( _T("Attempting GetToolName"));
					BSTR bstName = NULL;
					try
					{
						Result = m_pfnGetToolName( &bstName );
						if ( S_OK != Result )
						{
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("DLL GetToolName function failed.") ) );
						}
						else
						{
							fnNotifyProgress( CString( bstName ));
						}
					}
					catch(...)
					{
						Result = -12386;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("DLL GetToolName failed.- exception") ) );
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
							e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("DLL GetToolVersion function failed.") ) );
						}
						else
						{
							fnNotifyProgress( _T("Version ") + AsString( lTmp ) );
						}
					}
					catch(...)
					{
						Result = -12387;
						e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("DLL GetToolVersion failed.- exception") ) );
					}
				}

			}
			else	// get proc address failure
			{
				Result = -12350;
				e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("") ) );

				if ( !m_pfnSimpleTest )                             e.addMessage( SvStl::MessageData( -12351, _T("GetProcAddress failed SimpleTest") ), false );
				if ( !m_pfnGetToolName )                            e.addMessage( SvStl::MessageData( -12352, _T("GetProcAddress failed GetToolName") ), false );
				if ( !m_pfnGetToolVersion )                         e.addMessage( SvStl::MessageData( -12353, _T("GetProcAddress failed GetToolVersion") ), false );
				if ( !m_pfnRunTool )                                e.addMessage( SvStl::MessageData( -12354, _T("GetProcAddress failed RunTool") ), false );
				if ( !m_pfnStartup )                                e.addMessage( SvStl::MessageData( -12355, _T("GetProcAddress failed Startup") ), false );
				if ( !m_pfnShutDown )                               e.addMessage( SvStl::MessageData( -12356, _T("GetProcAddress failed ShutDown") ), false );
				if ( !m_pfnInitializeRun )                          e.addMessage( SvStl::MessageData( -12357, _T("GetProcAddress failed InitializeRun") ), false );
				if ( !m_pfnUninitializeRun )                        e.addMessage( SvStl::MessageData( -12358, _T("GetProcAddress failed UninitializeRun") ), false );
				if ( !m_pfnGetInputValueDefinitions )               e.addMessage( SvStl::MessageData( -12359, _T("GetProcAddress failed GetInputValueDefinitions") ), false );
				if ( !m_pfnDestroyInputValueDefinitionStructures )  e.addMessage( SvStl::MessageData( -12360, _T("GetProcAddress failed DestroyInputValueDefinitionStructures") ), false );
				if ( !m_pfnDestroyResultValueDefinitionStructures ) e.addMessage( SvStl::MessageData( -12361, _T("GetProcAddress failed DestroyResultValueDefinitionStructures") ), false );
				if ( !m_pfnSetInputValues )                         e.addMessage( SvStl::MessageData( -12362, _T("GetProcAddress failed SetInputValues") ), false );
				if ( !m_pfnGetResultValues )                        e.addMessage( SvStl::MessageData( -12363, _T("GetProcAddress failed GetResultValues") ), false );
				if ( !m_pfnGetMessageString )                       e.addMessage( SvStl::MessageData( -12364, _T("GetProcAddress failed GetMessageString") ), false );
				if ( !m_pfnValidateValueParameter )                 e.addMessage( SvStl::MessageData( -12365, _T("GetProcAddress failed ValidateValueParameter") ), false );
				if ( !m_pfnGetResultValueDefinitions )              e.addMessage( SvStl::MessageData( -12366, _T("GetProcAddress failed GetResultValueDefinitions") ), false );
				if ( !m_pfnGetNumberOfInputImages )                 e.addMessage( SvStl::MessageData( -12367, _T("GetProcAddress failed GetNumberOfInputImages") ), false );
				if ( !m_pfnGetResultImageDefinitions )              e.addMessage( SvStl::MessageData( -12368, _T("GetProcAddress failed GetResultImageDefinitions") ), false );
				if ( !m_pfnDestroyImageDefinitionStructure )        e.addMessage( SvStl::MessageData( -12369, _T("GetProcAddress failed DestroyImageDefinitionStructure") ), false );
			}
		}// end if ( m_hmHandle != NULL )
		else	// can't load library
		{
			Result = -12378;
			e.setMessage( SvStl::MessageData( static_cast<DWORD> (Result),  _T("Could not load DLL (LoadLibrary failed).") ) );
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
	if ( m_hmHandle == NULL )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


inline HRESULT SVDLLToolLoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_hmHandle != NULL )
	{
		if ( m_pfnShutDown != NULL )
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
			m_hmHandle = NULL;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	m_pfnSimpleTest = NULL;
	m_pfnGetToolName = NULL;
	m_pfnGetToolVersion = NULL;
	m_pfnRunTool = NULL;
	m_pfnStartup = NULL;
	m_pfnShutDown = NULL;
	m_pfnInitializeRun = NULL;
	m_pfnUninitializeRun = NULL;
	m_pfnGetInputValueDefinitions = NULL;
	m_pfnDestroyInputValueDefinitionStructures = NULL;
	m_pfnDestroyResultValueDefinitionStructures = NULL;
	m_pfnSetInputValues = NULL;
	m_pfnGetResultValues = NULL;
	m_pfnGetMessageString = NULL;
	m_pfnValidateValueParameter = NULL;
	m_pfnGetResultValueDefinitions = NULL;
	m_pfnGetNumberOfInputImages = NULL;
	m_pfnSetMILInputImages = NULL;
	m_pfnSetHBITMAPInputImages = NULL;
	m_pfnGetHBITMAPResultImages = NULL;
	m_pfnSetMILResultImages = NULL;
	m_pfnGetResultImageDefinitions = NULL;
	m_pfnDestroyImageDefinitionStructure = NULL;

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::GetToolName( BSTR* pbstrToolName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_pfnGetToolName != NULL )
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

	if ( m_pfnGetToolVersion != NULL )
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

	if ( m_pfnRunTool != NULL )
	{
		try
		{
			l_hrOk = m_pfnRunTool( tool, plStatus );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}

		if( l_hrOk != S_OK )
		{
			CString l_strTmp;
			l_strTmp.Format( _T("Exception in SVRunTool of some External Dll %d \n"), l_hrOk);
			TRACE (l_strTmp);
			ASSERT (0);

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (l_hrOk), l_strTmp, StdMessageParams );
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::Startup ()
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_pfnStartup != NULL )
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

	if ( m_pfnShutDown != NULL )
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

	if ( m_pfnInitializeRun != NULL )
	{
		try
		{
			l_hrOk = m_pfnInitializeRun( tool, lImageArraySize, paStructs, lValueArraySize, pavImputValues );
		}
		catch(...)
		{
			l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
		}

		if( l_hrOk != S_OK )
		{
			CString l_strTmp;
			l_strTmp.Format( _T("Exception in SVInitializeRun in some External Dll %d \n"), l_hrOk);
			TRACE (l_strTmp);
			ASSERT (0);

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (l_hrOk), l_strTmp, StdMessageParams );
		}
	}

	return l_hrOk;
}

inline HRESULT SVDLLToolLoadLibraryClass::UninitializeRun ( GUID tool )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_pfnUninitializeRun != NULL )
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

	if ( m_pfnGetInputValueDefinitions != NULL )
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

	if ( m_pfnDestroyInputValueDefinitionStructures != NULL )
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

	if ( m_pfnDestroyResultValueDefinitionStructures != NULL )
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

	if ( m_pfnSetInputValues != NULL )
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

	if ( m_pfnGetResultValues != NULL )
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

inline HRESULT SVDLLToolLoadLibraryClass::GetMessageString (unsigned long ulErrorNumber,BSTR* pbstrErrorMessage)
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_pfnGetMessageString != NULL )
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

	if ( m_pfnValidateValueParameter != NULL )
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

	if ( m_pfnGetResultValueDefinitions != NULL )
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

	if ( m_pfnGetNumberOfInputImages != NULL )
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

	if ( m_pfnSetMILInputImages != NULL )
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

	if ( m_pfnSetHBITMAPInputImages != NULL )
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

	if ( m_pfnGetHBITMAPResultImages != NULL )
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

	if ( m_pfnSetMILResultImages != NULL )
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

	if ( m_pfnGetResultImageDefinitions != NULL )
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

	if ( m_pfnDestroyImageDefinitionStructure != NULL )
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
	return m_hmHandle != NULL;
}

