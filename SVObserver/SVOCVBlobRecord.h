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
#pragma once

#pragma region Includes
#include "SVOCore/SVTaskObject.h"
#pragma endregion Includes

class SVOCVCharacterResultClass : public SVTaskObjectClass
{
protected:
	SV_DECLARE_CLASS( SVOCVCharacterResultClass )

public:
	SVOCVCharacterResultClass(LPCSTR ObjectName );
	SVOCVCharacterResultClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVCHARACTERRESULTOBJECT );
	virtual ~SVOCVCharacterResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	void init();

	void HideResults();
	void UnhideResults();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

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


