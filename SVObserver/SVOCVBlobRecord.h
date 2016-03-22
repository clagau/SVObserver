//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVBlobRecord
//* .File Name       : $Workfile:   SVOCVBlobRecord.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:26  $
//******************************************************************************

#include "SVTaskObject.h"

#pragma once

class SVOCVCharacterResultClass : public SVTaskObjectClass
{
protected:
	SV_DECLARE_CLASS( SVOCVCharacterResultClass )

public:
	SVOCVCharacterResultClass(LPCSTR ObjectName );
	SVOCVCharacterResultClass(SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOCVCHARACTERRESULTOBJECT );
	~SVOCVCharacterResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	void init();

	void HideResults();
	void UnhideResults();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//Attributes
public:
	SVCharValueObjectClass	 m_cvoLabelValue;

	SVDoubleValueObjectClass m_dvoOverlayLeft;
	SVDoubleValueObjectClass m_dvoOverlayTop;
	SVDoubleValueObjectClass m_dvoOverlayWidth;
	SVDoubleValueObjectClass m_dvoOverlayHeight;

	SVDoubleValueObjectClass m_dvoMatchScore;
};
typedef SVVector< SVOCVCharacterResultClass* > SVOCVCharacterResultArray;


