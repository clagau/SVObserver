//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointYResult
//* .File Name       : $Workfile:   SVDPointYResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{
class SVDPointValueObjectClass;
}

namespace SvOp
{

class SVDPointYResult : public SVResult  
{
	SV_DECLARE_CLASS( SVDPointYResult );

public:
	SVDPointYResult( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_Y );
	virtual ~SVDPointYResult();
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	SvVol::SVDPointValueObjectClass* getInputPoint();
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	// Output
	SvVol::SVDoubleValueObjectClass		y;
};

} //namespace SvOp
