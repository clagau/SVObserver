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
#include "SVOCore/SVTaskObject.h"
#pragma region Includes

class SVLinearEdgeProcessingClass : public SVTaskObjectClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeProcessingClass );

public:
	SVLinearEdgeProcessingClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPROCESSING );
	virtual ~SVLinearEdgeProcessingClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct *PCreateStructure);

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();
	virtual BOOL onRun( SVRunStatusClass &p_rsvRunStatus );

	SVImageClass*             GetInputImage();
	SVDoubleValueObjectClass* GetInputMinThreshold();
	SVDoubleValueObjectClass* GetInputMaxThreshold();
	SVDoubleValueObjectClass* GetInputLinearData();

	HRESULT GetPixelDepth();

	HRESULT GetPointFromDistance( double p_dDistance, SVExtentPointStruct &p_rsvPoint );
	HRESULT GetEdgeOverlayFromDistance( double p_dDistance, SVExtentLineStruct &p_rsvLine );

	HRESULT GetOutputEdgeDistance( double &p_rdValue );
	HRESULT GetOutputEdgePoint( SVExtentPointStruct &p_rsvPoint );

	HRESULT GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetHistogramOverlay( SVExtentLineStruct &p_rsvLine );
	HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	HRESULT GetSelectedEdgeOverlay( SVExtentLineStruct &p_rsvLine );
	
	//************************************
	// Method:    getLowerThresholdValue
	// Description: Get the value of the lower threshold. If there is are error, it return -1. 
	// Returns:   long
	//************************************
	long getLowerThresholdValue() const;

	//************************************
	// Method:    geUpperThresholdValue
	// Description: Get the value of the upper threshold. If there is are error, it return -1. 
	// Returns:   long
	//************************************
	long getUpperThresholdValue() const;

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;

	SVEnumerateValueObjectClass m_svDirection;
	SVEnumerateValueObjectClass m_svPolarisation;
	SVEnumerateValueObjectClass m_svEdgeSelect;
	SVDoubleValueObjectClass m_svEdgeSelectThisValue;

	SVBoolValueObjectClass m_svIsFixedEdgeMarker;
	SVEnumerateValueObjectClass m_svPosition;
	SVDoubleValueObjectClass m_svPositionOffsetValue;

	SVBoolValueObjectClass	m_svUseLowerThresholdSelectable;
	SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass	m_svUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass	m_svUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass m_svLowerThresholdValue;
	SVDWordValueObjectClass m_svLowerMaxMinusPercentDiffValue;
	SVDWordValueObjectClass m_svLowerMaxMinusOffsetValue;
	SVDWordValueObjectClass m_svLowerMinPlusOffsetValue;

	SVBoolValueObjectClass	m_svUseUpperThresholdSelectable;
	SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass	m_svUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass	m_svUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass m_svUpperThresholdValue;
	SVDWordValueObjectClass m_svUpperMaxMinusPercentDiffValue;
	SVDWordValueObjectClass m_svUpperMaxMinusOffsetValue;
	SVDWordValueObjectClass m_svUpperMinPlusOffsetValue;

	SVDoubleValueObjectClass m_svLinearEdges;

	DWORD m_dwColorNumber;

protected:
	HRESULT UpdateUpperThresholdValues( long p_lIndex );
	HRESULT UpdateLowerThresholdValues( long p_lIndex );
	HRESULT UpdateEdgeList( long p_lIndex );
	HRESULT IsEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation );
	HRESULT GetBlackWhiteEdgeValue( double p_dCurrent, DWORD p_dwUpper, DWORD p_dwLower, double &l_rdValue );
	HRESULT CalculateSubPixelEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation, double &p_rdDistance );

	SVInObjectInfoStruct m_svInputImageObjectInfo;
	SVInObjectInfoStruct m_svInputMinThreshold;
	SVInObjectInfoStruct m_svInputMaxThreshold;
	SVInObjectInfoStruct m_svInputLinearData;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;

	COLORREF m_cfThresholds;
	COLORREF m_cfHistogram;
	COLORREF m_cfEdges;
};

