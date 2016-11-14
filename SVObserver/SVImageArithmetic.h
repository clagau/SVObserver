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

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#include "SVImageClass.h"
#include "ObjectInterfaces\ImageArthimeticOperators.h"
#pragma endregion Includes

class SVToolClass;
class SVIPDoc;

class SVImageArithmeticClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVImageArithmeticClass )

public:
	SVImageArithmeticClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEARITHMETIC );
	virtual ~SVImageArithmeticClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	SVImageClass* getInputImageA();
	SVImageClass* getInputImageB();
	SVImageClass* getOutputImage();

	virtual BOOL OnValidate();

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;

protected:
	SVBoolValueObjectClass*		getInputEnableOffsetA();
	SVPointValueObjectClass*	getInputOffsetAPoint();

	SVBoolValueObjectClass*		getInputEnableOffsetB();
	SVPointValueObjectClass*	getInputOffsetBPoint();
	
	SVLongValueObjectClass*		getInputArithOperator();

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );

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

