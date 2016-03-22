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
#include "ValuesAccessor.h"
#include "GuiController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVCustomFilterDlg dialog
		class SVCustomFilterDlg : public CDialog 
		{
			// Construction
		public:
			SVCustomFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
			virtual ~SVCustomFilterDlg();

		protected:

			virtual HRESULT SetInspectionData();
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
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

			// Generated message map functions
			//{{AFX_MSG(SVCustomFilterDlg)
		protected:
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			virtual void OnCancel();
			afx_msg void OnSelchangeKernelWidth();
			afx_msg void OnSelchangeKernelHeight();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:	
			long	m_lKernelCells[49];
			const SVGUID& m_rInspectionID;
			const SVGUID& m_filterID;
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
			typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
			Controller m_Values;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

