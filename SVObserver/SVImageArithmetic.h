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
#include "InspectionEngine/SVTaskObject.h"
#include "InspectionEngine/SVImageClass.h"
#include "Definitions/ImageOperatorEnums.h"
#pragma endregion Includes

class SVToolClass;
class SVIPDoc;

class SVImageArithmeticClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVImageArithmeticClass )

public:
	SVImageArithmeticClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEARITHMETIC );
	virtual ~SVImageArithmeticClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVImageClass* getInputImageA() const;
	SVImageClass* getInputImageB() const;
	SVImageClass* getOutputImage();

protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	SVBoolValueObjectClass*		getInputEnableOffsetA() const;
	SVPointValueObjectClass*	getInputOffsetAPoint() const;

	SVBoolValueObjectClass*		getInputEnableOffsetB() const;
	SVPointValueObjectClass*	getInputOffsetBPoint() const;
	
	SVLongValueObjectClass*		getInputArithOperator() const;

	void ScaleWithAveraging( SVImageClass* pInputImage, SVImageClass* pOutputImage );
	
	HRESULT CollectInputImageNames( );

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SVInObjectInfoStruct		m_InputImageAInfo;
	SVInObjectInfoStruct		m_InputEnableOffsetAInfo;
	SVInObjectInfoStruct		m_InputOffsetAPointInfo;

	SVInObjectInfoStruct		m_InputImageBInfo;
	SVInObjectInfoStruct		m_InputEnableOffsetBInfo;
	SVInObjectInfoStruct		m_InputOffsetBPointInfo;

	SVInObjectInfoStruct		m_InputArithmaticOperatorInfo;

	// Embedded Object:
	SVImageClass				m_OutputImage;

};

