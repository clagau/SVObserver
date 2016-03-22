//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineROI
//* .File Name       : $Workfile:   SVLineROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:12:42  $
//******************************************************************************

#pragma once

#include "SVROI.h"
#include "SVLine.h"

class SVLineROIClass : public SVROIClass
{
	SV_DECLARE_CLASS( SVLineROIClass )

public:
	SVLineROIClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLINEROI );
	virtual ~SVLineROIClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	SVLineClass* getOutputLine();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVLineClass outputLineObject; // Embedded

};

