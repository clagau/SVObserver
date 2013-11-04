//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceParamEnum
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceParamEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:52  $
//******************************************************************************
#ifndef SVFILEACQUISITIONDEVICEPARAMENUM_H
#define SVFILEACQUISITIONDEVICEPARAMENUM_H

enum SVFileAcquisitionDeviceParamEnum
{
	SVFileAcquisitionParameterFilename, 
	SVFileAcquisitionParameterDirectory,
	SVFileAcquisitionParameterLoadingMode,
	SVFileAcquisitionParameterImageWidth,
	SVFileAcquisitionParameterImageHeight,
	SVFileAcquisitionParameterImageFormat,
	SVFileAcquisitionParameterSize
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVFileAcquisitionDevice\SVFileAcquisitionDeviceParamEnum.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:28:52   bWalter
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
 *    Rev 1.0   16 Sep 2008 07:58:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
