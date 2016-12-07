//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputInfoListClass
//* .File Name       : $Workfile:   SVInputInfoListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:34  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"

struct SVInObjectInfoStruct;

class SVInputInfoListClass : public SVVector< SVInObjectInfoStruct* >
{
public:
// Standard constructors
	SVInputInfoListClass();

// Standard destructor
	virtual ~SVInputInfoListClass();

	BOOL CheckExistence( int Index = -1 );

	int Add( SVInObjectInfoStruct* );
	void InsertAt( int nIndex, SVInObjectInfoStruct* PInObjectInfo );

protected:
	bool checkDuplicates( SVInObjectInfoStruct* PInObjectInfo );
};

