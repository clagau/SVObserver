//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVDialog
//* .File Name       : $Workfile:   SVOCVDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:58:54  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVOCVSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVOCVSheet::SVOCVSheet(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=nullptr*/)
	: CPropertySheet(IDS_OCV_PARAMETERS, pParent)
	, m_GeneralParamsDlg{ *this }
	, m_MatchStringParamsDlg{ *this }
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	{
		m_psh.dwFlags |= PSH_NOAPPLYNOW;

		AddPage(&m_GeneralParamsDlg);
		AddPage(&m_MatchStringParamsDlg);

		m_Values.Init();
	}

	SVOCVSheet::~SVOCVSheet()
	{
		RemovePage (&m_GeneralParamsDlg);
		RemovePage (&m_MatchStringParamsDlg);
	}

	void SVOCVSheet::DoDataExchange(CDataExchange* pDX)
	{
		CPropertySheet::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVOCVDialogClass)
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVOCVSheet, CPropertySheet)
		//{{AFX_MSG_MAP(SVOCVDialogClass)
		//}}AFX_MSG_MAP
		ON_COMMAND( IDOK, OnOK)
		ON_COMMAND( IDCANCEL, OnCancel)
	END_MESSAGE_MAP()

	void SVOCVSheet::OnOK()
	{
		m_GeneralParamsDlg.UpdateOCVParameters();
		m_MatchStringParamsDlg.UpdateOCVParameters();

		CPropertySheet::EndDialog(IDOK);
	}

	void SVOCVSheet::OnCancel()
	{
		CPropertySheet::EndDialog(IDCANCEL);
	}
} //namespace SvOg