//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorage
//* .File Name       : $Workfile:   SVStorage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:12  $
//******************************************************************************

#ifndef SVSTORAGE_H
#define SVSTORAGE_H

#include <comdef.h>
#include <map>
#include "SVUtilityLibrary/SVString.h"
#include "SVVisionProcessorConstants.h"

struct SVStorage
{
	SVStorage();
	SVStorage( SVVisionProcessor::SVStorageType p_StorageType, const _variant_t& p_rVariant );

	SVVisionProcessor::SVStorageType m_StorageType;
	_variant_t m_Variant;
};

typedef std::map< SVString, SVStorage > SVNameStorageMap;
typedef std::map< SVString, HRESULT > SVNameStatusMap;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStorage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:14:12   bWalter
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
