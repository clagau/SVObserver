//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVDialog
//* .File Name       : $Workfile:   SVOCVDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:02:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVOCVMatchDlg.h"
#include "SVOCVGeneralDlg.h"
#pragma endregion Includes

namespace SvOg
{
	class SVOCVSheet : public CPropertySheet
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	public:
		SVOCVSheet(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVOCVSheet();

		Controller& GetValues() { return m_Values; }

	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

		afx_msg void OnOK();
		afx_msg void OnCancel();
		DECLARE_MESSAGE_MAP()

	private:
		SVOCVGeneralDlg	m_GeneralParamsDlg;
		SVOCVMatchDlg	m_MatchStringParamsDlg;

		const SVGUID& m_rInspectionID;
		const SVGUID& m_rTaskObjectID;
		Controller m_Values;
	};
} //namespace SvOg
