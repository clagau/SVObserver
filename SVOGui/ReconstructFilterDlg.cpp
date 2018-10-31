//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file ReconstructFilterDlg.cpp
/// All Rights Reserved
//*****************************************************************************
/// The dialog class for the filter Reconstruct Filter
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "ReconstructFilterDlg.h"
#include "SVObjectLibrary\SVClsids.h"
#include "Definitions\TextDefineSvDef.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
ReconstructFilterDlg::ReconstructFilterDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectId, const SVGUID& rFilterID, CWnd* pParent) :
	CDialog(ReconstructFilterDlg::IDD, pParent)
	, m_filterID(rFilterID)
	, m_rInspectionID(rInspectionID)
	, m_Values {SvOg::BoundValues {rInspectionID, rFilterID}}
	, m_imageController(rInspectionID, rTaskObjectId)
{
	//{{AFX_DATA_INIT(ReconstructFilterDlg)
	m_bGrayScale = false;
	//}}AFX_DATA_INIT
}

ReconstructFilterDlg::~ReconstructFilterDlg()
{
}

HRESULT ReconstructFilterDlg::SetInspectionData()
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

void ReconstructFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReconstructFilterDlg)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_pictureDisplay);
	DDX_Check(pDX, IDC_GRAY_CHECK, m_bGrayScale);
	DDX_Control(pDX, IDC_BLOB_COLOR, m_cbBlobColor);
	DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_SeedImageCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ReconstructFilterDlg, CDialog)
	//{{AFX_MSG_MAP(ReconstructFilterDlg)
	ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeImageCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BlobReconstructFilterDlg message handlers

BOOL ReconstructFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_imageController.Init();
	m_Values.Init();
	m_bGrayScale = m_Values.Get<bool>(SVGrayOnGuid);

	const SvOi::NameValueVector& rBlobColorList = m_Values.GetEnumTypes(SVBlobColorGuid);
	m_cbBlobColor.SetEnumTypes(rBlobColorList);
	long CurrentSelection = m_Values.Get<long>(SVBlobColorGuid);
	m_cbBlobColor.SetCurSelItemData(CurrentSelection);

	const SvUl::NameGuidList& availImages = m_imageController.GetAvailableImageList();
	for (SvUl::NameGuidList::const_iterator it = availImages.begin(); availImages.end() != it; ++it)
	{
		m_SeedImageCombo.AddString(it->first.c_str());
	}

	const SvUl::InputNameGuidPairList& connectedImageList = m_imageController.GetConnectedImageList(m_filterID);
	if (0 < connectedImageList.size() && connectedImageList.begin()->first == SvDef::SeedImageConnectionName)
	{
		m_seedImageName = connectedImageList.begin()->second.first;
	}
	m_SeedImageCombo.SelectString(-1, m_seedImageName.c_str());

	m_pictureDisplay.AddTab("Seed image");
	setImages();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ReconstructFilterDlg::OnOK()
{
	SetInspectionData();

	CDialog::OnOK();
}

void ReconstructFilterDlg::OnCancel()
{
	CDialog::OnCancel();
}

void ReconstructFilterDlg::OnSelchangeImageCombo()
{
	UpdateData(true); // get data from dialog

	int iCurSel = m_SeedImageCombo.GetCurSel();
	CString currentImageName;
	m_SeedImageCombo.GetLBText(iCurSel, currentImageName);

	if (!currentImageName.IsEmpty())
	{
		m_imageController.ConnectToImage(SvDef::SeedImageConnectionName, std::string(currentImageName), m_filterID);
		m_seedImageName = currentImageName;
	}
	setImages();
}

void ReconstructFilterDlg::setImages()
{
	IPictureDisp* pImage = m_imageController.GetImage(m_seedImageName);
	m_pictureDisplay.setImage(pImage);
	m_pictureDisplay.Refresh();
}
}  //namespace SvOg