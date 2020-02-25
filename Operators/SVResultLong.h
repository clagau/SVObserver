//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultLong
//* .File Name       : $Workfile:   SVResultLong.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:56:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLongResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVLongResultClass );

public:
	SVLongResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_LONG );
	virtual ~SVLongResultClass();

public:
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	SvPb::EmbeddedIdEnum GetInputEmbeddedID() const;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

// Data Element(s):
protected:
	// Output
	SvVol::SVLongValueObjectClass m_Value;
};

} //namespace SvOp
