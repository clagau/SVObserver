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

	SVImageClass* getOutputImage() { return &m_OutputImage; }

protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void ScaleWithAveraging(SvTrc::IImagePtr pInputImageBuffer, const RECT& rInputRect, SvTrc::IImagePtr pOutputImageBuffer, const RECT& rOutputRect);
	
	HRESULT CollectInputImageNames( );

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SvOl::SVInObjectInfoStruct	m_InputImageAInfo;
	SvOl::SVInObjectInfoStruct	m_InputEnableOffsetAInfo;
	SvOl::SVInObjectInfoStruct	m_InputOffsetAPointInfo;

	SvOl::SVInObjectInfoStruct	m_InputImageBInfo;
	SvOl::SVInObjectInfoStruct	m_InputEnableOffsetBInfo;
	SvOl::SVInObjectInfoStruct	m_InputOffsetBPointInfo;

	SvOl::SVInObjectInfoStruct	m_InputArithmaticOperatorInfo;

	// Embedded Object:
	SVImageClass				m_OutputImage;

};

