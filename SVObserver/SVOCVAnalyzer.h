//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzer
//* .File Name       : $Workfile:   SVOCVAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************

#ifndef SVOCVANALYZERCLASS_H
#define SVOCVANALYZERCLASS_H

#include "SVAnalyzer.h"

class SVOCVAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVOCVAnalyzerClass );

public:
	SVOCVAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL , 
		int StringResourceID = IDS_CLASSNAME_SVOCVANALYZER
	);

	virtual ~SVOCVAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	SVResultClass* GetResultObject();

	virtual BOOL OnValidate();

	virtual BOOL IsPtOverResult( CPoint point );
	virtual	void DisplayAnalyzerResult( void );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

private:
	void init();
	bool m_bHasLicenseError;

};

#endif

