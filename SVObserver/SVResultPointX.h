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

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

protected:
	SVPointValueObjectClass* getInputPoint();
	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Output
	SVLongValueObjectClass		x;
};
