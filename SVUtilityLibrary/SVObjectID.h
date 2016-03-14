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

// Begin - To use SVUniqueObjectID as SVObjectID use the following section
#include "SVUniqueObjectID.h"
// End - To use SVUniqueObjectID as SVObjectID use the following section

// common includes
#include "SVUniqueIdGenerator.h"

/**
@SVObjectName Identifier Generator

@SVObjectOverview This type definition defines the specialization of the templated Unique Identifier Generator.

@SVObjectOperations Operations are defined as part of the Unique Identifier Generator template.

*/
typedef SVUniqueIdGenerator< SVObjectID, SVInvalidObject, SVGetNextObjectID > SVIDGenerator;

#endif

