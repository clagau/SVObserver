// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilterDlg
// * .File Name       : $Workfile:   SVThickeningFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:37:22  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "GuiController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVThickeningFilterDlg : public CDialog
	{
		// Construction
	public:
		SVThickeningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVThickeningFilterDlg();

		HRESULT SetInspectionData();

		// Dialog Data
		//{{AFX_DATA(SVThickeningFilterDlg)
		enum { IDD = IDD_THICKENING_FILTER };
		BOOL	m_bGrayScale;
		long	m_lIterations;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVThickeningFilterDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(SVThickeningFilterDlg)
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		virtual void OnCancel() override;
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const SVGUID& m_rInspectionID;
		const SVGUID& m_filterID;
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_Values;
	};
} //namespace SvOg
