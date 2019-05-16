//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultDouble
//* .File Name       : $Workfile:   SVResultDouble.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Jul 2014 15:15:22  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVResult.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVDoubleResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDoubleResultClass );

public:
	SVDoubleResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_DOUBLE );
	virtual ~SVDoubleResultClass();

public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

// Data Element(s):
protected:
	// Output
	SvVol::SVDoubleValueObjectClass m_Value;
};

} //namespace SvOp
