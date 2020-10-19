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
#include "SVStdImageOperatorListClass.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLinearImageOperatorList : public SVStdImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVLinearImageOperatorList )

public:
	SVLinearImageOperatorList( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARIMAGEOPERATORLIST );
	virtual ~SVLinearImageOperatorList();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
		
protected:
	HRESULT getUseRotationAngle(BOOL& rUseRotationAngle);
	HRESULT getInputProfileOrientation(long& rProfileOrientation);

	SvOl::SVInObjectInfoStruct inputUseRotationAngle;
	SvOl::SVInObjectInfoStruct inputProfileOrientation;

	SvVol::SVDoubleValueObjectClass m_svLinearData;

	SvVol::SVDoubleValueObjectClass m_MinProfile;
	SvVol::SVDoubleValueObjectClass m_MaxProfile;
	SvVol::SVDoubleValueObjectClass m_ProfileDelta;


	SvIe::SVDataBufferClass m_svProfileResultData;

	unsigned long m_ulLineLength;
	std::vector<long> m_aulLineData;

	std::vector<double> m_svArray;

private:
	void init();

	HRESULT UpdateLineExtentData();

	/// If Rotation used, remove ROIImage, if not add ROIImage to the embeddedList.
	void ResetLogicalROIImage();

	bool RunLocalRotation(RunStatus &rRunStatus, SvOi::SVImageBufferHandlePtr input, SvTrc::IImagePtr pOutputBuffer, const SVImageExtentClass& rImageExtent);
};

} //namespace SvOp
