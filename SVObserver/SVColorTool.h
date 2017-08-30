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
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

//! From the Matrox Imaging Library Help file for MbufChildColor2d, the Band parameter specifies the index of the band to use. 
//! Valid index values are from 0 to (number of bands of the buffer - 1). 
//! Band 0 corresponds to: the red band (for RGB parent buffers), the hue band (for HSI parent buffers) 
//! Band 1 corresponds to: the green band (for RGB parent buffers), the saturation band (for HSI parent buffers)
//! Band 2 corresponds to: the blue band (for RGB parent buffers), the luminance band (for HSI parent buffers)
enum BandEnum
{
	Band0 = 0,				//! Red or Hue band
	Band1 = 1,				//! Green or Saturation Band 
	Band2 = 2,				//! Blue or Luminance Band
	BandNumber = 3			//! The number of bands
};

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
	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool DoesObjectHaveExtents() const override { return true; };
	virtual SVStringValueObjectClass* GetInputImageNames() override { return &m_SourceImageNames; };

	SVImageClass* getBand0Image() { return &m_bandImage[BandEnum::Band0]; };

	SVBoolValueObjectClass* GetConvertToHSIVariable() { return &m_convertToHSI; };

	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent) override;
	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtentToFit(const SVImageExtentClass& rImageExtent) override;

	virtual SVTaskObjectClass* GetObjectAtPoint(const SVExtentPointStruct &rPoint) override;
	SVImageClass* getOutputImage() { return &m_OutputImage; };

	bool isConverted() { return m_ConvertTool; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual HRESULT IsInputImage(SVImageClass *pImage) override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//! Initialize the class
	void LocalInitialize();

	SVImageClass* getInputImage() { return m_pInputImage; };
	bool createBandChildLayer(BandEnum Band);
	HRESULT CollectInputImageNames();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Embeddeds
	SVImageClass m_OutputImage;
	SVImageClass m_LogicalROIImage;
	SVImageClass m_bandImage[BandEnum::BandNumber];

	SVBoolValueObjectClass m_convertToHSI;
	SVBoolValueObjectClass m_hasROI;
	// String value object for Source Image Names
	SVStringValueObjectClass m_SourceImageNames;

	SVImageClass* m_pInputImage;

	bool m_ConvertTool;
#pragma endregion Member Variables
};

