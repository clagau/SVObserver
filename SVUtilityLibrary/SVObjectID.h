//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectID
//* .File Name       : $Workfile:   SVObjectID.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:23:12  $
//******************************************************************************

#ifndef SVOBJECTID_H
#define SVOBJECTID_H

/*
// Begin - To use GUID as SVObjectID use the following section
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"

typedef SVGUID SVObjectID;
typedef SVGUID SVInvalidObject;
typedef SVGetNextGUID SVGetNextObjectID;
// End - To use GUID as SVObjectID use the following section
*/

// Begin - To use SVUniqueObjectID as SVObjectID use the following section
#include "SVUtilityLibrary/SVUniqueObjectID.h"
// End - To use SVUniqueObjectID as SVObjectID use the following section

// common includes
#include "SVUtilityLibrary/SVUniqueIdGenerator.h"

/**
@SVObjectName Identifier Generator

@SVObjectOverview This type definition defines the specialization of the templated Unique Identifier Generator.

@SVObjectOperations Operations are defined as part of the Unique Identifier Generator template.

*/
typedef SVUniqueIdGenerator< SVObjectID, SVInvalidObject, SVGetNextObjectID > SVIDGenerator;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVObjectID.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:23:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
