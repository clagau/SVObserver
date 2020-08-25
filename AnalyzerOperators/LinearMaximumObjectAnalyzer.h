//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LinearMaximumObjectAnalyzer
//* .File Name       : $Workfile:   LinearMaximumObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:58:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVDPointValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class LinearMaximumObjectAnalyzer : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( LinearMaximumObjectAnalyzer );

public:
	LinearMaximumObjectAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_LinearMaximumObjectAnalyzer );
	virtual ~LinearMaximumObjectAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) override;
	virtual void addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical = false) const override;

protected:
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvVol::SVDPointValueObjectClass mdpEdgeA;	// Point
	SvVol::SVDPointValueObjectClass mdpEdgeB;	// Point
	SvVol::SVDPointValueObjectClass mdpCenter;	// Point
	SvVol::SVDoubleValueObjectClass mdWidth;	// double

	SvVol::SVDoubleValueObjectClass m_svLinearDistanceA;
	SvVol::SVDoubleValueObjectClass m_svLinearDistanceB;

private:
	void init();
};

} //namespace SvAo
