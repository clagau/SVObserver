//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantResultClass
//* .File Name       : $Workfile:   SVVariantResultClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:19:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma region Includes

class SVVariantResultClass : public SVResultClass  
{
public:
	SVVariantResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_VARIANT );
	virtual ~SVVariantResultClass();
	SV_DECLARE_CLASS( SVVariantResultClass );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVValueObjectClass* GetInputValue();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	// Input
	SVInObjectInfoStruct		m_InputValueObjectInfo;

	// Output
	SVVariantValueObjectClass	m_Value;
};
