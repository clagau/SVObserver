//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAttributesDialog
//* .File Name       : $Workfile:   SVBarCodeAttributesDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 10:21:20  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"

#include "SVBarCodeAttributesDialog.h"
#include "SVBarCodeProperties.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVBarCodeAttributesDialog::SVBarCodeAttributesDialog(CWnd* /*=nullptr*/)
		: CPropertyPage(SVBarCodeAttributesDialog::IDD)
		, m_dErrorCorrection{ SVValueDefault }
		, m_dEncoding{ SVValueDefault }
	{
		//{{AFX_DATA_INIT(SVBarCodeAttributesDialog)
		//}}AFX_DATA_INIT
	}

	SVBarCodeAttributesDialog::~SVBarCodeAttributesDialog()
	{
	}

	void SVBarCodeAttributesDialog::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVBarCodeAttributesDialog)
		DDX_CBIndex(pDX, IDC_BARCODE_ENCODING, m_iEncoding);
		DDX_CBIndex(pDX, IDC_BARCODE_ERROR_CORRECTION, m_iCorrection);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(SVBarCodeAttributesDialog, CPropertyPage)
		//{{AFX_MSG_MAP(SVBarCodeAttributesDialog)
		ON_CBN_SELCHANGE(IDC_BARCODE_ERROR_CORRECTION, OnSelchangeBarcodeErrorCorrection)
		ON_CBN_SELCHANGE(IDC_BARCODE_ENCODING, OnSelchangeBarcodeEncoding)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVBarCodeAttributesDialog message handlers

	BOOL SVBarCodeAttributesDialog::OnInitDialog()
	{
		return CPropertyPage::OnInitDialog();
	}

	BOOL SVBarCodeAttributesDialog::OnSetActive()
	{
		InitSelections();
		return TRUE;
	}

	double SVBarCodeAttributesDialog::GetEncoding()
	{
		return m_dEncoding;
	}

	double SVBarCodeAttributesDialog::GetErrorCorrection()
	{
		return m_dErrorCorrection;
	}

	void SVBarCodeAttributesDialog::SetEncoding(double encoding)
	{
		m_dEncoding = encoding;
	}

	void SVBarCodeAttributesDialog::SetErrCorrection(double errorCorrection)
	{
		m_dErrorCorrection = errorCorrection;
	}

	void SVBarCodeAttributesDialog::InitSelections(long lBarCodeType)
	{
		CComboBox* pErrCorrectionBox;
		CComboBox* pEncodingBox;

		pErrCorrectionBox = (CComboBox*)GetDlgItem(IDC_BARCODE_ERROR_CORRECTION);
		pEncodingBox = (CComboBox*)GetDlgItem(IDC_BARCODE_ENCODING);

		pErrCorrectionBox->Clear();
		pEncodingBox->Clear();

		pErrCorrectionBox->ResetContent();
		pEncodingBox->ResetContent();

		const auto& rInfos = getBarCodeInfoByType(lBarCodeType);
		for (long MilID : rInfos.possibleecctypes())
		{
			int iIndex = pErrCorrectionBox->AddString(GetInfoNameByMil(m_barCodeTypeInfos.eccnames(), MilID).c_str());
			pErrCorrectionBox->SetItemData(iIndex, MilID);
		}
		for (long MilID : rInfos.possibleenctypes())
		{
			int iIndex = pEncodingBox->AddString(GetInfoNameByMil(m_barCodeTypeInfos.encnames(), MilID).c_str());
			pEncodingBox->SetItemData(iIndex, MilID);
		}

		pErrCorrectionBox->SetCurSel(0);
		for (int iIndex = 0; iIndex < pErrCorrectionBox->GetCount(); iIndex++)
		{
			if (pErrCorrectionBox->GetItemData(iIndex) == static_cast<DWORD_PTR>(m_dErrorCorrection))
			{
				pErrCorrectionBox->SetCurSel(iIndex);
			}
		}
		OnSelchangeBarcodeErrorCorrection();

		pEncodingBox->SetCurSel(0);
		for (int iIndex = 0; iIndex < pEncodingBox->GetCount(); iIndex++)
		{
			if (pEncodingBox->GetItemData(iIndex) == static_cast<DWORD_PTR>(m_dEncoding))
			{
				pEncodingBox->SetCurSel(iIndex);
			}
		}
		OnSelchangeBarcodeEncoding();
	}

	void SVBarCodeAttributesDialog::InitSelections()
	{
		SVBarCodeProperties* pProps = static_cast<SVBarCodeProperties*>(GetParent());
		InitSelections(pProps->m_dlgBarCodeGeneral.GetBarCodeType());
	}

	void SVBarCodeAttributesDialog::OnSelchangeBarcodeErrorCorrection()
	{
		CComboBox* pErrCorrectionBox = (CComboBox*)GetDlgItem(IDC_BARCODE_ERROR_CORRECTION);
		m_dErrorCorrection = (double)pErrCorrectionBox->GetItemData(pErrCorrectionBox->GetCurSel());
	}

	void SVBarCodeAttributesDialog::OnSelchangeBarcodeEncoding()
	{
		CComboBox* pEncodingBox = (CComboBox*)GetDlgItem(IDC_BARCODE_ENCODING);
		m_dEncoding = (double)pEncodingBox->GetItemData(pEncodingBox->GetCurSel());
	}

	DWORD SVBarCodeAttributesDialog::SetEncoding(DWORD dwEncoding)
	{
		m_dEncoding = (double)dwEncoding;
		return dwEncoding;
	}

	DWORD SVBarCodeAttributesDialog::SetErrCorrection(DWORD dwErrCorrection)
	{
		m_dErrorCorrection = (double)dwErrCorrection;
		return dwErrCorrection;
	}

	const SvPb::BarCodeTypeParameter& SVBarCodeAttributesDialog::getBarCodeInfoByType(long type) const
	{
		auto iter = std::find_if(m_barCodeTypeInfos.typeparameters().cbegin(), m_barCodeTypeInfos.typeparameters().cend(), [type](const auto& rEntry) { return rEntry.type() == type; });
		if (iter != m_barCodeTypeInfos.typeparameters().cend())
		{
			return *iter;
		}

		assert(false);
		return *m_barCodeTypeInfos.typeparameters().cbegin();
	}
}