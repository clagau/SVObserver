//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTwoImagePage
//* .File Name       : $Workfile:   TADlgTwoImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
class TADlgTwoImagePageClass : public CPropertyPage, protected SvOgu::ImageController
{
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
public:
	TADlgTwoImagePageClass(uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~TADlgTwoImagePageClass();
#pragma endregion Constructor

#pragma region Protected Methods
	//******************************************************************************
	// Class Wizard Generated Virtual Function(s):
	//******************************************************************************
		//{{AFX_VIRTUAL(TADlgTwoImagePageClass)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	//}}AFX_VIRTUAL
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
//******************************************************************************
// Class Wizard Generated Message Map Entries:
//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(TADlgTwoImagePageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSelChangeCombo2();
	afx_msg void OnSelchangeOperatorCombo();
	afx_msg void OnCheckEnableGain();
	afx_msg void OnCheckRecalculateOnce();
	afx_msg void OnBnClickedButtonGain();
	afx_msg void OnKillFocusGain();
	afx_msg void OnBnClickedButtonOffset();
	afx_msg void OnKillFocusOffset();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void ShowGainAndOffset(long operation); //! show or Hide controls for Gain and Offset op is selected operation

	HRESULT SetInspectionData();//!Get the data from dialig and set via ValueController
	void refresh();
	void setImages();
	void RetreiveCurrentlySelectedImageNames();
	void RetreiveResultImageNames();
#pragma endregion Private Methods

#pragma region Member variables
protected:
	//{{AFX_DATA(TADlgTwoImagePageClass)
	enum { IDD = IDD_TA_TWO_IMAGE_DIALOG };
	CComboBox m_operatorCtrl;
	SvOgu::PictureDisplay m_secondImageCtrl;
	SvOgu::PictureDisplay m_firstImageCtrl;
	SvMc::AvailableObjectListComboBox<uint32_t> m_firstAvailableSourceImageListBoxCtl;
	SvMc::AvailableObjectListComboBox<uint32_t> m_secondAvailableSourceImageListBoxCtl;
	//}}AFX_DATA

private:
	BOOL m_IsGainEnabled {FALSE};
	BOOL m_RecalculateOnce {FALSE};

	CButton m_ButtonGainLink;
	CEdit m_EditGainValue;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_GainWidget;
	CButton m_ButtonOffsetLink;
	CEdit m_EditOffsetValue;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_OffsetWidget;

	bool m_IgnoreNotification {false};

	std::string m_firstImageName;
	std::string m_secondImageName;
	std::string m_resultImageName;
	uint32_t m_resultImageID;

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;

	SvOgu::ValueController m_values;
#pragma endregion Member variables
};
} //namespace SvOg
