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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVRESULTDOUBLE_H
#define SVRESULTDOUBLE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"



class SVDoubleResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVDoubleResultClass );

public:
	SVDoubleResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_DOUBLE );
	virtual ~SVDoubleResultClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();
	SVDoubleValueObjectClass* getInputDouble();
protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

// Data Element(s):
protected:
	// Output
	SVDoubleValueObjectClass		value;
};



#endif	// SVRESULTDOUBLE_H

