// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeProcessingClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   10 Jul 2014 17:46:18  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma region Includes

namespace SvOp
{

class SVLinearEdgeProcessingClass : public SvIe::SVTaskObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVLinearEdgeProcessingClass );

public:
	SVLinearEdgeProcessingClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPROCESSING );
	virtual ~SVLinearEdgeProcessingClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool onRun( SVRunStatusClass &p_rsvRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	HRESULT GetInputMinThreshold(double& rMinThreshold);
	HRESULT GetInputMaxThreshold(double& rMaxThreshold);
	HRESULT GetInputLinearVectorData(std::vector<double>& rData);

	HRESULT GetPixelDepth();

	HRESULT GetPointFromDistance( double p_dDistance, SVPoint<double>& rPoint );
	HRESULT GetEdgeOverlayFromDistance( double p_dDistance, SVExtentLineStruct& rLine );

	HRESULT GetOutputEdgeDistance( double& rValue );
	HRESULT GetOutputEdgePoint(SVPoint<double>& rPoint );

	HRESULT GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetHistogramOverlay( SVExtentLineStruct &p_rsvLine );
	HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetSelectedEdgeOverlay( SVExtentLineStruct &p_rsvLine );

	void addOverlayFullResult(SvPb::Overlay& rOverlay, bool isVertical = false) const;
	void addOverlayResultMarker(SvPb::OverlayShapeGroup& rGroup, bool isVertical = false) const;
	enum class ResultType
	{
		EdgeA,
		EdgeB,
		PixelCounting,
	};
	void addOverlayResultDetails(SvPb::Overlay& rOverlay, ResultType resultType, bool isVertical) const;
	/// Add Graph overlay and return true if done.
	bool addGraphOverlay(SvPb::Overlay& rOverlay, bool isVertical = false);
	
	const std::vector<SvPb::EmbeddedIdEnum>& getEdgeEmbeddedIds() { return m_EdgeEmbeddedIds; }

	SvVol::SVEnumerateValueObjectClass m_svDirection;
	SvVol::SVEnumerateValueObjectClass m_svPolarisation;
	SvVol::SVEnumerateValueObjectClass m_svEdgeSelect;
	SvVol::SVDoubleValueObjectClass m_svEdgeSelectThisValue;

	SvVol::SVBoolValueObjectClass m_svIsFixedEdgeMarker;
	SvVol::SVEnumerateValueObjectClass m_svPosition;
	SvVol::SVDoubleValueObjectClass m_svPositionOffsetValue;

	SvVol::SVBoolValueObjectClass	m_svUseLowerThresholdSelectable;
	SvVol::SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusPercentDiff;
	SvVol::SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusOffset;
	SvVol::SVBoolValueObjectClass	m_svUseLowerThresholdMinPlusOffset;
	SvVol::SVDWordValueObjectClass m_svLowerThresholdValue;
	SvVol::SVDWordValueObjectClass m_svLowerMaxMinusPercentDiffValue;
	SvVol::SVDWordValueObjectClass m_svLowerMaxMinusOffsetValue;
	SvVol::SVDWordValueObjectClass m_svLowerMinPlusOffsetValue;

	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdSelectable;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusPercentDiff;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusOffset;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMinPlusOffset;
	SvVol::SVDWordValueObjectClass m_svUpperThresholdValue;
	SvVol::SVDWordValueObjectClass m_svUpperMaxMinusPercentDiffValue;
	SvVol::SVDWordValueObjectClass m_svUpperMaxMinusOffsetValue;
	SvVol::SVDWordValueObjectClass m_svUpperMinPlusOffsetValue;

	SvVol::SVDoubleValueObjectClass m_svLinearEdges;

	DWORD m_dwColorNumber;

protected:
	HRESULT UpdateUpperThresholdValues();
	HRESULT UpdateLowerThresholdValues();
	HRESULT UpdateEdgeList();
	HRESULT IsEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation );
	HRESULT GetBlackWhiteEdgeValue( double p_dCurrent, DWORD p_dwUpper, DWORD p_dwLower, double &l_rdValue );
	HRESULT CalculateSubPixelEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation, double &p_rdDistance );

	SvOl::SVInObjectInfoStruct m_InputImageObjectInfo;
	SvOl::SVInObjectInfoStruct m_InputMinThreshold;
	SvOl::SVInObjectInfoStruct m_InputMaxThreshold;
	SvOl::SVInObjectInfoStruct m_InputDelta;
	SvOl::SVInObjectInfoStruct m_InputLinearData;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;

	COLORREF m_cfThresholds;
	COLORREF m_cfHistogram;
	COLORREF m_cfEdges;

	std::vector<SvPb::EmbeddedIdEnum> m_EdgeEmbeddedIds;
};

} //namespace SvOp
