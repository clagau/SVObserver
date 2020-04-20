//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilterDlg
//* .File Name       : $Workfile:   SVRankingFilterDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:39:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVRankingFilterDlg : public CDialog
	{
		// Construction
	public:
		SVRankingFilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVRankingFilterDlg();

		HRESULT SetInspectionData();
		void EnableCells();

		// Dialog Data
		//{{AFX_DATA(SVRankingFilterDlg)
	public:
		enum { IDD = IDD_RANKING_FILTER };
		CComboBox	m_ctlRankingWidth;
		CComboBox	m_ctlRankingHeight;
		CComboBox	m_ctlRankingRank;
		long	m_lRankingWidth { 3 };
		long	m_lRankingHeight{ 3 };
		long	m_lRankingRank{ 0 };
		//}}AFX_DATA

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVRankingFilterDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SVRankingFilterDlg)
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		afx_msg void OnSelchangeRankingWidth();
		afx_msg void OnSelchangeRankingHeight();
		afx_msg void OnSelchangeRankingRank();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		long m_lRankingCells[49];
		const uint32_t m_InspectionID;
		const uint32_t m_filterID;
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller m_Values;
	};
} //namespace SvOg
