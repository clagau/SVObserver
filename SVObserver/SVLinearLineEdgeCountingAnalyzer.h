//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgeCountingAnalyzer
//* .File Name       : $Workfile:   SVLinearLineEdgeCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:59:00  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#pragma endregion Includes

class SVLinearEdgeCountingLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearEdgeCountingLineAnalyzerClass );

public:
	SVLinearEdgeCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	virtual ~SVLinearEdgeCountingLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual BOOL OnValidate() override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

	SVLongValueObjectClass	m_svEdgeCount;

private:
	void init();
};


