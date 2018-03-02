//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   27 Jan 2014 15:50:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVMFCControls/SVDlgGraph.h"
#include "SVMFCControls/CLabel.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOg
{
	class SVToolAdjustmentLUTPage : public CPropertyPage
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	#pragma region Constructor
	public:
		SVToolAdjustmentLUTPage(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rLutEquationID);
		virtual ~SVToolAdjustmentLUTPage();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		HRESULT SetInspectionData();
		Controller& GetValues() { return m_Values; }
	#pragma endregion Public Methods

	#pragma region Protected Methods
	protected:

	#pragma region virtual
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVToolAdjustmentDialogLUTPageClass)
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL
	#pragma endregion virtual
	
		// Generated message map functions
		//{{AFX_MSG(SVToolAdjustmentDialogLUTPageClass)
		afx_msg void OnActivateCheck();
		afx_msg void OnSelChangeLutModeCombo();
		afx_msg void OnLUTFormulaButton();
		afx_msg void OnContinuousRecalcCheck();
		afx_msg void OnClipModeCheck();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnLowerEditLostFocus();
		afx_msg void OnUpperEditLostFocus();
		//}}AFX_MSG
		afx_msg LRESULT OnGraphRefresh( WPARAM mp1, LPARAM mp2 );
		DECLARE_MESSAGE_MAP()

		//************************************
		// Method:    refresh
		// Description: Refresh the GUI.
		// Access:    protected
		// Returns:   void
		// Parameter: bool bSave = true: If true set data from GUI to inspection.
		//************************************
		void refresh( bool bSave = true );

		//************************************
		// Method:    refreshLUTGraph
		// Description: Refresh the LUT graph.
		// Access:    protected
		// Returns:   void
		//************************************
		void refreshLUTGraph();

		//************************************
		// Method:    refreshClip
		// Description: Refresh the clip controls (Lower and Upper).
		// Access:    protected
		// Returns:   void
		//************************************
		void refreshClip();

		//************************************
		// Method:    showFormulaControls
		// Description: Show formula controls and hide the others (e.g. clip controls).
		// Access:    protected
		// Returns:   void
		//************************************
		void showFormulaControls();

		//************************************
		// Method:    showClipControls
		// Description: Show clip controls and hide the others (e.g. formula controls).
		// Access:    protected
		// Returns:   void
		//************************************
		void showClipControls();

		//************************************
		// Method:    hideAllUncommonControls
		// Description: Hide all controls (e.g. formula and clip controls), expect of common controls (useLTU, contRecalc, LTUMode).
		// Access:    protected
		// Returns:   void
		//************************************
		void hideAllUncommonControls();

		//************************************
		// Method:    enableAllControls
		// Description: Enable or disable all controls, expect of the useLTU checkbox.
		// Access:    protected
		// Returns:   void
		// Parameter: BOOL isEnable: Flag if controls enabled (TRUE) or disabled (FALSE).
		//************************************
		void enableAllControls(BOOL isEnable);
	#pragma endregion Protected Methods

	#pragma region Member variables
	//******************************************************************************
	// Data Element(s):
	//******************************************************************************
	private:
		//{{AFX_DATA(SVToolAdjustmentDialogLUTPageClass)
		enum { IDD = IDD_TA_LUT_DIALOG };
		CSliderCtrl m_upperSlider;
		CSliderCtrl m_lowerSlider;
		SvMc::SVDlgGraphClass m_LUTGraph;
		SvMc::SVEnumerateComboClass m_LUTModeCombo;
		CString m_strUpperClipValue;
		CString m_strLowerClipValue;
		CLabel m_yAxisLabel;
		BOOL m_bUseLUT;
		BOOL m_bContinuousRecalcLUT;
		BOOL m_isFormulaClip;
		//}}AFX_DATA

		const GUID& m_rInspectionID;
		const GUID& m_rTaskObjectID;
		Controller m_Values;
		Controller m_LutEquation;
	#pragma endregion Member variables
	};
} //namespace SvOg