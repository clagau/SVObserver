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

// Default PreTriggerTimeWindow is in milliseconds (750µs)
constexpr double		cDefaultPreTriggerTimeWindow = 0.75;

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
constexpr double		cDefaultScaleFactor = 1.0;

constexpr long			cDefaultPolarTransformCenterX = 200L;
constexpr long			cDefaultPolarTransformCenterY = 50L;
constexpr long			cDefaultPolarTransformStartRadius = 200L;
constexpr long			cDefaultPolarTransformEndRadius = 100L;
constexpr long			cDefaultPolarTransformStartAngle = 210L;
constexpr long			cDefaultPolarTransformEndAngle = 330L;

constexpr int			cMaximumCameras = 4;
constexpr int			cMinPatternModelNewSize = 8;
constexpr int			cMinPatternModelLoadSize = 4;

constexpr const char* cGeneralExcludeChars = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7");
constexpr const char* cExcludeCharsToolIpName = cGeneralExcludeChars;
constexpr const char* cExcludeCharsConfigName = _T("\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#");


//! Fully qualified names
constexpr const char* FqnRoot = _T("Root");
constexpr const char* FqnGlobal = _T("Global");
constexpr const char* FqnConfiguration = _T("Configuration Object");
constexpr const char* FqnInspections = _T("Inspections");
constexpr const char* FqnPPQs = _T("PPQs");
constexpr const char* FqnPPQVariables = _T("PPQ Variables");
constexpr const char* FqnRemoteInputs = _T("RemoteInputs");
//! Environment FQN names
constexpr const char* FqnEnvironment = _T("Environment");
constexpr const char* FqnEnvironmentView = _T("Environment.View");
constexpr const char* FqnEnvironmentModelNumber = _T("Environment.Model Number");
constexpr const char* FqnEnvironmentSerialNumber = _T("Environment.Serial Number");
constexpr const char* FqnEnvironmentWin = _T("Environment.Windows");
constexpr const char* FqnEnvironmentWinKey = _T("Environment.Windows.Win Key");
constexpr const char* FqnEnvironmentAutoSave = _T("Environment.Auto Save");
constexpr const char* FqnEnvironmentDiskProtection = _T("Environment.Disk Protection");
constexpr const char* FqnEnvironmentStartLastConfig = _T("Environment.Start Last Configuration");
constexpr const char* FqnEnvironmentImageUpdate = _T("Environment.View.Image Display Update");
constexpr const char* FqnEnvironmentResultUpdate = _T("Environment.View.Result Display Update");

constexpr const char* FqnEnvironmentConfigurationName = _T("Environment.Configuration Name");
constexpr const char* FqnEnvironmentConfigurationFileName = _T("Environment.Configuration Filename");

constexpr const char* FqnEnvironmentCurrentDate = _T("Environment.Current Date");
constexpr const char* FqnEnvironmentCurrentTime = _T("Environment.Current Time");

constexpr const char* FqnEnvironmentMode = _T("Environment.Mode");
constexpr const char* FqnEnvironmentModeIsRun = _T("Environment.Mode.Is Run");
constexpr const char* FqnEnvironmentModeIsStop = _T("Environment.Mode.Is Stop");
constexpr const char* FqnEnvironmentModeIsRegressionTest = _T("Environment.Mode.Is Regression Test");
constexpr const char* FqnEnvironmentModeIsTest = _T("Environment.Mode.Is Test");
constexpr const char* FqnEnvironmentModeIsEdit = _T("Environment.Mode.Is Edit");
constexpr const char* FqnEnvironmentModeValue = _T("Environment.Mode.Value");
//! Camera FQN names
constexpr const char* FqnCameras = _T("Cameras");
constexpr const char* FqnCameraSerialNumber = _T("Serial Number");
constexpr const char* FqnCameraShutter = _T("Shutter");
constexpr const char* FqnCameraGain = _T("Gain");
constexpr const char* FqnCameraRegPath = _T("Reg Image Pathname");
constexpr const char* FqnCameraRegFile = _T("Reg Image Filename");
//! PPQ FQN names
constexpr const char* FqnPpqLength = _T("Length");
constexpr const char* FqnPpqTriggerCount = _T("Trigger Count");


constexpr const char* FqnPpqNotCompleteCount = _T("Not Complete Count");
constexpr const char* FqnPpqMissingImageCount = _T("Missing Image Count");



//! PPQ Variables FQN names
constexpr const char* FqnRemoteInput = _T(".Remote Input");
constexpr const char* FqnDioInput = _T(".DIO.Input");

constexpr const char* cConfigExtension = _T(".svx");
constexpr const char* cPackedConfigExtension = _T(".svz");

constexpr const char* cWebApp = _T("WebApp.json");
constexpr const char* cWebAppIds = _T("WebAppIds.json");

constexpr LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);

//Filter constants
constexpr long cStandardKernelSize = 3;
constexpr int cMaxKernelSize = 21;

//SV Error condition mask
constexpr DWORD svErrorCondition = 0xc0000000;

//! These are special SVObserver variant VT types for SVPointValueObjectClass and SVDPointValueObjectClass
constexpr unsigned long VT_POINT = 80;
constexpr unsigned long VT_DPOINT = 81;

constexpr const char* cIndexKeyword = _T("IDX");

#pragma endregion Declarations
} //namespace SvDef
