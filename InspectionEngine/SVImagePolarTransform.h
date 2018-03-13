//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePolarTransform
//* .File Name       : $Workfile:   SVImagePolarTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:46:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVPolarTransform.h"
#include "SVImageClass.h"
#pragma endregion Includes

class SVEquationClass;

class SVImagePolarTransformClass : public SVPolarTransformClass
{
	SV_DECLARE_CLASS( SVImagePolarTransformClass )

public:
	SVImagePolarTransformClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEPOLARTRANSFORM );
	virtual ~SVImagePolarTransformClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	SVImageClass* GetOutputImage();

	bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr);

protected:
	void NewCorrectAngles( double& RDStartAngle, double& RDEndAngle );
	void correctAngles( double& RDStartAngle, double& RDEndAngle );
	void AnglesTo360( double& p_dStart, double& p_dEnd);

	bool SetDefaultEquation( SVEquationClass* pEquation, const std::string& rName, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	
	SVImageClass				outputImageObject;
	
	// Tool's center x value, comes from mouse positioning...
	SVDoubleValueObjectClass m_centerX;
	// Tool's center y value, comes from mouse positioning...
	SVDoubleValueObjectClass m_centerY;
	// Tool's start radius value, comes from mouse positioning...
	SVDoubleValueObjectClass m_startRadius;
	// Tool's end radius value, comes from mouse positioning...
	SVDoubleValueObjectClass m_endRadius;
	// Tool's start angle value, comes from mouse positioning...
	SVDoubleValueObjectClass m_startAngle;
	// Tool's end angle value, comes from mouse positioning...
	SVDoubleValueObjectClass m_endAngle;

	// If TRUE we use the result inputs from evaluate objects,
	//	which are then used to overwrite centerX, centerY, etc. in onRun(...). 
	//
	// If FALSE we don't overwrite in onRun(...), so the values are free
	//	to be changed by mouse movings.
	SVBoolValueObjectClass m_useFormulaInput;

	// M_NEAREST_NEIGHBOR or M_BILINEAR or M_BICUBIC
	//	plus: M_OVERSCAN_ENABLE or M_OVERSCAN_DISABLE or M_OVERSCAN_CLEAR
	SVEnumerateValueObjectClass m_interpolationMode;

	// Inputs
	SvOl::SVInObjectInfoStruct		m_inputImageObjectInfo;

private:
	void SetCalculatedPrintableFlags();
	static double g_dMaxAngularDistance;

	HRESULT CollectInputImageNames();

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
};

