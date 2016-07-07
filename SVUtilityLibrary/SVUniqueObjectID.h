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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <list>
#pragma endregion Includes

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
};

// Specializations
typedef SVUniqueObjectID SVObjectID;
typedef SVGetNextUniqueObjectID SVGetNextObjectID;
typedef std::list<SVObjectID> SVObjectIDList;

