//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMFCControls/EditNumbers.h"
#include "DataController.h"
#include "NormalizerController.h"
#pragma endregion Includes

namespace SvOg
{
	//! Note this enum is sensitive to the order in which the ID's are set
	//! They must match the embedded list filled 
	enum EdgeEmbeddedEnum
	{
		EdgeDirection = 0,
		EdgePolarisation,
		EdgeSelect,
		EdgeSelectThisValue,
		EdgeIsFixedEdgeMarker,
		EdgePosition,
		EdgePositionOffset,
		UseLowerThresholdSelectable,
		UseLowerThresholdMaxMinusPercentDiff,
		UseLowerThresholdMaxMinusOffset,
		UseLowerThresholdMinPlusOffset,
		EdgeLowerThresholdValue,
		EdgeLowerThresholdSelected,
		LowerThresholdMaxMinusPercentDiff,
		LowerThresholdMaxMinusOffset,
		LowerThresholdMinPlusOffset,
		UseUpperThresholdSelectable,
		UseUpperThresholdMaxMinusPercentDiff,
		UseUpperThresholdMaxMinusOffset,
		UseUpperThresholdMinPlusOffset,
		EdgeUpperThresholdValue,
		EdgeUpperThresholdSelected,
		UpperThresholdMaxMinusPercentDiff,
		UpperThresholdMaxMinusOffset,
		UpperThresholdMinPlusOffset,
		LinearEdgesClass,

		EdgeIdCount
	};

	constexpr auto g_EdgeAEmbeddedIds = std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>
	{
		SvPb::EdgeADirectionEId, SvPb::EdgeAPolarisationEId, SvPb::EdgeAEdgeSelectEId, SvPb::EdgeAEdgeSelectThisValueEId,
		SvPb::EdgeAIsFixedEdgeMarkerEId, SvPb::EdgeAPositionEId, SvPb::EdgeAPositionOffsetEId, SvPb::UseLowerThresholdSelectableEId,
		SvPb::UseLowerThresholdMaxMinusPercentDiffEId, SvPb::UseLowerThresholdMaxMinusOffsetEId, SvPb::UseLowerThresholdMinPlusOffsetEId,
		SvPb::EdgeALowerThresholdValueEId, SvPb::EdgeALowerThresholdSelectedValueEId, SvPb::LowerThresholdMaxMinusPercentDiffEId, SvPb::LowerThresholdMaxMinusOffsetEId,
		SvPb::LowerThresholdMinPlusOffsetEId, SvPb::UseUpperThresholdSelectableEId, SvPb::UseUpperThresholdMaxMinusPercentDiffEId,
		SvPb::UseUpperThresholdMaxMinusOffsetEId, SvPb::UseUpperThresholdMinPlusOffsetEId, SvPb::EdgeAUpperThresholdValueEId, SvPb::EdgeAUpperThresholdSelectedValueEId,
		SvPb::UpperThresholdMaxMinusPercentDiffEId, SvPb::UpperThresholdMaxMinusOffsetEId, SvPb::UpperThresholdMinPlusOffsetEId,
		SvPb::LinearEdgesEId
	};

	constexpr auto g_EdgeBEmbeddedIds = std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>
	{
		SvPb::EdgeBDirectionEId, SvPb::EdgeBPolarisationEId, SvPb::EdgeBEdgeSelectEId, SvPb::EdgeBEdgeSelectThisValueEId,
		SvPb::EdgeBIsFixedEdgeMarkerEId, SvPb::EdgeBPositionEId, SvPb::EdgeBPositionOffsetEId, SvPb::UseLowerThresholdSelectableBEId,
		SvPb::UseLowerThresholdMaxMinusPercentDiffBEId, SvPb::UseLowerThresholdMaxMinusOffsetBEId, SvPb::UseLowerThresholdMinPlusOffsetBEId,
		SvPb::EdgeBLowerThresholdValueEId, SvPb::EdgeBLowerThresholdSelectedValueEId, SvPb::LowerThresholdMaxMinusPercentDiffBEId, SvPb::LowerThresholdMaxMinusOffsetBEId,
		SvPb::LowerThresholdMinPlusOffsetBEId, SvPb::UseUpperThresholdSelectableBEId, SvPb::UseUpperThresholdMaxMinusPercentDiffBEId,
		SvPb::UseUpperThresholdMaxMinusOffsetBEId, SvPb::UseUpperThresholdMinPlusOffsetBEId, SvPb::EdgeBUpperThresholdValueEId, SvPb::EdgeBUpperThresholdSelectedValueEId,
		SvPb::UpperThresholdMaxMinusPercentDiffBEId, SvPb::UpperThresholdMaxMinusOffsetBEId, SvPb::UpperThresholdMinPlusOffsetBEId,
		SvPb::LinearEdgesEId
	};

	class SVEdgeMarkerAdjustmentPageClass : public CPropertyPage
	{
	public:
		SVEdgeMarkerAdjustmentPageClass(uint32_t inspectionId, uint32_t taskObjectId, const std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>& rEdgeEmbeddedIds, uint32_t analyzerID, UINT nIDCaption = 0, int ID = IDD);
		virtual ~SVEdgeMarkerAdjustmentPageClass();

		virtual HRESULT GetInspectionData();
		virtual HRESULT SetInspectionData();

		//flags
		bool m_bEnableDirection;
		bool m_bEnableEdgeSelect;
		bool m_bEnablePolarisation;
		bool m_bEnablePosition;
		bool m_bEnableThreshold;
		bool m_bEdgeA;
		bool m_existEdgeB = true;

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
		virtual HRESULT UpdateSliderData() = 0;
		HRESULT UpdateSliderData(DWORD Lower, DWORD Upper);

		void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
		void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );

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
		//! either EdgeA or EdgeB and the size and order must correspond to the enum EdgeEmbeddedEnum list
		const std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>& m_rEdgeEmbeddedIds;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		ValueController m_values;
		NormalizerController m_normalizerController;
		NormalizerController m_lowerNormalizerController;
	};
} //namespace SvOg
