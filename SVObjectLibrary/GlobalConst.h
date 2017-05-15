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

namespace SvOl
{
	#pragma region Declarations
	//! Fully qualified names
	const TCHAR* const FqnRoot							= _T( "Root" );
	const TCHAR* const FqnGlobal						= _T( "Global" );
	const TCHAR* const FqnConfiguration					= _T( "Configuration Object" );
	const TCHAR* const FqnInspections					= _T( "Inspections" );
	const TCHAR* const FqnPPQs							= _T( "PPQs" );
	const TCHAR* const FqnPPQVariables					= _T( "PPQ Variables" );
	const TCHAR* const FqnRemoteInputs					= _T( "RemoteInputs" );
	//! Environment FQN names
	const TCHAR* const FqnEnvironment					= _T( "Environment" );
	const TCHAR* const FqnEnvironmentModelNumber		= _T( "Environment.Model Number" );
	const TCHAR* const FqnEnvironmentSerialNumber		= _T( "Environment.Serial Number" );
	const TCHAR* const FqnEnvironmentWinKey				= _T( "Environment.Windows.Win Key" );
	const TCHAR* const FqnEnvironmentImageUpdate		= _T( "Environment.View.Image Display Update" );
	const TCHAR* const FqnEnvironmentResultUpdate		= _T( "Environment.View.Result Display Update" );
	const TCHAR* const FqnEnvironmentMode				= _T( "Environment.Mode" );
	const TCHAR* const FqnEnvironmentModeIsRun			= _T( "Environment.Mode.Is Run" );
	const TCHAR* const FqnEnvironmentModeIsStop			= _T( "Environment.Mode.Is Stop" );
	const TCHAR* const FqnEnvironmentModeIsRegressionTest	= _T( "Environment.Mode.Is Regression Test" );
	const TCHAR* const FqnEnvironmentModeIsTest			= _T( "Environment.Mode.Is Test" );
	const TCHAR* const FqnEnvironmentModeIsEdit			= _T( "Environment.Mode.Is Edit" );
	const TCHAR* const FqnEnvironmentModeValue			= _T( "Environment.Mode.Value" );
	//! Camera FQN names
	const TCHAR* const FqnCameras						= _T( "Cameras" );
	const TCHAR* const FqnCameraSerialNumber			= _T( "Serial Number" );
	const TCHAR* const FqnCameraShutter					= _T( "Shutter" );
	const TCHAR* const FqnCameraGain					= _T( "Gain" );
	//! PPQ FQN names
	const TCHAR* const FqnPpqLength						= _T("Length");
	//! PPQ Variables FQN names
	const TCHAR* const FqnRemoteInput					= _T( ".Remote Input" );
	const TCHAR* const FqnDioInput						= _T( ".DIO.Input" );

	static const LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	#pragma endregion Declarations
} //namespace SvOl

