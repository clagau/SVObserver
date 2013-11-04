//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerEnums
//* .File Name       : $Workfile:   SVDataManagerEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:46  $
//******************************************************************************

#ifndef SVDATAMANAGERENUMS_H
#define SVDATAMANAGERENUMS_H

enum SVDataManagerLockTypeEnum
{
   SV_INVALID_TYPE = -1,
   SV_ACQUISITION = 0,
   SV_INSPECTION,
   SV_PPQ,
   SV_ARCHIVE,
   SV_DCOM,
   SV_DISPLAY,
   SV_LAST_INSPECTED,
   SV_OTHER       // Code assumes that SV_OTHER will always be last.
};

enum SVLogType
{
	SVLogDMErrors = 1,
	SVLogDMAcquisition = 2,
	SVLogDMInspection = 4,
	SVLogDMPPQ = 8,
	SVLogDMArchive = 16,
	SVLogDMDCom = 32,
	SVLogDMDisplay = 64,
	SVLogDMLastInspection = 128,
	SVLogDMOther = 256,
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerEnums.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2009 12:34:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
