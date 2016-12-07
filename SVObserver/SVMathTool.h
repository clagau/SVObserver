//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

class SVMathEquationClass;

class SVMathToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVMathToolClass );

public:
	SVMathToolClass( BOOL BCreateDefaultTaskList=FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMATHTOOL );

	virtual ~SVMathToolClass();

	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStruct ) override;

	virtual BOOL OnValidate() override;

private:
	void init(void);

	SVMathEquationClass* pMathEquation;
};
