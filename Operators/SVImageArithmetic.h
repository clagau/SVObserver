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
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes

namespace SvOp
{

class SVImageArithmetic : public SvIe::SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVImageArithmetic )

public:
	SVImageArithmetic( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEARITHMETIC );
	virtual ~SVImageArithmetic();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SvIe::SVImageClass* getOutputImage() { return &m_OutputImage; }

protected:
	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void ScaleWithAveraging(SvOi::ITRCImagePtr pInputImageBuffer, const RECT& rInputRect, SvOi::ITRCImagePtr pOutputImageBuffer, const RECT& rOutputRect);
	
	HRESULT CollectInputImageNames( );

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SvOl::InputObject	m_InputImageA;
	SvOl::InputObject	m_InputEnableOffsetA;
	SvOl::InputObject	m_InputOffsetAPoint;

	SvOl::InputObject	m_InputImageB;
	SvOl::InputObject	m_InputEnableOffsetB;
	SvOl::InputObject	m_InputOffsetBPoint;

	SvOl::InputObject	m_InputArithmaticOperator;

	// Embedded Object:
	SvIe::SVImageClass	m_OutputImage;

};

} //namespace SvOp
