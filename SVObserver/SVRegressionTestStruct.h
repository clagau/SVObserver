// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionTestStruct
// * .File Name       : $Workfile:   SVRegressionTestStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   04 Sep 2014 12:45:20  $
// ******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"

enum RegressionFileEnum
{
	RegFileList = 0, //For a listing of files
	RegSingleFile = 1,
	RegNone = 2
};

struct RegressionTestStruct
{
	SVString Camera;
	SVString FirstFile;
	SVString FileMask;
	RegressionFileEnum iFileMethod; 
	SVStringVector stdVectorFile;
	SVStringVector::iterator stdIteratorStart;
	SVStringVector::iterator stdIteratorCurrent;
	bool bDone;
};

struct RegressionRunFileStruct
{
	SVString CameraName;
	SVString FileName;
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

enum RegressionFileSelectCode
{
	SelectGood				= 0,
	SelectionBadList		= 1,
	SelectionInvalid		= 2,
	SelectionInvalidMask	= 3,
	SelectionNoFiles		= 4,
	SelectionFileNotExist	= 5,
	SelectionEmptyList		= 6
};

