//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzerClass
//* .File Name       : $Workfile:   SVOCRAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:50:48  $
//******************************************************************************

#pragma once

#include "SVAnalyzer.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCRAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVOCRAnalyzerClass : public SVImageAnalyzerClass
{
protected:
	SV_DECLARE_CLASS( SVOCRAnalyzerClass );

public:
	SVOCRAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL , 
		int StringResourceID = IDS_CLASSNAME_SVOCRANALYZER 
	);

private:
	void init();

public:
	virtual ~SVOCRAnalyzerClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

public:
	SVResultClass* GetResultObject();
public:
	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun(  SVRunStatusClass& RRunStatus );

};

