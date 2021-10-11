// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass.h
// * .File Name       : $Workfile:   SVLinearToolClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
const SvOi::NameValueVector cOrientationEnums{ {_T("Horizontal"), 0}, {_T("Vertical"), 90}};
#pragma endregion Declarations

namespace SvTo
{

class SVLinearToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
#pragma region Constructor
	SVLinearToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARTOOL );
	virtual ~SVLinearToolClass();
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

	bool GetRotation() const;

	virtual bool canResizeToParent() const override { return m_canResizeToParent && !GetRotation(); };


	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual SvPb::EAutoSize GetAutoSizeEnabled() const override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;

	virtual void addOverlays(const SvIe::SVImageClass* p_Image, SvPb::OverlayDesc& rOverlay) const override;
	virtual void overwriteInputSource(SvOi::SVImageBufferHandlePtr imageHandlePtr) override;
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse&) const override;
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
#pragma endregion Protected Methods

	SvVol::SVDoubleValueObjectClass m_svRotationAngle;
	SvVol::SVDoubleValueObjectClass m_svRotationPointX;
	SvVol::SVDoubleValueObjectClass m_svRotationPointY;

private:
#pragma region Private Methods
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvVol::SVStringValueObjectClass m_SourceImageNames;
	//Embedded Object : Profile Orientation
	SvVol::SVEnumerateValueObjectClass m_voProfileOrientation;
	SvVol::SVBoolValueObjectClass m_voUseProfileRotation;
#pragma endregion Member Variables
};

} //namespace SvTo
