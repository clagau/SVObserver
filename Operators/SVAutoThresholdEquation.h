//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoThresholdEquation
//* .File Name       : $Workfile:   SVAutoThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVAutoThresholdEquation : public SVEquation
{
	SV_DECLARE_CLASS( SVAutoThresholdEquation);

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVAutoThresholdEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVAUTOTHRESHOLDEQUATION );
	virtual ~SVAutoThresholdEquation();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	void init();
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SvVol::SVDoubleValueObjectClass		result;			// resultant value
};

} //namespace SvOp
