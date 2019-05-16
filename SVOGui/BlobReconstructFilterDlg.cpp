//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file BlobReconstructFilterDlg.cpp
/// All Rights Reserved
//*****************************************************************************
/// The dialog class for the filter Blob Reconstruct Filters (e.g. EraseBorderBlobs)
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "BlobReconstructFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
BlobReconstructFilterDlg::BlobReconstructFilterDlg(UINT caption, const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent) :
	CDialog(BlobReconstructFilterDlg::IDD, pParent)
	, m_filterID(rFilterID)
	, m_rInspectionID(rInspectionID)
	, m_captionId(caption)
	, m_Values {SvOg::BoundValues {rInspectionID, rFilterID}}
{
	//{{AFX_DATA_INIT(BlobReconstructFilterDlg)
	m_bGrayScale = false;
	//}}AFX_DATA_INIT
}

BlobReconstructFilterDlg::~BlobReconstructFilterDlg()
{
}

HRESULT BlobReconstructFilterDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData(TRUE); // get data from dialog

	m_Values.Set<bool>(SVGrayOnGuid, m_bGrayScale ? true : false);
	int CurrentSelection = m_cbBlobColor.GetCurSel();
	if (0 <= CurrentSelection)
	{
		long Value = static_cast<long> (m_cbBlobColor.GetItemData(CurrentSelection));
		m_Values.Set<long>(SVBlobColorGuid, Value);
	}
	m_Values.Commit();

	UpdateData(FALSE);

	return l_hrOk;
}

void BlobReconstructFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BlobReconstructFilterDlg)
	DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
	DDX_Control(pDX, IDC_BLOB_COLOR, m_cbBlobColor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(BlobReconstructFilterDlg, CDialog)
	//{{AFX_MSG_MAP(BlobReconstructFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BlobReconstructFilterDlg message handlers

BOOL BlobReconstructFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Values.Init();
	m_bGrayScale = m_Values.Get<bool>(SVGrayOnGuid);

	const SvOi::NameValueVector& rBlobColorList = m_Values.GetEnumTypes(SVBlobColorGuid);
	m_cbBlobColor.SetEnumTypes(rBlobColorList);
	long CurrentSelection = m_Values.Get<long>(SVBlobColorGuid);
	m_cbBlobColor.SetCurSelItemData(CurrentSelection);

	SetWindowText(SvUl::LoadStdString(m_captionId).c_str());

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void BlobReconstructFilterDlg::OnOK()
{
	SetInspectionData();

	CDialog::OnOK();
}

void BlobReconstructFilterDlg::OnCancel()
{
	CDialog::OnCancel();
}
}  //namespace SvOg

