// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
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
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVFillBlobDlg : public CDialog
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	public:
		SVFillBlobDlg(Controller& rValues, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVFillBlobDlg();
		HRESULT SetInspectionData();

		// Dialog Data
			//{{AFX_DATA(SVFillBlobDlg)
		enum { IDD = IDD_DLG_FILL_BLOBS };
		SvMc::SVEnumerateComboClass	m_ctlBlobFillType;
		SvMc::SVEnumerateComboClass	m_ctlBlobFillColor;
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
		Controller& m_rValues;
	};
} //namespace SvOg