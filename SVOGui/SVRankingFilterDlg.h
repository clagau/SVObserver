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
#include "GuiController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVRankingFilterDlg dialog
		class SVRankingFilterDlg : public CDialog
		{
			// Construction
		public:
			SVRankingFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
			virtual ~SVRankingFilterDlg();

			virtual HRESULT SetInspectionData();
			void EnableCells();

			// Dialog Data
			//{{AFX_DATA(SVRankingFilterDlg)
		public:
			enum { IDD = IDD_RANKING_FILTER };
			CComboBox	m_ctlRankingWidth;
			CComboBox	m_ctlRankingHeight;
			CComboBox	m_ctlRankingRank;
			long	m_lRankingWidth;
			long	m_lRankingHeight;	
			long	m_lRankingRank;
			//}}AFX_DATA

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVRankingFilterDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

			// Generated message map functions
			//{{AFX_MSG(SVRankingFilterDlg)
		protected:
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			afx_msg void OnSelchangeRankingWidth();
			afx_msg void OnSelchangeRankingHeight();
			afx_msg void OnSelchangeRankingRank();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			long m_lRankingCells[49];
			const SVGUID& m_rInspectionID;
			const SVGUID& m_filterID;
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
			typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
			Controller m_Values;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

