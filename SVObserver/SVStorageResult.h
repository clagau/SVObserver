//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorageResult
//* .File Name       : $Workfile:   SVStorageResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:36  $
//******************************************************************************

#ifndef SVSTORAGERESULT_H
#define SVSTORAGERESULT_H

#include <set>
#include "SVStorage.h"

struct SVStorageResult
{
	SVStorageResult();
	SVStorageResult( const SVStorage& p_rStorage, HRESULT p_Status, unsigned long p_TriggerCount );

	SVStorage m_Storage;
	HRESULT m_Status;
	unsigned long m_TriggerCount;
};

typedef std::set< SVString > SVNameSet;
typedef std::map< SVString, SVStorageResult > SVNameStorageResultMap;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStorageResult.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:14:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
