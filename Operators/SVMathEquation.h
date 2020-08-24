//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathEquation
//* .File Name       : $Workfile:   SVMathEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVMathEquation : public SVEquation
{
	SV_DECLARE_CLASS( SVMathEquation);

public:
	SVMathEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMATHEQUATION );
	virtual ~SVMathEquation();

protected:
	void init();
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SvVol::SVDoubleValueObjectClass result;			// resultant value
};

} //namespace SvOp
