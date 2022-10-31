// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionTestStruct
// * .File Name       : $Workfile:   SVRegressionTestStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   04 Sep 2014 12:45:20  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

enum RegressionFileEnum
{
	RegFileList = 0, //For a listing of files
	RegSingleFile = 1,
	RegSingleDirectory = 2,
	RegSubDirectories = 3,
	RegNone = 4
};

struct RegressionTestStruct
{
	std::string Name {};
	std::string firstFilepath {};
	uint32_t objectId{ SvDef::InvalidObjectId };
	RegressionFileEnum iFileMethod {};
	SvDef::StringVector stdVectorFile {};
	SvDef::StringVector::iterator stdIteratorStart {};
	SvDef::StringVector::iterator stdIteratorCurrent {};
	std::vector<std::pair<std::string, std::string>> fileSortRange;
};

struct RegressionRunFileStruct
{
	std::string ObjectName;
	std::string FileName;
};

enum RegressionRunModeEnum
{
	RegModePlay = 0,
	RegModePause = 1,
	RegModeSingleStepForward = 2,
	RegModeSingleStepBack = 3,
	RegModeBackToBeginningPlay = 4,
	RegModeBackToBeginningStop = 5,
	RegModeStopExit = 6,
	RegModeResetSettings = 7
};

enum RegressionPlayPause
{
	Pause = 0,
	Play = 1
};

enum RegressionPlayModeEnum
{
	RunToEnd = 0,
	Continue = 1
};
