//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzer
//* .File Name       : $Workfile:   SVOCVAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageAnalyzerClass.h"
#pragma endregion Includes

namespace SvAo
{

class SVOCVAnalyzer : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVOCVAnalyzer );

public:
	SVOCVAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZER	);

	virtual ~SVOCVAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void init();
	bool m_bHasLicenseError;
};

} //namespace SvAo
