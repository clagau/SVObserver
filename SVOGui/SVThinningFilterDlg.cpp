// ******************************************************************************
// * COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThinningFilterDlg
// * .File Name       : $Workfile:   SVThinningFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 12:25:06  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVThinningFilterDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVThinningFilterDlg::SVThinningFilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent) :
		CDialog(SVThinningFilterDlg::IDD, pParent)
		,m_filterID(filterId)
		,m_InspectionID(inspectionId)
		,m_values{ SvOgu::BoundValues{ inspectionId, filterId } }
	{
		//{{AFX_DATA_INIT(SVThinningFilterDlg)
		m_bGrayScale = FALSE;
		m_lIterations = 0;
		//}}AFX_DATA_INIT
	}

	SVThinningFilterDlg::~SVThinningFilterDlg()
	{
	}

	HRESULT SVThinningFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		m_values.Set<bool>(SvPb::GrayOnEId, m_bGrayScale ? true : false);
		m_values.Set<long>(SvPb::ThinningFilterItterationsEId, m_lIterations);
		m_values.Commit();

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVThinningFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVThinningFilterDlg)
		DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
		DDX_Text(pDX, IDC_ITTERATIONS_EDIT, m_lIterations);
		//}}AFX_DATA_MAP
	}
	
	BEGIN_MESSAGE_MAP(SVThinningFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVThinningFilterDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVThinningFilterDlg message handlers

	BOOL SVThinningFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_values.Init();
		m_bGrayScale = m_values.Get<bool>(SvPb::GrayOnEId);
		m_lIterations = m_values.Get<long>(SvPb::ThinningFilterItterationsEId);

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVThinningFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVThinningFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}
}  //namespace SvOg

