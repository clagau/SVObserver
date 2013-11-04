//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileCamera
//* .File Name       : $Workfile:   SVFileCameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:42  $
//******************************************************************************
#ifndef SVFILECAMERASTRUCT_H
#define SVFILECAMERASTRUCT_H

#include "SVUtilityLibrary/SVString.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"

struct SVFileCameraStruct
{
	SVFileAcquisitonLoadingModeEnum mode;
	SVString directory;
	SVString fileName;
	SIZE imageSize;
	SVImageFormatEnum imageFormat; // bit depth basically

	SVFileCameraStruct()
	: mode(ContinuousMode), imageFormat(SVImageFormatMono8)
	{
		imageSize.cx = 640;
		imageSize.cy = 480;
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVFileAcquisitionDevice\SVFileCameraStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:28:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:23:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
