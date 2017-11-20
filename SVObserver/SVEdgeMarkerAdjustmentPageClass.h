//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEdgeMarkerAdjustmentPageClass
//* .File Name       : $Workfile:   SVEdgeMarkerAdjustmentPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Jul 2014 13:08:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls\SVEditNumbers.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVOCore/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVIPDoc;
class SVAnalyzerClass;
class SVTaskObjectClass;
class SVBoolValueObjectClass;
class SVDWordValueObjectClass;
class SVEnumerateValueObjectClass;
class SVDoubleValueObjectClass;

class SVEdgeMarkerAdjustmentPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
	DECLARE_DYNCREATE(SVEdgeMarkerAdjustmentPageClass)

public:
	SVEdgeMarkerAdjustmentPageClass( UINT nIDCaption = 0, int id = IDD );
	virtual ~SVEdgeMarkerAdjustmentPageClass();

	virtual HRESULT GetInspectionData();
	virtual HRESULT SetInspectionData();

	void SetNormalizer( const SVValueBaseNormalizerClass &l_rsvNormalizer );

	SVAnalyzerClass* m_pCurrentAnalyzer;

	//edge value objects
	SVBoolValueObjectClass* m_pvoUseLowerThresholdSelectable;
	SVBoolValueObjectClass* m_pvoUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass* m_pvoUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass* m_pvoUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass* m_pvoLowerThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass* m_pvoLowerThresholdMaxMinusOffset;
	SVDWordValueObjectClass* m_pvoLowerThresholdMinPlusOffset;
	SVBoolValueObjectClass* m_pvoUseUpperThresholdSelectable;
	SVBoolValueObjectClass* m_pvoUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass* m_pvoUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass* m_pvoUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass* m_pvoUpperThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass* m_pvoUpperThresholdMaxMinusOffset;
	SVDWordValueObjectClass* m_pvoUpperThresholdMinPlusOffset;

	SVEnumerateValueObjectClass* m_pvoEdgeDirection;
	SVEnumerateValueObjectClass* m_pvoEdgePolarisation;
	SVEnumerateValueObjectClass* m_pvoEdgeSelect;
	SVDoubleValueObjectClass* m_pvoEdgeSelectThisValue;
	SVBoolValueObjectClass* m_pvoEdgeIsFixedEdgeMarker;
	SVEnumerateValueObjectClass* m_pvoEdgePosition;
	SVDoubleValueObjectClass* m_pvoEdgePositionOffsetValue;
	SVDWordValueObjectClass* m_pvoEdgeLowerThresholdValue;
	SVDWordValueObjectClass* m_pvoEdgeUpperThresholdValue;
	
	//flags
	bool m_bEnableDirection;
	bool m_bEnableEdgeSelect;
	bool m_bEnablePolarisation;
	bool m_bEnablePosition;
	bool m_bEnableThreshold;
	bool m_bEdgeA;

// Dialogfelddaten
	//{{AFX_DATA(SVEdgeMarkerAdjustmentPageClass)
	enum { IDD = IDD_GR_EDGE_MARKER_ADJUST_DIALOG };
	SvMc::CEditNumbersFloat	m_editPositionOffset;
	SvMc::CEditNumbersFloat	m_editSelectThis;
	CSliderCtrl	UpperSliderCtrl;
	CSliderCtrl	LowerSliderCtrl;
	CString	StrLower;
	CString	StrUpper;
	CString	StrPositionOffset;
	CString	StrEdgeSelectThis;
	//}}AFX_DATA

protected:
	//************************************
	// Method:    UpdateSliderData
	// Description: Sets the scroll position for the slider controls and updates the public members StrLower and StrUpper.
	// Parameter: p_dLower - New position for the lower slider control
	// Parameter: p_dUpper - New position for the upper slider control
	// Returns:   S_OK every time
	//************************************
	// @WARNING:  This is a bad design.  Public member variables should not be used.  Instead use accessor functions.
	virtual HRESULT UpdateSliderData( double p_dLower, double p_dUpper );

	void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
	void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );

	// Normalizer for Lower threshold
	SVValueBaseNormalizerClass LowerNormalizer;
	SVValueBaseNormalizerClass Normalizer;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVEdgeMarkerAdjustmentPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDirectionHeadToTailRadio();
	afx_msg void OnDirectionTailToHeadRadio();
	afx_msg void OnEdgeSelectFirstRadio();
	afx_msg void OnEdgeSelectLastRadio();
	afx_msg void OnEdgeSelectThisRadio();
	afx_msg void OnFixedEdgeMarkerRadio();
	afx_msg void OnPolarisationAnyRadio();
	afx_msg void OnPolarisationNegativeRadio();
	afx_msg void OnPolarisationPositiveRadio();
	afx_msg void OnPositionCenterRadio();
	afx_msg void OnPositionEndRadio();
	afx_msg void OnPositionOffsetRadio();
	afx_msg void OnPositionStartRadio();
	afx_msg void OnSearchedEdgeMarkerRadio();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEdgeSelectThisEdit();
	afx_msg void OnChangePositionOffsetEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVEdgeMarkerAdjustmentPageClass)
	public:
	virtual void OnCancel() override;
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

};

