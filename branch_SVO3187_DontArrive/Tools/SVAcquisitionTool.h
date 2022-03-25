//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:10:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "InspectionEngine/SVMainImageClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVAcquisitionToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
	SVAcquisitionToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVACQUISITIONTOOL );

	virtual ~SVAcquisitionToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames( ) override;
	virtual void overwriteInputSource(SvOi::SVImageBufferHandlePtr imageHandlePtr) override;
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse&) const override;
	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;

protected:
	virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
	virtual bool useOverlayColorTool() const override { return false; };
	void init();

	// Embedded Objects:
	SvIe::SVMainImageClass	mainImageObject;	// Embedded

	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;

	SvOi::SVImageBufferHandlePtr m_replaceSourceImage; ///< This image handle is set if regressionTest will replace the source image. Than use this.
};

} //namespace SvTo
