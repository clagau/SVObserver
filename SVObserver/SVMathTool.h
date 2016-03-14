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

#ifndef SVMATHTOOL_H
#define SVMATHTOOL_H

#include "SVTool.h"

class SVMathEquationClass;

class SVMathToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVMathToolClass );

public:
	SVMathToolClass( BOOL BCreateDefaultTaskList=FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMATHTOOL );

	virtual ~SVMathToolClass();

	BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStruct );

	virtual BOOL OnValidate();

protected:
	virtual void init(void);

	SVMathEquationClass* pMathEquation;
};

#endif
