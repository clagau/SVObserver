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
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVImageTransform : public SVTransform
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	SVImageTransform( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETRANSFORM );
	virtual ~SVImageTransform();
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
	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( RunStatus& runStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

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
	SvOl::InputObject	m_inputImage;

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
