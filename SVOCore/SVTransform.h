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

#pragma region Includes
#include "SVTaskObject.h"
#pragma endregion Includes

class SVTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVTransformClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVTransformClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRANSFORM );
	virtual ~SVTransformClass();
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	SVDoubleValueObjectClass* getInputTranslationXResult();
	SVDoubleValueObjectClass* getInputTranslationYResult();
	SVDoubleValueObjectClass* getInputRotationXResult();
	SVDoubleValueObjectClass* getInputRotationYResult();
	SVDoubleValueObjectClass* getInputRotationAngleResult();

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

