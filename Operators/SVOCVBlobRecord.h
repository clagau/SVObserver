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
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVTaskObject.h"
#pragma endregion Includes

namespace SvOp
{

class SVOCVCharacterResultClass : public SvIe::SVTaskObjectClass
{
protected:
	SV_DECLARE_CLASS( SVOCVCharacterResultClass )

public:
	SVOCVCharacterResultClass(LPCSTR ObjectName );
	SVOCVCharacterResultClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVCHARACTERRESULTOBJECT );
	virtual ~SVOCVCharacterResultClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	void init();

	void HideResults();
	void UnhideResults();

protected:

//Attributes
public:
	SvVol::SVCharValueObjectClass	 m_cvoLabelValue;

	SvVol::SVDoubleValueObjectClass m_dvoOverlayLeft;
	SvVol::SVDoubleValueObjectClass m_dvoOverlayTop;
	SvVol::SVDoubleValueObjectClass m_dvoOverlayWidth;
	SvVol::SVDoubleValueObjectClass m_dvoOverlayHeight;

	SvVol::SVDoubleValueObjectClass m_dvoMatchScore;
};

typedef std::vector<SVOCVCharacterResultClass*> SVOCVCharacterResultPtrVector;

} //namespace SvOp