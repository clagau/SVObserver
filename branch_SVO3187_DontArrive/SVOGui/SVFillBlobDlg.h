// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.h
// * .File Name       : $Workfile:   SVFillBlobDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 07:05:36  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class ValueController;
	class SVFillBlobDlg : public CDialog
	{
	public:
		explicit SVFillBlobDlg(ValueController& rValues, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVFillBlobDlg();
		HRESULT SetInspectionData();

		// Dialog Data
			//{{AFX_DATA(SVFillBlobDlg)
		enum { IDD = IDD_DLG_FILL_BLOBS };
		SvMc::SVEnumerateCombo	m_ctlBlobFillType;
		SvMc::SVEnumerateCombo	m_ctlBlobFillColor;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVFillBlobDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(SVFillBlobDlg)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeComboBlobColor();
		afx_msg void OnSelchangeComboBlobType();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		void FillCombos();

		//This is a reference to the controller from the parent dialog which has already been initialized
		ValueController& m_rValues;
	};
} //namespace SvOg
