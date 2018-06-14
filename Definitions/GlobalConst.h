//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file GlobalConst.h
//*****************************************************************************
/// Global constants used in interfaces
//******************************************************************************
#pragma once

#pragma region Includes
#include "Windef.h"
#include "Color.h"
#pragma endregion Includes

namespace SvDef
{
#pragma region Declarations
	const int			cRingBufferDepthMin = 2;
	const int			cRingBufferDepthMax = 30;
	const int			cRingBufferNumberOutputImages = 2;
	const int			cTableMaxRowMin = 1;
	const int			cTableMaxRowMax = 10000;
	const int			cMaxStringSize = 512;

	const long			cDefaultToolUpperThreshold = 255L;
	const long			cDefaultToolLowerThreshold = 40L;
	const bool			cDefaultToolThresholdActivate = true;
	const bool			cDefaultToolUpperThresholdActivate = true;
	const bool			cDefaultToolLowerThresholdActivate = true;			 

	// Default view color settings...
	const COLORREF		cDefaultImageViewBackgroundColor = Ivory;

	// Default image settings...
	const long			cDefaultSourceImageDepth = 15L;
	const long			cDefaultResultImageDepth = 5L;
	const long			cDefaultCameraImageWidth = 640L;
	const long			cDefaultCameraImageHeight = 480L;
	const long			cDefaultImageViewWidth = cDefaultCameraImageWidth;
	const long			cDefaultImageViewHeight = cDefaultCameraImageHeight;
	const long			cDefaultToolsetViewWidth = 200L;
	const long			cDefaultToolsetViewHeight = 400L;
	const long			cDefaultResultViewWidth = 840L;
	const long			cDefaultResultViewHeight = 200L;

	// Default tool extents
	const double		cDefaultWindowToolLeft = 10.0;
	const double		cDefaultWindowToolTop = 10.0;
	const double		cDefaultWindowToolWidth = 100.0;
	const double		cDefaultWindowToolHeight  = 100.0;
	const double		cDefaultWindowToolWidthScaleFactor = 1.0;
	const double		cDefaultWindowToolHeightScaleFactor = 1.0;

	const long			cDefaultPolarTransformCenterX = 200L;
	const long			cDefaultPolarTransformCenterY =  50L;
	const long			cDefaultPolarTransformStartRadius = 200L;
	const long			cDefaultPolarTransformEndRadius = 100L;
	const long			cDefaultPolarTransformStartAngle = 210L;
	const long			cDefaultPolarTransformEndAngle = 330L;

	const int			cMaximumCameras = 4;
	const int			cMinPatternModelNewSize = 8;
	const int			cMinPatternModelLoadSize = 4;

	const TCHAR* const cGeneralExcludeChars = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7");
	const TCHAR* const cExcludeCharsToolIpName = cGeneralExcludeChars;
	const TCHAR* const cExcludeCharsConfigName = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#");


	//! Fully qualified names
	const TCHAR* const FqnRoot = _T("Root");
	const TCHAR* const FqnGlobal = _T("Global");
	const TCHAR* const FqnConfiguration = _T("Configuration Object");
	const TCHAR* const FqnInspections = _T("Inspections");
	const TCHAR* const FqnPPQs = _T("PPQs");
	const TCHAR* const FqnPPQVariables = _T("PPQ Variables");
	const TCHAR* const FqnRemoteInputs = _T("RemoteInputs");
	//! Environment FQN names
	const TCHAR* const FqnEnvironment = _T("Environment");
	const TCHAR* const FqnEnvironmentModelNumber = _T("Environment.Model Number");
	const TCHAR* const FqnEnvironmentSerialNumber = _T("Environment.Serial Number");
	const TCHAR* const FqnEnvironmentWinKey = _T("Environment.Windows.Win Key");
	const TCHAR* const FqnEnvironmentAutoSave = _T("Environment.Auto Save");
	const TCHAR* const FqnEnvironmentDiskProtection = _T("Environment.Disk Protection");
	const TCHAR* const FqnEnvironmentStartLastConfig = _T("Environment.Start Last Configuration");
	const TCHAR* const FqnEnvironmentImageUpdate = _T("Environment.View.Image Display Update");
	const TCHAR* const FqnEnvironmentResultUpdate = _T("Environment.View.Result Display Update");
	const TCHAR* const FqnEnvironmentMode = _T("Environment.Mode");
	const TCHAR* const FqnEnvironmentModeIsRun = _T("Environment.Mode.Is Run");
	const TCHAR* const FqnEnvironmentModeIsStop = _T("Environment.Mode.Is Stop");
	const TCHAR* const FqnEnvironmentModeIsRegressionTest = _T("Environment.Mode.Is Regression Test");
	const TCHAR* const FqnEnvironmentModeIsTest = _T("Environment.Mode.Is Test");
	const TCHAR* const FqnEnvironmentModeIsEdit = _T("Environment.Mode.Is Edit");
	const TCHAR* const FqnEnvironmentModeValue = _T("Environment.Mode.Value");
	//! Camera FQN names
	const TCHAR* const FqnCameras = _T("Cameras");
	const TCHAR* const FqnCameraSerialNumber = _T("Serial Number");
	const TCHAR* const FqnCameraShutter = _T("Shutter");
	const TCHAR* const FqnCameraGain = _T("Gain");
	//! PPQ FQN names
	const TCHAR* const FqnPpqLength = _T("Length");
	//! PPQ Variables FQN names
	const TCHAR* const FqnRemoteInput = _T(".Remote Input");
	const TCHAR* const FqnDioInput = _T(".DIO.Input");

	const TCHAR* const cConfigExtension = _T(".svx");
	const TCHAR* const cPackedConfigExtension = _T(".svz");

	static const LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
#pragma endregion Declarations
} //namespace SvDef
