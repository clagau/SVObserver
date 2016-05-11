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

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

protected:
	SVDoubleValueObjectClass*		getOutputMathResult();

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

protected:
	// Embedded Object: ( Outputs )
	SVDoubleValueObjectClass m_outputMathResult;
};
