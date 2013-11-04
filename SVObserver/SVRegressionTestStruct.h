// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionTestStruct
// * .File Name       : $Workfile:   SVRegressionTestStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:42:34  $
// ******************************************************************************

#ifndef _SV_REGRESSION_TEST_STRUCT
#define _SV_REGRESSION_TEST_STRUCT

enum RegressionFileEnum
{
	RegFileList = 0, //For a listing of files
	RegSingleFile = 1,
	RegNone = 2
};

struct RegressionTestStruct
{
	CString csCamera;
	CString csFirstFile;
	CString csFileMask;
	RegressionFileEnum iFileMethod; 
	std::vector<CString> stdVectorFile;
	std::vector<CString>::iterator stdIteratorStart;
	std::vector<CString>::iterator stdIteratorCurrent;
	bool bDone;
/*	POSITION posStarting;
	POSITION posCurrent;
	POSITION posPrevious;
*/};

struct RegressionRunFileStruct
{
	CString csCameraName;
	CString csFileName;
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
	SelectGood = 0,
	SelectionBadList = 1,
	SelectionInvalid = 2,
	SelectionNoFiles = 3
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRegressionTestStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:42:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Aug 2005 10:33:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with regression test.  settings button will reselect files and will reset the files back to beginning of the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/