//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Nov 2014 05:09:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVBlobAnalyzerDialog : public CDialog
	{
	public:
	protected:
	// Generated message map functions
		//{{AFX_MSG(SVBlobAnalyzerDialog)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnButtonAdd();
		afx_msg void OnButtonRemove();
		afx_msg void OnButtonSetRange();
		virtual void OnOK() override;
		virtual void OnCancel() override;
		afx_msg void OnSetSortBtn();
		afx_msg void OnAscending();
		afx_msg void OnDestroy();
		afx_msg void OnSelchangeList2();
		afx_msg void OnNbrOfBlobs();
		afx_msg void OnExcludeBlob();
		afx_msg void OnBtnFillBlobs();
		afx_msg void OnFillBlobs();
		afx_msg void OnBlobColor();
		afx_msg void OnChangeEditMaxNbrBlobs();
		afx_msg void OnKillFocusMaxNbrBlobs();
		afx_msg void OnButtonSetFeatureProperties();
		afx_msg void OnChangeEditMaxBlobDataArraySize(); 
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	public:
		SVBlobAnalyzerDialog(uint32_t inspectionID, uint32_t taskObjectID, CWnd* pParent = nullptr);
		virtual ~SVBlobAnalyzerDialog();

	private:
		HRESULT SetInspectionData();
		void EnableButtons();

	private:
		void initFeatureListBox(CListBox& listBox, bool EnabledFeatures);
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung

		enum { IDD = IDD_BA_FEATURE_DIALOG };
		CEdit	m_ebMaxBlobDataArraySize;
		CEdit	m_ctlMaxNbrBlobs;
		CButton	m_chkFillBlob;
		CButton	m_btnFillBlobs;
		SvMc::SVEnumerateCombo m_cbBlobColor;
		CListBox m_lbSelectedFeatures;
		CListBox m_lbAvailableFeatures;
		CString m_SortFeatureEdt;
		BOOL	m_Ascending{false};
		BOOL	m_bExclude{false};
		long	m_lMaxNumberBlobs{100};
		long	m_lMaxBlobDataArraySize{100};
	
		std::string m_FeaturesEnabled;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		ValueController m_values;
	};
} //namespace SvOg
