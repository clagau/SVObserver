//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransform
//* .File Name       : $Workfile:   SVTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 14:48:44  $
//******************************************************************************

#pragma once

#include "SVTaskObject.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVTransformClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTRANSFORM );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVTransformClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVDoubleValueObjectClass* getInputTranslationXResult();
	SVDoubleValueObjectClass* getInputTranslationYResult();
	SVDoubleValueObjectClass* getInputRotationXResult();
	SVDoubleValueObjectClass* getInputRotationYResult();
	SVDoubleValueObjectClass* getInputRotationAngleResult();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SVInObjectInfoStruct inputTranslationXResult;
	SVInObjectInfoStruct inputTranslationYResult;
	
	SVInObjectInfoStruct inputRotationXResult;
	SVInObjectInfoStruct inputRotationYResult;
	SVInObjectInfoStruct inputRotationAngleResult;

	// Embedded Objects (outputs)
	SVBoolValueObjectClass performTranslation;
	SVBoolValueObjectClass performRotation;

	SVDoubleValueObjectClass    learnedTranslationX;
	SVDoubleValueObjectClass    learnedTranslationY;
	SVDoubleValueObjectClass    learnedRotationX;
	SVDoubleValueObjectClass    learnedRotationY;
	SVDoubleValueObjectClass    learnedRotationAngle;
};
