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
#include "DataController.h"
#pragma region Includes

namespace SvOg
{
	class SVThinningFilterDlg : public CDialog
	{
		// Construction
	public:
		SVThinningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVThinningFilterDlg();

		HRESULT SetInspectionData();

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
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(SVThinningFilterDlg)
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		virtual void OnCancel() override;
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const SVGUID& m_rInspectionID;
		const SVGUID& m_filterID;
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_Values;
	};
} //namespace SvOg
