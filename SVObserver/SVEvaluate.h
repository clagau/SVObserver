//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#pragma endregion Includes

class SVEvaluateClass : public SVMathContainerClass
{
	SV_DECLARE_CLASS( SVEvaluateClass )

public:
	SVEvaluateClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE );
	virtual ~SVEvaluateClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

protected:
	SVDoubleValueObjectClass*		getOutputMathResult();

	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:
	// Embedded Object: ( Outputs )
	SVDoubleValueObjectClass m_outputMathResult;
};
