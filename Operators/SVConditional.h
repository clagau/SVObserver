//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditional
//* .File Name       : $Workfile:   SVConditional.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEquation.h"
#include "ObjectInterfaces/ITool.h"
#pragma endregion Includes

namespace SvOp
{

class SVConditional : public SVEquation
{
	SV_DECLARE_CLASS( SVConditional );

//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVConditional( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCONDITIONAL );
	virtual ~SVConditional();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvOi::ParameterPairForML getResultData() const { return SvOi::ParameterPairForML {result.GetCompleteName(), result.getObjectId()}; };

protected:
	void init();

protected:
	SvVol::SVBoolValueObjectClass result;			// resultant value
};

} //namespace SvOp
