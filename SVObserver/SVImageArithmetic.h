//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageArithmetic
//* .File Name       : $Workfile:   SVImageArithmetic.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:44:02  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#include "SVImageClass.h"
#include "ObjectInterfaces\ImageArthimeticOperators.h"
#pragma endregion Includes

class SVToolClass;
class SVIPDoc;

		
//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVImageArithmeticClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVImageArithmeticClass )

public:
	SVImageArithmeticClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVIMAGEARITHMETIC );
	virtual ~SVImageArithmeticClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	SVImageClass*				getInputImageA();
	SVImageClass*				getInputImageB();
	SVImageClass*				getOutputImage();

	virtual BOOL OnValidate();

protected:
	SVBoolValueObjectClass*		getInputEnableOffsetA();
	SVPointValueObjectClass*	getInputOffsetAPoint();

	SVBoolValueObjectClass*		getInputEnableOffsetB();
	SVPointValueObjectClass*	getInputOffsetBPoint();
	
	SVLongValueObjectClass*		getInputArithOperator();

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void ScaleWithAveraging( SVImageClass* pInputImage, SVImageClass* pOutputImage );

	SVInObjectInfoStruct		inputImageAInfo;
	SVInObjectInfoStruct		inputEnableOffsetAInfo;
	SVInObjectInfoStruct		inputOffsetAPointInfo;

	SVInObjectInfoStruct		inputImageBInfo;
	SVInObjectInfoStruct		inputEnableOffsetBInfo;
	SVInObjectInfoStruct		inputOffsetBPointInfo;

	SVInObjectInfoStruct		inputArithOperatorInfo;

	// Embedded Object:
	SVImageClass				outputImageObject;

	HRESULT CollectInputImageNames( );

};

