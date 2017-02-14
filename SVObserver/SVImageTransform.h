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
#include "SVOCore/SVImageClass.h"
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
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;

	virtual bool ResetObject( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion

	SVImageClass* getInputImage();
	SVImageClass* getOutputImage();
#pragma endregion

#pragma region Protected Methods
protected:
	virtual BOOL onRun( SVRunStatusClass& runStatus ) override;

	HRESULT UpdateTransformData( long p_lIndex );
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

