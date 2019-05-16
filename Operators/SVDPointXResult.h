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

class SVDPointXResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDPointXResultClass );
public:
	SVDPointXResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_DPOINT_X );
	virtual ~SVDPointXResultClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	SvVol::SVDPointValueObjectClass* getInputPoint();
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	// Output
	SvVol::SVDoubleValueObjectClass		x;
};

} //namespace SvOp

