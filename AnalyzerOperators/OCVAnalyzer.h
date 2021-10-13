//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : OCVAnalyzer
//* .File Name       : $Workfile:   OCVAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageAnalyzer.h"
#pragma endregion Includes

namespace SvAo
{

class OCVAnalyzer : public ImageAnalyzer
{
	SV_DECLARE_CLASS

public:
	OCVAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZER	);

	virtual ~OCVAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;

protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void init();
	bool m_bHasLicenseError;
};

} //namespace SvAo
