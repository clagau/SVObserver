//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVEdgeMarkerAdjustmentPageClass.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes


#pragma region Declarations
#pragma endregion Declarations

namespace SvOg
{
class SVProfileEdgeMarkerAdjustmentPage : public SVEdgeMarkerAdjustmentPageClass
{
#pragma region Constructor
public:
	SVProfileEdgeMarkerAdjustmentPage(uint32_t inspectionID, uint32_t egdeObjectId, const std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>& rEdgeEmbeddedIds, uint32_t analyzerID, UINT nIDCaption = 0);
	virtual ~SVProfileEdgeMarkerAdjustmentPage();
#pragma endregion Constructor

#pragma region MFC Methods
	//{{AFX_VIRTUAL(SVProfileEdgeMarkerAdjustmentPage)
public:
	virtual void OnCancel() override;
	virtual BOOL OnSetActive() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVProfileEdgeMarkerAdjustmentPage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUseFlagChanged();
	afx_msg void OnChangeUpperSelected();
	afx_msg void OnChangeUpperPercentDiff();
	afx_msg void OnChangeUpperMaxOffset();
	afx_msg void OnChangeUpperMinOffset();
	afx_msg void OnChangeLowerSelected();
	afx_msg void OnChangeLowerPercentDiff();
	afx_msg void OnChangeLowerMaxOffset();
	afx_msg void OnChangeLowerMinOffset();
	afx_msg void OnUpperSelectedButton();
	afx_msg void OnUpperPercentDiffButton();
	afx_msg void OnUpperMaxOffsetButton();
	afx_msg void OnUpperMinOffsetButton();
	afx_msg void OnLowerSelectedButton();
	afx_msg void OnLowerPercentDiffButton();
	afx_msg void OnLowerMaxOffsetButton();
	afx_msg void OnLowerMinOffsetButton();
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
	SvPb::EmbeddedIdEnum getValueEIdFromThresholdOption(bool isUpper) const;
	virtual HRESULT UpdateSliderData() override;
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
	void setMarkerOverlayToPicture(DWORD value, bool bVertical, long allowType = CDSVPictureDisplay::AllowNone);

	void OnChangeValue(std::unique_ptr<SvOgu::LinkedValueWidgetHelper>& rWidget);
#pragma endregion Private Methods

#pragma region Member variables
private:
	//const values for the graph display of the edge lines
	static const long m_egdeLinesGraphMaxY = 20;
	static const long m_egdeLinesStartPos = 8;
	static const long m_egdeLinesStopPos = 12;

	enum { IDD = IDD_PROFILE_GR_EDGE_MARKER_ADJUST_DIALOG };
	CEdit	m_LowerSelectedEditCtrl;
	CButton m_LowerSelectedButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LowerSelectedWidget;
	CEdit	m_LowerMinOffsetEditCtrl;
	CButton m_LowerMinOffsetButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LowerMinOffsetWidget;
	CEdit	m_LowerMaxOffsetEditCtrl;
	CButton m_LowerMaxOffsetButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LowerMaxOffsetWidget;
	CEdit	m_LowerMaxDiffEditCtrl;
	CButton m_LowerMaxDiffButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LowerMaxDiffWidget;
	CEdit	m_UpperSelectedEditCtrl;
	CButton m_UpperSelectedButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_UpperSelectedWidget;
	CEdit	m_UpperMinOffsetEditCtrl;
	CButton m_UpperMinOffsetButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_UpperMinOffsetWidget;
	CEdit	m_UpperMaxOffsetEditCtrl;
	CButton m_UpperMaxOffsetButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_UpperMaxOffsetWidget;
	CEdit	m_UpperMaxDiffEditCtrl;
	CButton m_UpperMaxDiffButton;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_UpperMaxDiffWidget;
	int		m_lowerThresholdOption = 0;
	int		m_upperThresholdOption = 0;

	SvOgu::PictureDisplay m_dialogImage;
	SvOgu::ValueController m_analyzerValues;
#pragma endregion Member variables
};
}  //namespace SvOg