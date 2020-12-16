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
#include "InspectionEngine/SVImageClass.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVEquation;

class SVImagePolarTransform : public SVPolarTransform
{
	SV_DECLARE_CLASS( SVImagePolarTransform )

public:
	SVImagePolarTransform( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEPOLARTRANSFORM );
	virtual ~SVImagePolarTransform();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	SvIe::SVImageClass* GetOutputImage();

	bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	void setSliceOverlay(SvPb::OverlayShapeSlice& rSlice) const;

protected:
	void NewCorrectAngles( double& RDStartAngle, double& RDEndAngle );
	void correctAngles( double& RDStartAngle, double& RDEndAngle );
	void AnglesTo360( double& p_dStart, double& p_dEnd);

	bool SetDefaultEquation( SVEquation* pEquation, const std::string& rName, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvIe::SVImageClass	m_outputImage;
	
	// Tool's center x value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_centerX;
	// Tool's center y value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_centerY;
	// Tool's start radius value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_startRadius;
	// Tool's end radius value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_endRadius;
	// Tool's start angle value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_startAngle;
	// Tool's end angle value, comes from mouse positioning...
	SvVol::SVDoubleValueObjectClass m_endAngle;

	// If TRUE we use the result inputs from evaluate objects,
	//	which are then used to overwrite centerX, centerY, etc. in onRun(...). 
	//
	// If FALSE we don't overwrite in onRun(...), so the values are free
	//	to be changed by mouse movings.
	SvVol::SVBoolValueObjectClass m_useFormulaInput;

	// M_NEAREST_NEIGHBOR or M_BILINEAR or M_BICUBIC
	//	plus: M_OVERSCAN_ENABLE or M_OVERSCAN_DISABLE or M_OVERSCAN_CLEAR
	SvVol::SVEnumerateValueObjectClass m_interpolationMode;

	// Inputs
	SvOl::InputObject		m_inputImage;

private:
	void SetCalculatedPrintableFlags();
	static double g_dMaxAngularDistance;

	HRESULT CollectInputImageNames();

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
};

} //namespace SvOp
