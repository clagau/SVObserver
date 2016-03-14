//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzer
//* .File Name       : $Workfile:   SVOCRGrayAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:55:32  $
//******************************************************************************

// 19 Ju 1999 - frb.
//
#ifndef SVOCRGRAYANALYZER_H
#define SVOCRGRAYANALYZER_H

#include "SVAnalyzer.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCRGrayAnalyzerClass
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
class SVOCRGrayAnalyzerClass : public SVImageAnalyzerClass
{
protected:
	SV_DECLARE_CLASS( SVOCRGrayAnalyzerClass );

public:
	SVOCRGrayAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL , 
		int StringResourceID = IDS_CLASSNAME_SVOCRGRAYANALYZER
	);

private:
	void init();

public:
	virtual ~SVOCRGrayAnalyzerClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

public:
	SVResultClass* GetResultObject();
public:
	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun(  SVRunStatusClass& RRunStatus );

public:
	virtual BOOL IsPtOverResult(CPoint point );
	virtual	void DisplayAnalyzerResult( void);

};

#endif //__SVOCRGRAYANALYZER_H__

