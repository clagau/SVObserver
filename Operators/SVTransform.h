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
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVTransform : public SvIe::SVTaskObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVTransform )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVTransform( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRANSFORM );
	virtual ~SVTransform();
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
	SvVol::SVBoolValueObjectClass m_performTranslation;
	SvVol::SVBoolValueObjectClass m_performRotation;

	SvVol::SVDoubleValueObjectClass    m_learnedTranslationX;
	SvVol::SVDoubleValueObjectClass    m_learnedTranslationY;
	SvVol::SVDoubleValueObjectClass    m_learnedRotationX;
	SvVol::SVDoubleValueObjectClass    m_learnedRotationY;
	SvVol::SVDoubleValueObjectClass    m_learnedRotationAngle;
};

} //namespace SvOp
