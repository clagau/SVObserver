//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileEdgeMarkerAdjustmentPage
//* .File Name       : $Workfile:   SVProfileEdgeMarkerAdjustmentPage.h  $
//* .Description	 : Display the average graph and the edges to the GUI and let the user to change the parameter
//*					   ATTENTION: This class is used now for the linear tool, the name should be changed, but it is done yet because it is a mess with PVCS.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   10 Jul 2014 17:41:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEdgeMarkerAdjustmentPageClass.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#pragma endregion Includes

#pragma region Declarations
class SVLinearEdgeProcessingClass;
#pragma endregion Declarations

class SVProfileEdgeMarkerAdjustmentPageClass : public SVEdgeMarkerAdjustmentPageClass
{
	DECLARE_DYNCREATE(SVProfileEdgeMarkerAdjustmentPageClass)
#pragma region Constructor
public:
	SVProfileEdgeMarkerAdjustmentPageClass( UINT nIDCaption = 0 );
	virtual ~SVProfileEdgeMarkerAdjustmentPageClass();
#pragma endregion Constructor

#pragma region MFC Methods
	//{{AFX_VIRTUAL(SVProfileEdgeMarkerAdjustmentPageClass)
public:
	virtual void OnCancel() override;
	virtual BOOL OnSetActive() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVProfileEdgeMarkerAdjustmentPageClass)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpperUserSelectable();
	afx_msg void OnLowerUserSelectable();
	afx_msg void OnUpperUseMaxMinusDiff();
	afx_msg void OnUpperUseMaxMinusOffset();
	afx_msg void OnUpperUseMinPlusOffset();
	afx_msg void OnLowerUseMaxMinusDiff();
	afx_msg void OnLowerUseMaxMinusOffset();
	afx_msg void OnLowerUseMinPlusOffset();
	afx_msg void OnChangeUpperPercentDiff();
	afx_msg void OnChangeUpperMaxOffset();
	afx_msg void OnChangeUpperMinOffset();
	afx_msg void OnChangeLowerPercentDiff();
	afx_msg void OnChangeLowerMaxOffset();
	afx_msg void OnChangeLowerMinOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

#pragma region Public Methods
public:
	virtual HRESULT GetInspectionData() override;
	virtual HRESULT SetInspectionData() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual HRESULT UpdateSliderData( double p_dLower, double p_dUpper ) override;
#pragma region Protected Methods

#pragma region Private Methods
private:
	void updateControls();
	void updateGraphDisplay();

	//************************************
	// Method:    setGraphOverlayToPicture
	// Description: Set the histogram and the edge lines as overlay to the display control. 
	// Parameter: SVLinearEdgeProcessingClass* pEdge The linear edge class from where to get the graph points.
	// Returns:   void
	//************************************
	void setGraphOverlayToPicture( SVLinearEdgeProcessingClass* pEdge );

	//************************************
	// Method:    setMarkerOverlayToPicture
	// Description: Set a marker overlay to the display control.
	// Parameter: unsigned long value The position of the marker.
	// Parameter: long allowType Set the allowType to the control. Default is none change allowed.
	// Returns:   void
	//************************************
	void setMarkerOverlayToPicture( unsigned long value, long allowType = CDSVPictureDisplay::AllowNone );
#pragma endregion Private Methods

#pragma region Member variables
private:
	//const values for the graph display of the edge lines
	static const long m_egdeLinesGraphMaxY = 20;
	static const long m_egdeLinesStartPos = 8;
	static const long m_egdeLinesStopPos = 12;

	//{{AFX_DATA(SVProfileEdgeMarkerAdjustmentPageClass)
	enum { IDD = IDD_PROFILE_GR_EDGE_MARKER_ADJUST_DIALOG };
	CEdit	m_LowerMinOffsetEditCtrl;
	CEdit	m_LowerMaxOffsetEditCtrl;
	CEdit	m_LowerMaxDiffEditCtrl;
	CEdit	m_UpperMinOffsetEditCtrl;
	CEdit	m_UpperMaxOffsetEditCtrl;
	CEdit	m_UpperMaxDiffEditCtrl;
	BYTE	m_lowerThresholdMaxOffset;
	BYTE	m_lowerThresholdMinOffset;
	int		m_lowerThresholdMaxPercentDiff;
	BYTE	m_upperThresholdMaxOffset;
	BYTE	m_upperThresholdMinOffset;
	int 	m_upperThresholdMaxPercentDiff;
	int		m_lowerThresholdOption;
	int		m_upperThresholdOption;
	SvOg::PictureDisplay m_dialogImage;
	//}}AFX_DATA
#pragma endregion Member variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio 

