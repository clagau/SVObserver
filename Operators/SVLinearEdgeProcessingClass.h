// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/LinearEdgeEnums.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma region Includes

namespace SvOp
{

class SVLinearEdgeProcessingClass : public SvIe::SVTaskObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	SVLinearEdgeProcessingClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPROCESSING );
	virtual ~SVLinearEdgeProcessingClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool onRun( SvIe::RunStatus &p_rsvRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

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
	
	std::vector<double> getLinearEdges() const { std::vector<double> result; m_svLinearEdges.GetArrayValues(result); return result; };
	void setDefaultValues(SvDef::SV_EDGECONTROL_POLARISATION_ENUM polarisationValue, SvDef::SV_EDGECONTROL_EDGESELECT_ENUM edgeSelectValue, double edgeSelectThisValue);
	void setEdgeSelectDefaultValue(SvDef::SV_EDGECONTROL_EDGESELECT_ENUM edgeSelectValue) { m_svEdgeSelect.SetDefaultValue(edgeSelectValue, true); };
	void setDirectionDefaultValue(SvDef::SV_EDGECONTROL_DIRECTION_ENUM directionValue) { m_svDirection.SetDefaultValue(directionValue, true); };
	DWORD getColorNumber() const { return m_dwColorNumber; };

protected:
	enum class UpdateEnum
	{
		None,
		Selected,
		PercentDiff,
		MaxOffset,
		MinOffset
	};
	bool UpdateThresholdValues(UpdateEnum updateState, const SvVol::LinkedValue& rThresholdSelected, const SvVol::LinkedValue& rThresholdPercentDiff,
		const SvVol::LinkedValue& rThresholdMinOffset, const SvVol::LinkedValue& rThresholdMaxOffset, byte& rThresholdValue, SvVol::SVDWordValueObjectClass& rThresholdValueObject);
	HRESULT UpdateEdgeList();
	HRESULT IsEdge(double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation);
	HRESULT GetBlackWhiteEdgeValue(double p_dCurrent, DWORD p_dwUpper, DWORD p_dwLower, double& l_rdValue);
	HRESULT CalculateSubPixelEdge(double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation, double& p_rdDistance);


	UpdateEnum getThresholdMode(const SvVol::SVBoolValueObjectClass& rUseSelected, const SvVol::SVBoolValueObjectClass& rUsePercentDiff, const SvVol::SVBoolValueObjectClass& rUseMinOffset, const SvVol::SVBoolValueObjectClass& rUseMaxOffset);

protected:
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
	SvVol::SVDWordValueObjectClass m_svLowerThresholdValueObject;
	byte m_lowerThreshold = 0;
	UpdateEnum m_updateLowerState = UpdateEnum::Selected;
	bool m_needUpdateThresholdValueLower = true;
	SvVol::LinkedValue m_svLowerThresholdSelected;
	SvVol::LinkedValue m_svLowerMaxMinusPercentDiffValue;
	SvVol::LinkedValue m_svLowerMaxMinusOffsetValue;
	SvVol::LinkedValue m_svLowerMinPlusOffsetValue;

	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdSelectable;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusPercentDiff;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusOffset;
	SvVol::SVBoolValueObjectClass	m_svUseUpperThresholdMinPlusOffset;
	SvVol::SVDWordValueObjectClass m_svUpperThresholdValueObject;
	byte m_upperThreshold = 0;
	UpdateEnum m_updateUpperState = UpdateEnum::Selected;
	bool m_needUpdateThresholdValueUpper = true;
	SvVol::LinkedValue m_svUpperThresholdSelected;
	SvVol::LinkedValue m_svUpperMaxMinusPercentDiffValue;
	SvVol::LinkedValue m_svUpperMaxMinusOffsetValue;
	SvVol::LinkedValue m_svUpperMinPlusOffsetValue;

	SvVol::SVDoubleValueObjectClass m_svLinearEdges;

	DWORD m_dwColorNumber;


	SvOl::InputObject m_InputImage;
	SvOl::InputObject m_InputMinThreshold;
	SvOl::InputObject m_InputMaxThreshold;
	SvOl::InputObject m_InputDelta;
	SvOl::InputObject m_InputLinearData;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;

	COLORREF m_cfThresholds;
	COLORREF m_cfHistogram;
	COLORREF m_cfEdges;
};

} //namespace SvOp
