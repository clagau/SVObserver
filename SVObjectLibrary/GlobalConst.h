//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Global constants
//* .File Name       : $Workfile:   GlobalConst.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   25 Aug 2014 02:33:18  $
//* ----------------------------------------------------------------------------
//* This is the global constant file for the SVObjectLibrary project.
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVObjectLibrary
	{
		#pragma region Declarations
		const TCHAR* const FqnConfiguration				= _T( "Configuration Object" );
		const TCHAR* const FqnInspections				= _T( "Inspections" );
		const TCHAR* const FqnPPQs						= _T( "PPQs" );
		const TCHAR* const FqnPPQVariables				= _T( "PPQ Variables" );
		const TCHAR* const FqnRemoteInputs				= _T( "RemoteInputs" );
		const TCHAR* const FqnCameras					= _T( "Cameras" );
		const TCHAR* const FqnEnvironmentMode			= _T( "Environment.Mode" );
		const TCHAR* const FqnEnvironment				= _T( "Environment" );
		const TCHAR* const FqnRoot						= _T( "Root" );
		const TCHAR* const FqnGlobal					= _T( "Global" );
		static const LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
		#pragma endregion Declarations
	}
}

namespace SvOl = Seidenader::SVObjectLibrary;

