// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.cpp
// * .File Name       : $Workfile:   SVFillBlobDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 07:05:36  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFillBlobDlg.h"
#include "DataController.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVFillBlobDlg, CDialog)
		//{{AFX_MSG_MAP(SVFillBlobDlg)
		ON_CBN_SELCHANGE(IDC_COMBO_BLOB_COLOR, OnSelchangeComboBlobColor)
		ON_CBN_SELCHANGE(IDC_COMBO_BLOB_TYPE, OnSelchangeComboBlobType)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVFillBlobDlg::SVFillBlobDlg(ValueController& rValues, CWnd* pParent /*=nullptr*/)
	: CDialog(SVFillBlobDlg::IDD, pParent)
	, m_rValues(rValues)
	{
	}

	SVFillBlobDlg::~SVFillBlobDlg()
	{
	}

	HRESULT SVFillBlobDlg::SetInspectionData()
	{
		long Value{ 0L };
		int CurrentSelection = m_ctlBlobFillType.GetCurSel();
		if (0 <= CurrentSelection)
		{
			Value = static_cast<long> (m_ctlBlobFillType.GetItemData(CurrentSelection));
			m_rValues.Set<long>(SvPb::BlobFillTypeEId, Value);
		}

		CurrentSelection = m_ctlBlobFillColor.GetCurSel();
		if (0 <= CurrentSelection)
		{
			Value = static_cast<long> (m_ctlBlobFillColor.GetItemData(CurrentSelection));
			m_rValues.Set<long>(SvPb::BlobFillColorEId, Value);
		}

		return m_rValues.Commit();
	}

	void SVFillBlobDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVFillBlobDlg)
		DDX_Control(pDX, IDC_COMBO_BLOB_TYPE, m_ctlBlobFillType);
		DDX_Control(pDX, IDC_COMBO_BLOB_COLOR, m_ctlBlobFillColor);
		//}}AFX_DATA_MAP
	}

	BOOL SVFillBlobDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		FillCombos();
	
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVFillBlobDlg::FillCombos()
	{
		const SvOi::NameValueVector& rBlobFillTypeList = m_rValues.GetEnumTypes(SvPb::BlobFillTypeEId);
		m_ctlBlobFillType.SetEnumTypes(rBlobFillTypeList);
		long CurrentSelection = m_rValues.Get<long>(SvPb::BlobFillTypeEId);
		m_ctlBlobFillType.SetCurSelItemData(CurrentSelection);

		const SvOi::NameValueVector& rBlobFillColorList = m_rValues.GetEnumTypes(SvPb::BlobFillColorEId);
		m_ctlBlobFillColor.SetEnumTypes(rBlobFillColorList);
		CurrentSelection = m_rValues.Get<long>(SvPb::BlobFillColorEId);
		m_ctlBlobFillColor.SetCurSelItemData(CurrentSelection);

		UpdateData(false);
	}

	void SVFillBlobDlg::OnSelchangeComboBlobColor() 
	{
		SetInspectionData();
	}

	void SVFillBlobDlg::OnSelchangeComboBlobType() 
	{
		SetInspectionData();
	}
} //namespace SvOg
