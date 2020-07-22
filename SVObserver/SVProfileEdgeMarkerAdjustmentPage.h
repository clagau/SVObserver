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
#include "SVOResource/resource.h"
#include "SVOGui/SVEdgeMarkerAdjustmentPageClass.h"
#include "SVOGui/PictureDisplay.h"
#pragma endregion Includes

#pragma region Declarations
//@TODO[gra][8.00][25.01.2018]: This class should move to SVOGui
namespace SvAo
{
class SVAnalyzerClass;
}
namespace SvOp
{
class SVLinearEdgeProcessingClass;
}
namespace SvTo
{
class SVToolClass;
}
#pragma endregion Declarations

class SVProfileEdgeMarkerAdjustmentPageClass : public SvOg::SVEdgeMarkerAdjustmentPageClass
{
#pragma region Constructor
public:
	SVProfileEdgeMarkerAdjustmentPageClass(uint32_t inspectionID, uint32_t taskObjectID, const std::vector<SvPb::EmbeddedIdEnum>& rEdgeEmbeddedIds, uint32_t analyzerID, UINT nIDCaption = 0 );
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
	virtual HRESULT UpdateSliderData(DWORD Lower, DWORD Upper) override;
#pragma region Protected Methods

#pragma region Private Methods
private:
	void updateControls();
	void updateGraphDisplay();

	/// Set the histogram and the edge lines as overlay to the display control. 
	/// \param bVertical [in] If true it spin the graph to vertical.
	void setGraphOverlayToPicture(bool bVertical);

	/// Set a marker overlay to the display control.
	/// \param value [in] The position of the marker.
	/// \param bVertical [in] If true it spin the graph to vertical.
	/// \param allowType [in] Set the allowType to the control. Default is none change allowed.
	void setMarkerOverlayToPicture( DWORD value, bool bVertical, long allowType = CDSVPictureDisplay::AllowNone );
#pragma endregion Private Methods

#pragma region Member variables
private:
	//const values for the graph display of the edge lines
	static const long m_egdeLinesGraphMaxY = 20;
	static const long m_egdeLinesStartPos = 8;
	static const long m_egdeLinesStopPos = 12;

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

	//@TODO[gra][8.00][25.01.2018]: This should be changed when access is possible through Inspection commands
	SvTo::SVToolClass* m_pTool{ nullptr };
	SvAo::SVAnalyzerClass* m_pAnalyzer {nullptr};
	SvOp::SVLinearEdgeProcessingClass* m_pEdge{ nullptr };
	std::shared_ptr<SvOg::ValueController> m_pAnalyzerValues;
#pragma endregion Member variables
};
