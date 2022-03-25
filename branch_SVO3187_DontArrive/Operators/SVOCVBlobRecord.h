//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVValueObjectLibrary/SVCharValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVOCVCharacterResult : public SvIe::SVTaskObjectClass
{
protected:
	SV_DECLARE_CLASS

public:
	explicit SVOCVCharacterResult(LPCSTR ObjectName );
	SVOCVCharacterResult(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVCHARACTERRESULTOBJECT );
	virtual ~SVOCVCharacterResult();

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

typedef std::vector<SVOCVCharacterResult*> SVOCVCharacterResultPtrVector;

} //namespace SvOp
