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

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	SVDoubleValueObjectClass* getInputCenterXResult(bool bRunMode = false);
	SVDoubleValueObjectClass* getInputCenterYResult(bool bRunMode = false);

	SVDoubleValueObjectClass* getInputStartRadiusResult(bool bRunMode = false);
	SVDoubleValueObjectClass* getInputEndRadiusResult(bool bRunMode = false);

	SVDoubleValueObjectClass* getInputStartAngleResult(bool bRunMode = false);
	SVDoubleValueObjectClass* getInputEndAngleResult(bool bRunMode = false);

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SVInObjectInfoStruct m_inputCenterXResult;
	SVInObjectInfoStruct m_inputCenterYResult;
	
	SVInObjectInfoStruct m_inputStartRadiusResult;
	SVInObjectInfoStruct m_inputEndRadiusResult;

	SVInObjectInfoStruct m_inputStartAngleResult;
	SVInObjectInfoStruct m_inputEndAngleResult;
};


