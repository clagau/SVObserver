//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUpperThresholdEquation
//* .File Name       : $Workfile:   SVUpperThresholdEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:56:32  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "SVValueObjectLibrary\SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVUpperThresholdEquation : public SVEquation
{
	SV_DECLARE_CLASS

public:
	SVUpperThresholdEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUPPERTHRESHOLDEQUATION );
	virtual ~SVUpperThresholdEquation();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	void init();
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	SvVol::SVDoubleValueObjectClass		result;			// resultant value
};

} //namespace SvOp
