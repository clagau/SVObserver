//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMathContainer.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluate : public SVMathContainer
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	SVEvaluate( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE );
	virtual ~SVEvaluate();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	SvVol::SVDoubleValueObjectClass*		getOutputMathResult();
protected:

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	// Embedded Object: ( Outputs )
	SvVol::SVDoubleValueObjectClass m_outputMathResult;
};

} //namespace SvOp
