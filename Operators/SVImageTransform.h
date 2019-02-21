//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTransform
//* .File Name       : $Workfile:   SVImageTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 12:44:00  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTransform.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVImageTransformClass : public SVTransformClass
{
	SV_DECLARE_CLASS( SVImageTransformClass )

#pragma region Constructor
public:
	SVImageTransformClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETRANSFORM );
	virtual ~SVImageTransformClass();
#pragma endregion

#pragma region Public Methods
public:
#pragma region virtual
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion

	SvIe::SVImageClass* getOutputImage();
#pragma endregion

#pragma region Protected Methods
protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& runStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	HRESULT UpdateTransformData();
#pragma endregion

#pragma region Private Methods
private:
	HRESULT CollectInputImageNames();
#pragma endregion

#pragma region Member variables
protected:
	// Embedded Object: ( Outputs )
	SvIe::SVImageClass			m_outputImage;
	SvVol::SVBoolValueObjectClass		m_useExtentsOnly;

	// Inputs
	SvOl::SVInObjectInfoStruct	m_inputImageObjectInfo;

	SvVol::SVDoubleValueObjectClass	m_extentWidth;
	SvVol::SVDoubleValueObjectClass	m_extentHeight;

	SvVol::SVDoubleValueObjectClass	m_extentDisplacementX;
	SvVol::SVDoubleValueObjectClass	m_extentDisplacementY;
	
	SvVol::SVDoubleValueObjectClass	m_extentSourceX;
	SvVol::SVDoubleValueObjectClass	m_extentSourceY;

	SvVol::SVDoubleValueObjectClass	m_extentRotationAngle;
	SvVol::SVEnumerateValueObjectClass m_interpolationMode;
#pragma region
};

} //namespace SvOp