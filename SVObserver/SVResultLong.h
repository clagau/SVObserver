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
#pragma endregion Includes

class SVLongResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVLongResultClass );

public:
	SVLongResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_LONG );
	virtual ~SVLongResultClass();

public:
	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;

	const GUID & GetInputEmbeddedID() const;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

// Data Element(s):
protected:
	// Output
	SVLongValueObjectClass m_Value;
};

