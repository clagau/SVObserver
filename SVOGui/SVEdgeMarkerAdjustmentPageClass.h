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
#include "SVOResource/resource.h"
#include "SVMFCControls/SVEditNumbers.h"
#include "SVLibrary/SVValueBaseNormalizerClass.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	//! Note this enum is sensitive to the order in which the GUID ID's are set
	//! They must match the GUID list filled 
	enum EdgeEmbeddedGuids
	{
		EdgeDirectionGuid = 0,
		EdgePolarisationGuid,
		EdgeSelectGuid,
		EdgeSelectThisValueGuid,
		EdgeIsFixedEdgeMarkerGuid,
		EdgePositionGuid,
		EdgePositionOffsetGuid,
		UseLowerThresholdSelectableGuid,
		UseLowerThresholdMaxMinusPercentDiffGuid,
		UseLowerThresholdMaxMinusOffsetGuid,
		UseLowerThresholdMinPlusOffsetGuid,
		EdgeLowerThresholdValueGuid,
		LowerThresholdMaxMinusPercentDiffGuid,
		LowerThresholdMaxMinusOffsetGuid,
		LowerThresholdMinPlusOffsetGuid,
		UseUpperThresholdSelectableGuid,
		UseUpperThresholdMaxMinusPercentDiffGuid,
		UseUpperThresholdMaxMinusOffsetGuid,
		UseUpperThresholdMinPlusOffsetGuid,
		EdgeUpperThresholdValueGuid,
		UpperThresholdMaxMinusPercentDiffGuid,
		UpperThresholdMaxMinusOffsetGuid,
		UpperThresholdMinPlusOffsetGuid,
		LinearEdgesClassGuid,

		EdgeGuidCount
	};

	class SVEdgeMarkerAdjustmentPageClass : public CPropertyPage
	{
	public:
		SVEdgeMarkerAdjustmentPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGuidVector& rEdgeEmbeddedGuids, UINT nIDCaption = 0, int ID = IDD);
		virtual ~SVEdgeMarkerAdjustmentPageClass();

		virtual HRESULT GetInspectionData();
		virtual HRESULT SetInspectionData();

		void SetNormalizer( const SVValueBaseNormalizerClass &l_rsvNormalizer );

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
		// Parameter: Lower - New position for the lower slider control
		// Parameter: Upper - New position for the upper slider control
		// Returns:   S_OK every time
		//************************************
		// @WARNING:  This is a bad design.  Public member variables should not be used.  Instead use accessor functions.
		virtual HRESULT UpdateSliderData(DWORD Lower, DWORD Upper);

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

		//! This list needs to be initialized by the corresponding Linear Edge
		//! either EdgeA or EdgeB and the size and order must correspond to the enum EdgeEmbeddedGuids list
		const SVGuidVector& m_rEdgeEmbeddedGuids;

		const SVGUID& m_rInspectionID;
		const SVGUID& m_rTaskObjectID;
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller m_Values;
	};
} //namespace SvOg
