//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file GlobalConst.h
//*****************************************************************************
/// Global constants used in interfaces
//******************************************************************************
#pragma once

#pragma region Includes
#include "Color.h"
#pragma endregion Includes

namespace SvDef
{
#pragma region Declarations
constexpr int			cRingBufferDepthMin = 2;
constexpr int			cRingBufferDepthMax = 100;
constexpr int			cRingBufferNumberOutputImages = 2;
constexpr int			cTableMaxRowMin = 1;
constexpr int			cTableMaxRowMax = 10000;
constexpr int			cMaxStringByteSize = 200;

constexpr long			cMin8BitPixelValue = 0;
constexpr long			cMax8BitPixelValue = 255;
constexpr long			cDefaultToolUpperThreshold = cMax8BitPixelValue;
constexpr long			cDefaultToolLowerThreshold = 40L;
constexpr bool			cDefaultToolThresholdActivate = true;
constexpr bool			cDefaultToolUpperThresholdActivate = true;
constexpr bool			cDefaultToolLowerThresholdActivate = true;

// Default view color settings...
constexpr COLORREF		cDefaultImageViewBackgroundColor = Ivory;

// Default image settings...
constexpr long			cDefaultSourceImageDepth = 15L;
constexpr long			cDefaultCameraImageWidth = 640L;
constexpr long			cDefaultCameraImageHeight = 480L;
constexpr long			cDefaultImageViewWidth = cDefaultCameraImageWidth;
constexpr long			cDefaultImageViewHeight = cDefaultCameraImageHeight;
constexpr long			cDefaultToolsetViewWidth = 200L;
constexpr long			cDefaultToolsetViewHeight = 400L;
constexpr long			cDefaultResultViewWidth = 840L;
constexpr long			cDefaultResultViewHeight = 200L;

// Default tool extents
constexpr double		cDefaultWindowToolLeft = 10.0;
constexpr double		cDefaultWindowToolTop = 10.0;
constexpr double		cDefaultWindowToolWidth = 100.0;
constexpr double		cDefaultWindowToolHeight = 100.0;
constexpr double		cDefaultWindowToolWidthScaleFactor = 1.0;
constexpr double		cDefaultWindowToolHeightScaleFactor = 1.0;

constexpr long			cDefaultPolarTransformCenterX = 200L;
constexpr long			cDefaultPolarTransformCenterY = 50L;
constexpr long			cDefaultPolarTransformStartRadius = 200L;
constexpr long			cDefaultPolarTransformEndRadius = 100L;
constexpr long			cDefaultPolarTransformStartAngle = 210L;
constexpr long			cDefaultPolarTransformEndAngle = 330L;

constexpr int			cMaximumCameras = 4;
constexpr int			cMinPatternModelNewSize = 8;
constexpr int			cMinPatternModelLoadSize = 4;

constexpr char* cGeneralExcludeChars = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7");
constexpr char* cExcludeCharsToolIpName = cGeneralExcludeChars;
constexpr char* cExcludeCharsConfigName = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#");


//! Fully qualified names
constexpr char* FqnRoot = _T("Root");
constexpr char* FqnGlobal = _T("Global");
constexpr char* FqnConfiguration = _T("Configuration Object");
constexpr char* FqnInspections = _T("Inspections");
constexpr char* FqnPPQs = _T("PPQs");
constexpr char* FqnPPQVariables = _T("PPQ Variables");
constexpr char* FqnRemoteInputs = _T("RemoteInputs");
//! Environment FQN names
constexpr char* FqnEnvironment = _T("Environment");
constexpr char* FqnEnvironmentModelNumber = _T("Environment.Model Number");
constexpr char* FqnEnvironmentSerialNumber = _T("Environment.Serial Number");
constexpr char* FqnEnvironmentWinKey = _T("Environment.Windows.Win Key");
constexpr char* FqnEnvironmentAutoSave = _T("Environment.Auto Save");
constexpr char* FqnEnvironmentDiskProtection = _T("Environment.Disk Protection");
constexpr char* FqnEnvironmentStartLastConfig = _T("Environment.Start Last Configuration");
constexpr char* FqnEnvironmentImageUpdate = _T("Environment.View.Image Display Update");
constexpr char* FqnEnvironmentResultUpdate = _T("Environment.View.Result Display Update");

constexpr char* FqnEnvironmentConfigurationName = _T("Environment.Configuration Name");
constexpr char* FqnEnvironmentConfigurationFileName = _T("Environment.Configuration Filename");

constexpr char* FqnEnvironmentCurrentDate = _T("Environment.Current Date");
constexpr char* FqnEnvironmentCurrentTime = _T("Environment.Current Time");

constexpr char* FqnEnvironmentMode = _T("Environment.Mode");
constexpr char* FqnEnvironmentModeIsRun = _T("Environment.Mode.Is Run");
constexpr char* FqnEnvironmentModeIsStop = _T("Environment.Mode.Is Stop");
constexpr char* FqnEnvironmentModeIsRegressionTest = _T("Environment.Mode.Is Regression Test");
constexpr char* FqnEnvironmentModeIsTest = _T("Environment.Mode.Is Test");
constexpr char* FqnEnvironmentModeIsEdit = _T("Environment.Mode.Is Edit");
constexpr char* FqnEnvironmentModeValue = _T("Environment.Mode.Value");
//! Camera FQN names
constexpr char* FqnCameras = _T("Cameras");
constexpr char* FqnCameraSerialNumber = _T("Serial Number");
constexpr char* FqnCameraShutter = _T("Shutter");
constexpr char* FqnCameraGain = _T("Gain");
constexpr char* FqnCameraRegPath = _T("Reg Image Pathname");
constexpr char* FqnCameraRegFile = _T("Reg Image Filename");
//! PPQ FQN names
constexpr char* FqnPpqLength = _T("Length");
constexpr char* FqnPpqTriggerCount = _T("Trigger Count");
//! PPQ Variables FQN names
constexpr char* FqnRemoteInput = _T(".Remote Input");
constexpr char* FqnDioInput = _T(".DIO.Input");

constexpr char* cConfigExtension = _T(".svx");
constexpr char* cPackedConfigExtension = _T(".svz");

constexpr LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);

//Filter constants
constexpr long cStandardKernelSize = 3;
constexpr int cMaxKernelSize = 21;

//SV Error condition mask
constexpr DWORD svErrorCondition = 0xc0000000;

//! These are special SVObserver variant VT types for SVPointValueObjectClass and SVDPointValueObjectClass
constexpr unsigned long VT_POINT = 80;
constexpr unsigned long VT_DPOINT = 81;

#pragma endregion Declarations
} //namespace SvDef
