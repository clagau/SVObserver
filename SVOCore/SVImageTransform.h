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
#include "SVImageClass.h"
#pragma endregion Includes

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

	SVImageClass* getInputImage() const;
	SVImageClass* getOutputImage();
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
	SVImageClass				m_outputImageObject;
	SVBoolValueObjectClass		m_useExtentsOnly;

	// Inputs
	SVInObjectInfoStruct		m_inputImageObjectInfo;

	SVDoubleValueObjectClass	m_extentWidth;
	SVDoubleValueObjectClass	m_extentHeight;

	SVDoubleValueObjectClass	m_extentDisplacementX;
	SVDoubleValueObjectClass	m_extentDisplacementY;
	
	SVDoubleValueObjectClass	m_extentSourceX;
	SVDoubleValueObjectClass	m_extentSourceY;

	SVDoubleValueObjectClass	m_extentRotationAngle;
	SVEnumerateValueObjectClass m_interpolationMode;
#pragma region
};

