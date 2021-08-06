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
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/LookUp.h"
#pragma endregion Includes

namespace SvOp
{



class SVImageArithmetic : public SvIe::SVTaskObjectClass
{
	SV_DECLARE_CLASS

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
	bool getEnableGainAndOffset(BOOL& enableGain, SvStl::MessageContainerVector* pErrorMessages);
	bool getEnableLut(BOOL& enableLut, SvStl::MessageContainerVector* pErrorMessages);
	bool getOperator(long& Operator, SvStl::MessageContainerVector* pErrorMessages);
	bool getGainAndOffset(double& gain, double& offset, SvStl::MessageContainerVector* pErrorMessages);
	void ScaleWithAveraging(SvOi::ITRCImagePtr pInputImageBuffer, const RECT& rInputRect, SvOi::ITRCImagePtr pOutputImageBuffer, const RECT& rOutputRect);
	
	HRESULT CollectInputImageNames( );

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	SvOl::InputObject	m_InputImageA;
	SvOl::InputObject	m_InputImageB;
	SvOl::InputObject	m_InputArithmaticOperator;
	
	SvOl::InputObject	m_InputGain;
	SvOl::InputObject	m_InputUseLut;
	SvOl::InputObject	m_InputOffset;
	SvOl::InputObject	m_InputIsGainOffsetEnabled;

	// Embedded Object:
	SvIe::SVImageClass	m_OutputImage;
	
	SVMatroxBuffer m_milFloatImageBuffer;
	std::unique_ptr<LookUp> m_pLookUp {nullptr};
};

} //namespace SvOp
