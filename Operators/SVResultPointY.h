//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVValueObjectLibrary\SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVPointYResult : public SVResult  
{
	SV_DECLARE_CLASS

public:
	SVPointYResult( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_POINT_Y );
	virtual ~SVPointYResult();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Output
	SvVol::SVLongValueObjectClass m_Y;
};

} //namespace SvOp
