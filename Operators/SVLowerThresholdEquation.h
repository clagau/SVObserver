//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLowerThresholdEquation
//* .File Name       : $Workfile:   SVLowerThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:50  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLowerThresholdEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVLowerThresholdEquationClass);

public:
	SVLowerThresholdEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLOWERTHRESHOLDEQUATION );
	virtual ~SVLowerThresholdEquationClass();

protected:
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	void init();
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SvVol::SVDoubleValueObjectClass		result;			// resultant value
};

} //namespace SvOp
