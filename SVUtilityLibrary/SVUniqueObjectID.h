//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUniqueObjectID
//* .File Name       : $Workfile:   SVUniqueObjectID.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:26:16  $
//******************************************************************************

#ifndef SVUNIQUEOBJECTID_H
#define SVUNIQUEOBJECTID_H
#include <list>
/**
@SVObjectName Unique Object Identifier

@SVObjectOverview A Unique ID (Handle) that represents an Object instance

@SVObjectOperations

*/
typedef unsigned long SVUniqueObjectID;

enum
{
	SVInvalidObjectID = 0
};

/**
@SVObjectName Invalid Object

@SVObjectOverview An Invalid Unique ID (Handle) that represents an Invalid Object instance

@SVObjectOperations

*/
struct SVInvalidObject
{
	bool operator == (const SVUniqueObjectID& objectID) const { return objectID == SVInvalidObjectID; }
	bool operator != (const SVUniqueObjectID& objectID) const { return objectID != SVInvalidObjectID; }
	operator SVUniqueObjectID () const  { return SVUniqueObjectID(SVInvalidObjectID); }
};

/**
@SVObjectName GetNext Unique ID

@SVObjectOverview A method to get the next Unique ID (Handle)

@SVObjectOperations

*/
struct SVGetNextUniqueObjectID
{
	static void Next(SVUniqueObjectID& id) { id++; }
	//void operator() (SVUniqueObjectID& id) { id++; }
};

// SEJ - specializations
typedef SVUniqueObjectID SVObjectID;
typedef SVGetNextUniqueObjectID SVGetNextObjectID;
typedef std::list<SVObjectID> SVObjectIDList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVUniqueObjectID.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:26:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:33:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
