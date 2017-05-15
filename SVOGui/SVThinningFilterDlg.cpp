// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
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
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVThinningFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	static const std::string GrayOnTag("GrayOn");
	static const std::string IterationTag("Iteration");

	/////////////////////////////////////////////////////////////////////////////
	// SVThinningFilterDlg dialog
	SVThinningFilterDlg::SVThinningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent) : 
		CDialog(SVThinningFilterDlg::IDD, pParent)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
		(GrayOnTag, SVThinningFilterGrayOnGuid)
		(IterationTag, SVThinningFilterItterationsGuid)))
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

		m_Values.Set<bool>(GrayOnTag, TRUE == m_bGrayScale);
		m_Values.Set<long>(IterationTag, m_lIterations);
		m_Values.Commit();

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

		m_Values.Init();
		m_bGrayScale = m_Values.Get<bool>(GrayOnTag);
		m_lIterations = m_Values.Get<long>(IterationTag);

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

