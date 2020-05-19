//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilterDlg
//* .File Name       : $Workfile:   SVCustomFilterDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:04:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVCustomFilterDlg : public CDialog 
	{
		// Construction
	public:
		SVCustomFilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVCustomFilterDlg();

	protected:

		HRESULT SetInspectionData();
		void EnableCells();

		// Dialog Data
		//{{AFX_DATA(SVCustomFilterDlg)
	public:
		enum { IDD = IDD_CUSTOM_FILTER };
		CComboBox	m_ctlKernelWidth;
		CComboBox	m_ctlKernelHeight;
		BOOL	m_bAbsoluteValue;
		BOOL	m_bClippingOn;
		long	m_lTransformationFactor;
		long	m_lKernelWidth;
		long	m_lKernelHeight;
		//}}AFX_DATA

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVCustomFilterDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SVCustomFilterDlg)
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		virtual void OnCancel() override;
		afx_msg void OnSelchangeKernelWidth();
		afx_msg void OnSelchangeKernelHeight();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:	
		long	m_lKernelCells[49];
		const uint32_t m_InspectionID;
		const uint32_t m_filterID;
		ValueController m_values;
	};
} //namespace SvOg

