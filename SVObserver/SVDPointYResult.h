//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointYResult
//* .File Name       : $Workfile:   SVDPointYResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma endregion Includes

class SVDPointValueObjectClass;

class SVDPointYResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDPointYResultClass );

public:
	SVDPointYResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_Y );
	virtual ~SVDPointYResultClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVDPointValueObjectClass* getInputPoint();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	// Output
	SVDoubleValueObjectClass		y;
};
