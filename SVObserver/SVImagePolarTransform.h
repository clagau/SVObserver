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
#include "SVImageClass.h"
#include "SVPolarTransform.h"
#pragma endregion Includes

class SVImagePolarTransformClass : public SVPolarTransformClass
{
	SV_DECLARE_CLASS( SVImagePolarTransformClass )

public:
	SVImagePolarTransformClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEPOLARTRANSFORM );
	virtual ~SVImagePolarTransformClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual BOOL OnValidate();

	HRESULT ResetObject();

	SVImageClass* GetOutputImage();

	BOOL SetDefaultFormulas();

	SVImageClass* getInputImage();

protected:
	void NewCorrectAngles( double& RDStartAngle, double& RDEndAngle );
	void correctAngles( double& RDStartAngle, double& RDEndAngle );
	void AnglesTo360( double& p_dStart, double& p_dEnd);

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVImageClass				outputImageObject;
	
	// Tool's center x value, comes from mouse positioning...
	SVDoubleValueObjectClass centerX;
	// Tool's center y value, comes from mouse positioning...
	SVDoubleValueObjectClass centerY;
	// Tool's start radius value, comes from mouse positioning...
	SVDoubleValueObjectClass startRadius;
	// Tool's end radius value, comes from mouse positioning...
	SVDoubleValueObjectClass endRadius;
	// Tool's start angle value, comes from mouse positioning...
	SVDoubleValueObjectClass startAngle;
	// Tool's end angle value, comes from mouse positioning...
	SVDoubleValueObjectClass endAngle;

	// If TRUE we use the result inputs from evaluate objects,
	//	which are then used to overwrite centerX, centerY, etc. in onRun(...). 
	//
	// If FALSE we don't overwrite in onRun(...), so the values are free
	//	to be changed by mouse movings.
	SVBoolValueObjectClass useFormulaInput;

	// M_NEAREST_NEIGHBOR or M_BILINEAR or M_BICUBIC
	//	plus: M_OVERSCAN_ENABLE or M_OVERSCAN_DISABLE or M_OVERSCAN_CLEAR
	SVEnumerateValueObjectClass interpolationMode;

	// Inputs
	SVInObjectInfoStruct		inputImageObjectInfo;

private:
	void SetCalculatedPrintableFlags();
	static double g_dMaxAngularDistance;

	HRESULT CollectInputImageNames();
};

