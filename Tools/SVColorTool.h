//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
	SV_DECLARE_CLASS( SVColorToolClass );

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

	virtual SvIe::SVTaskObjectClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	SvIe::SVImageClass* getOutputImage() { return &m_OutputImage; };

	bool isConverted() { return m_ConvertTool; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
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

	SvOl::SVInObjectInfoStruct m_InputImageObjectInfo;

	bool m_ConvertTool;
#pragma endregion Member Variables
};

} //namespace SvTo
