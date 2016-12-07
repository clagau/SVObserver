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
#pragma once

#pragma region Includes
#include "SVImageAnalyzerClass.h"
#pragma endregion Includes

class SVOCVAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVOCVAnalyzerClass );

public:
	SVOCVAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = nullptr, 
		int StringResourceID = IDS_CLASSNAME_SVOCVANALYZER
	);

	virtual ~SVOCVAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual HRESULT ResetObject() override;

	SVResultClass* GetResultObject();

	virtual BOOL OnValidate() override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual void DisplayAnalyzerResult() override;
	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

private:
	void init();
	bool m_bHasLicenseError;
};

