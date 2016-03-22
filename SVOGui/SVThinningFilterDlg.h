// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThinningFilterDlg
// * .File Name       : $Workfile:   SVThinningFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 12:25:20  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "GuiController.h"
#pragma region Includes

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVThinningFilterDlg dialog
		class SVThinningFilterDlg : public CDialog
		{
			// Construction
		public:
			SVThinningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
			virtual ~SVThinningFilterDlg();

			virtual HRESULT SetInspectionData();

			// Dialog Data
			//{{AFX_DATA(SVThinningFilterDlg)
			enum { IDD = IDD_THINNING_FILTER };
			BOOL	m_bGrayScale;
			long	m_lIterations;
			//}}AFX_DATA


			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVThinningFilterDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		protected:

			// Generated message map functions
			//{{AFX_MSG(SVThinningFilterDlg)
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			virtual void OnCancel();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			const SVGUID& m_rInspectionID;
			const SVGUID& m_filterID;
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
			typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
			Controller m_Values;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

