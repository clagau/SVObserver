//******************************************************************************
//* COPYRIGHT (c) 2011 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageCopyUtility
//* .File Name       : $Workfile:   SVImageCopyUtility.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:28  $
//******************************************************************************
#ifndef INCL_SVIMAGECOPYUTILITY_H
#define INCL_SVIMAGECOPYUTILITY_H

#include "SVBitmapInfo.h"

class SVImageCopyUtility
{
public:
	virtual HRESULT Copy(const SVBitmapInfo& p_Info, const void* p_pSource) = 0;

protected:
	SVImageCopyUtility() {}
	virtual ~SVImageCopyUtility() {}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVImageCopyUtility.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:22:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 14:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added copy interface to allow for writing data to shared memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/