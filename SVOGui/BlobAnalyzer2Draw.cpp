//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Draw.cpp
/// This is the class for the draw tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Draw.h"
#include "BoundValue.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(BlobAnalyzer2Draw, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2Draw)
		ON_BN_CLICKED(IDC_FILL_BLOBS, OnCheckFillBlob)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	BlobAnalyzer2Draw::BlobAnalyzer2Draw(uint32_t inspectionId, uint32_t taskObjectId)
		: CPropertyPage(BlobAnalyzer2Draw::IDD)
		, m_InspectionID(inspectionId)
		, m_TaskObjectID(taskObjectId)
		, m_Values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	{
	}

	BlobAnalyzer2Draw::~BlobAnalyzer2Draw()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool BlobAnalyzer2Draw::QueryAllowExit()
	{
		return setInspectionData();
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void BlobAnalyzer2Draw::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(BlobAnalyzer2Draw)
		DDX_Check(pDX, IDC_FILL_BLOBS, m_useFillBlob);
		DDX_Control(pDX, IDC_COMBO_BLOB_TYPE, m_cbBlobType);
		DDX_Text(pDX, IDC_BLOB_COLOR, m_fillColor);
		DDV_MinMaxLong(pDX, m_fillColor, 0, 255);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2Draw::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_Values.Init();
		m_useFillBlob = m_Values.Get<bool>(SvPb::BlobUseFillEId);

		const SvOi::NameValueVector& rBlobTypeList = m_Values.GetEnumTypes(SvPb::BlobFillTypeEId);
		m_cbBlobType.SetEnumTypes(rBlobTypeList);
		long CurrentSelection = m_Values.Get<long>(SvPb::BlobFillTypeEId);
		m_cbBlobType.SetCurSelItemData(CurrentSelection);
		m_fillColor = m_Values.Get<long>(SvPb::BlobFillColorEId);

		enableControls();

		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void BlobAnalyzer2Draw::OnCheckFillBlob()
	{
		UpdateData();
		enableControls();
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	bool BlobAnalyzer2Draw::setInspectionData()
	{
		UpdateData(TRUE);

		m_Values.Set<bool>(SvPb::BlobUseFillEId, (TRUE == m_useFillBlob));
		int CurrentSelection = m_cbBlobType.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbBlobType.GetItemData(CurrentSelection));
			m_Values.Set<long>(SvPb::BlobFillTypeEId, Value);
		}
		m_Values.Set<byte>(SvPb::BlobFillColorEId, static_cast<byte>(m_fillColor));
		m_Values.Commit();
		
		return true;// (S_OK == hResult);
	}

	void BlobAnalyzer2Draw::enableControls()
	{
		GetDlgItem(IDC_COMBO_BLOB_TYPE)->EnableWindow(m_useFillBlob);
		GetDlgItem(IDC_BLOB_COLOR)->EnableWindow(m_useFillBlob);			
	}
#pragma endregion Private Mehods
} //namespace SvOg
