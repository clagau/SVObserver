//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLevelCreateStruct
//* .File Name       : $Workfile:   SVObjectLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:51:02  $
//******************************************************************************

#ifndef SVOBJECTLEVELCREATESTRUCT_H
#define SVOBJECTLEVELCREATESTRUCT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SVObjectInfoStruct.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectLevelCreateStruct
// -----------------------------------------------------------------------------
// .Description : Base structure for SVM_CREATE... messages
//              : Supports structure identifying by structure name
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVObjectLevelCreateStruct
{
//******************************************************************************
// Constructor(s):
//******************************************************************************

	SVObjectLevelCreateStruct();
	SVObjectLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVObjectLevelCreateStruct();

//******************************************************************************
// Operator(s):
//******************************************************************************

	BOOL CheckCreateStructType( SVObjectLevelCreateStruct* PCreateStructure );

	BOOL CheckCreateStructName( const char* StrCreateStructureName );


//******************************************************************************
// Data Element(s):
//******************************************************************************

	SVObjectInfoStruct OwnerObjectInfo;

protected:

	const char* strCreateStructName;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVOBJECTLEVELCREATESTRUCT_H

