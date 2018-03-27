//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:54:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVStdImageOperatorListClass.h"
#include "InspectionEngine/SVDataBuffer.h"
#pragma endregion Includes

class SVLinearImageOperatorListClass : public SVStdImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVLinearImageOperatorListClass )

public:
	SVLinearImageOperatorListClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARIMAGEOPERATORLIST );
	virtual ~SVLinearImageOperatorListClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

protected:
	HRESULT getUseRotationAngle(BOOL& rUseRotationAngle);
	HRESULT getInputProfileOrientation(long& rProfileOrientation);

	SvOl::SVInObjectInfoStruct inputUseRotationAngle;
	SvOl::SVInObjectInfoStruct inputProfileOrientation;

	SVDoubleValueObjectClass m_svLinearData;

	SVDoubleValueObjectClass m_svMinThreshold;
	SVDoubleValueObjectClass m_svMaxThreshold;

	SVDataBufferClass m_svProfileResultData;

	unsigned long m_ulLineLength;
	SVMatroxLongArray m_aulLineData;

	std::vector<double> m_svArray;

private:
	void init();

	HRESULT UpdateLineExtentData();

	/// If Rotation used, remove ROIImage, if not add ROIImage to the embeddedList.
	void ResetLogicalROIImage();
};

