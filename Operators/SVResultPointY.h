//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointY
//* .File Name       : $Workfile:   SVResultPointY.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma endregion Includes

namespace SvOp
{

class SVPointYResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVPointYResultClass );

public:
	SVPointYResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_POINT_Y );
	virtual ~SVPointYResultClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Output
	SvVol::SVLongValueObjectClass m_Y;
};

} //namespace SvOp