//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorTool
//* .File Name       : $Workfile:   SVColorTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 17:26:46  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "InspectionEngine/SVImageClass.h"
#include "SVTool.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "Definitions/BandEnums.h"
#pragma endregion Includes


namespace SvTo
{

//! The class has the main image (color) as an input image and separates it into 3 output image (Bands) in either the RGB bands
//! or HSI bands when HSI conversion is set
//! The tool now has an ROI which can be seen on the input image, older configurations are converted to the full image extents of the parent for compatibility
//! The ROI image is also an output image and is the input image for the band separation
//! The tool also has the Color Threshold as and embedded value
class SVColorToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	SVColorToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCOLORTOOL );

	virtual ~SVColorToolClass();
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool DoesObjectHaveExtents() const override { return true; };
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override { return &m_SourceImageNames; };

	SvIe::SVImageClass* getBand0Image() { return &m_bandImage[SvDef::BandEnum::Band0]; };

	SvVol::SVBoolValueObjectClass* GetConvertToHSIVariable() { return &m_convertToHSI; };

	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent) override;
	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtentToFit(const SVImageExtentClass& rImageExtent) override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

	virtual SvPb::SVExtentLocationPropertyEnum getLocationPropertyAtPoint(const SVPoint<double>& rPoint) const override;
	SvIe::SVImageClass* getOutputImage() { return &m_OutputImage; };

	bool isConverted() const { return m_ConvertTool; }
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//! Initialize the class
	void LocalInitialize();

	bool createBandChildLayer(SvDef::BandEnum Band);
	HRESULT CollectInputImageNames();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Embeddeds
	SvIe::SVImageClass m_OutputImage;
	SvIe::SVImageClass m_LogicalROIImage;
	SvIe::SVImageClass m_bandImage[SvDef::BandEnum::BandNumber];

	SvVol::SVBoolValueObjectClass m_convertToHSI;
	SvVol::SVBoolValueObjectClass m_hasROI;
	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;

	SvOl::InputObject m_InputImage;

	bool m_ConvertTool;
#pragma endregion Member Variables
};

} //namespace SvTo
