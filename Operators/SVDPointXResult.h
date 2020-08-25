//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointResultX
//* .File Name       : $Workfile:   SVDPointXResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma region Includes

namespace SvVol
{
class SVDPointValueObjectClass;
}

namespace SvOp
{

class SVDPointXResult : public SVResult  
{
	SV_DECLARE_CLASS( SVDPointXResult );
public:
	SVDPointXResult( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_X );
	virtual ~SVDPointXResult();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	SvVol::SVDPointValueObjectClass* getInputPoint();
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	// Output
	SvVol::SVDoubleValueObjectClass		x;
};

} //namespace SvOp

