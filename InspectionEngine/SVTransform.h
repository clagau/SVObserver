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

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SvOl::SVInObjectInfoStruct m_inputTranslationXResult;
	SvOl::SVInObjectInfoStruct m_inputTranslationYResult;
	
	SvOl::SVInObjectInfoStruct m_inputRotationXResult;
	SvOl::SVInObjectInfoStruct m_inputRotationYResult;
	SvOl::SVInObjectInfoStruct m_inputRotationAngleResult;

	// Embedded Objects (outputs)
	SVBoolValueObjectClass m_performTranslation;
	SVBoolValueObjectClass m_performRotation;

	SVDoubleValueObjectClass    m_learnedTranslationX;
	SVDoubleValueObjectClass    m_learnedTranslationY;
	SVDoubleValueObjectClass    m_learnedRotationX;
	SVDoubleValueObjectClass    m_learnedRotationY;
	SVDoubleValueObjectClass    m_learnedRotationAngle;
};

