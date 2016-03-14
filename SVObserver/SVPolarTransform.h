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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVPOLARTRANSFORM_H
#define SVPOLARTRANSFORM_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObject.h"

class SVToolClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVPolarTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVPolarTransformClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPolarTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORM );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPolarTransformClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
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


	// Embedded Objects (outputs)
	//SVBoolValueObjectClass performPolarTransformation;
};


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVPOLARTRANSFORM_H

