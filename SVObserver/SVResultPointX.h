//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointX
//* .File Name       : $Workfile:   SVResultPointX.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma endregion Includes

class SVPointXResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVPointXResultClass );

public:
	SVPointXResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_POINT_X );
	virtual ~SVPointXResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVPointValueObjectClass* getInputPoint();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Output
	SVLongValueObjectClass		x;
};
