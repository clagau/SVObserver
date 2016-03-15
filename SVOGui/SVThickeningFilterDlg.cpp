// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilterDlg
// * .File Name       : $Workfile:   SVThickeningFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:37:08  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <boost/assign/list_of.hpp>
#include "SVThickeningFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVOGui
	{
		static const std::string GrayOnTag("GrayOn");
		static const std::string IterationTag("Iteration");
		/////////////////////////////////////////////////////////////////////////////
		// SVThickeningFilterDlg dialog
	SVThickeningFilterDlg::SVThickeningFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent): 
		CDialog(SVThickeningFilterDlg::IDD)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
			(GrayOnTag, SVThickeningFilterGrayOnGuid)
			(IterationTag, SVThickeningFilterItterationsGuid)))
	{
		//{{AFX_DATA_INIT(SVThickeningFilterDlg)
		m_bGrayScale = FALSE;
		m_lIterations = 0;
		//}}AFX_DATA_INIT
	}

	SVThickeningFilterDlg::~SVThickeningFilterDlg()
	{
	}

	HRESULT SVThickeningFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		m_Values.Set<bool>(GrayOnTag, TRUE == m_bGrayScale);
		m_Values.Set<long>(IterationTag, m_lIterations);
		m_Values.Commit();

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVThickeningFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVThickeningFilterDlg)
		DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
		DDX_Text(pDX, IDC_ITTERATIONS_EDIT, m_lIterations);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVThickeningFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVThickeningFilterDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVThickeningFilterDlg message handlers

	BOOL SVThickeningFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_Values.Init();
		m_bGrayScale = m_Values.Get<bool>(GrayOnTag);
		m_lIterations = m_Values.Get<long>(IterationTag);

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVThickeningFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVThickeningFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

