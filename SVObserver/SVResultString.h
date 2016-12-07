//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultString
//* .File Name       : $Workfile:   SVResultString.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:58:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma endregion Includes

class SVStringResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVStringResultClass );

public:
	SVStringResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_STRING );
	virtual ~SVStringResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual BOOL OnValidate() override;

protected:
	SVStringValueObjectClass* getInputString();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:

	// Input
	SVInObjectInfoStruct		m_SVInputStringObjectInfo;

	// Output
	SVStringValueObjectClass		m_szValue;
};

