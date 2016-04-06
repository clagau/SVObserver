//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#pragma endregion Includes

class SVToolClass;

class SVPolarTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVPolarTransformClass )

public:
	SVPolarTransformClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORM );
	virtual ~SVPolarTransformClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVDoubleValueObjectClass* getInputCenterXResult();
	SVDoubleValueObjectClass* getInputCenterYResult();

	SVDoubleValueObjectClass* getInputStartRadiusResult();
	SVDoubleValueObjectClass* getInputEndRadiusResult();

	SVDoubleValueObjectClass* getInputStartAngleResult();
	SVDoubleValueObjectClass* getInputEndAngleResult();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SVInObjectInfoStruct inputCenterXResult;
	SVInObjectInfoStruct inputCenterYResult;
	
	SVInObjectInfoStruct inputStartRadiusResult;
	SVInObjectInfoStruct inputEndRadiusResult;

	SVInObjectInfoStruct inputStartAngleResult;
	SVInObjectInfoStruct inputEndAngleResult;
};


