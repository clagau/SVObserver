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
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVBarCodeAttributesDialog::SVBarCodeAttributesDialog(CWnd* /*=nullptr*/)
	: CPropertyPage(SVBarCodeAttributesDialog::IDD)
{
	//{{AFX_DATA_INIT(SVBarCodeAttributesDialog)
	m_iEncoding = -1;
	m_iCorrection = -1;
	//}}AFX_DATA_INIT

	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccNone, _T("None")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueAny, _T("Any")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEcc050, _T("ECC-50")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEcc080, _T("ECC-80")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEcc100, _T("ECC-100")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEcc140, _T("ECC-140")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEcc200, _T("ECC-200")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccCheckDigit, _T("Check Digit")));

	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon, _T("Reed-Solomon")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon1, _T("Reed-Solomon 1")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon2, _T("Reed-Solomon 2")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon3, _T("Reed-Solomon 3")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon4, _T("Reed-Solomon 4")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon5, _T("Reed-Solomon 5")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon6, _T("Reed-Solomon 6")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon7, _T("Reed-Solomon 7")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccReedSolomon8, _T("Reed-Solomon 8")));

	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueAny, _T("Any")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncStandard, _T("Standard")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncNum, _T("Numeric")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncUpcAAddOn, _T("UpcA AddOn")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncUpcEAddOn, _T("UpcE AddOn")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCAlpha, _T("Alpha")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCAlphaNum, _T("Alpha-Numeric")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCAlphaNumPunc, _T("Alpha-Numeric with Punctuation")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCAscii, _T("ASCII")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCIso8, _T("ISO-8")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccH, _T("ECC-H")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccL, _T("ECC-L")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccM, _T("ECC-M")));
	m_aMilEcc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEccQ, _T("ECC-Q")));

	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCMode2, _T("Mode 2")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCMode3, _T("Mode 3")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCMode4, _T("Mode 4")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCMode5, _T("Mode 5")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueENCMode6, _T("Mode 6")));
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1, _T("GS1") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1Trunc, _T("GS1 Truncated") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1Limited, _T("GS1 Limited") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1Expanded, _T("GS1 Expanded") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1Stacked, _T("GS1 Stacked") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1StackedOmni, _T("GS1 Stacked Omni") ) );
	m_aMilEnc.push_back( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCGS1ExpandedStacked, _T("GS1 Expanded Stacked") ) );
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncQRCodeModel1, _T("QRCode Model1")));
	m_aMilEnc.push_back(SVBarCodeErrorCorrectionEncodingStruct(SVValueEncQRCodeModel2, _T("QRCode Model2")));

	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEccNone);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueAny);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEcc050);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEcc080);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEcc100);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEcc140);
	m_mapBarCodeEcc[SVDataMatrix].push_back(SVValueEcc200);

	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueEncNum);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueENCAlpha);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueENCAlphaNum);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueENCAlphaNumPunc);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueENCAscii);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueENCIso8);
	m_mapBarCodeEnc[SVDataMatrix].push_back(SVValueAny);


	m_mapBarCodeEcc[SVEan13].push_back(SVValueEccCheckDigit);
	m_mapBarCodeEnc[SVEan13].push_back(SVValueEncNum);


	m_mapBarCodeEcc[SVCode39].push_back(SVValueEccNone);
	m_mapBarCodeEcc[SVCode39].push_back(SVValueEccCheckDigit);
	m_mapBarCodeEnc[SVCode39].push_back(SVValueEncStandard);
	m_mapBarCodeEnc[SVCode39].push_back(SVValueENCAscii);


	m_mapBarCodeEcc[SVInterleaved25].push_back(SVValueEccNone);
	m_mapBarCodeEcc[SVInterleaved25].push_back(SVValueEccCheckDigit);
	m_mapBarCodeEnc[SVInterleaved25].push_back(SVValueEncNum);


	m_mapBarCodeEcc[SVCode128].push_back(SVValueEccCheckDigit);
	m_mapBarCodeEnc[SVCode128].push_back(SVValueENCAscii);

	m_mapBarCodeEcc[SVPDF417].push_back(SVValueAny);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon1);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon2);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon3);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon4);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon5);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon6);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon7);
	m_mapBarCodeEcc[SVPDF417].push_back(SVValueEccReedSolomon8);
	m_mapBarCodeEnc[SVPDF417].push_back(SVValueEncStandard);

	m_mapBarCodeEcc[SVBC412].push_back(SVValueEccNone);
	m_mapBarCodeEnc[SVBC412].push_back(SVValueEncStandard);

	m_mapBarCodeEcc[SVCodeABar].push_back(SVValueEccNone);
	m_mapBarCodeEnc[SVCodeABar].push_back(SVValueEncStandard);

	m_mapBarCodeEcc[SVMaxiCode].push_back(SVValueEccReedSolomon);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueAny);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueENCMode2);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueENCMode3);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueENCMode4);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueENCMode5);
	m_mapBarCodeEnc[SVMaxiCode].push_back(SVValueENCMode6);

	m_mapBarCodeEnc[SVPostNet].push_back(SVValueEncNum);
	m_mapBarCodeEcc[SVPostNet].push_back(SVValueEccCheckDigit);

	m_mapBarCodeEnc[SVPlanet].push_back(SVValueEncNum);
	m_mapBarCodeEcc[SVPlanet].push_back(SVValueEccCheckDigit);

	m_mapBarCodeEnc[SVUpcA].push_back(SVValueEncNum);
	m_mapBarCodeEnc[SVUpcA].push_back(SVValueEncUpcAAddOn);
	m_mapBarCodeEcc[SVUpcA].push_back(SVValueEccCheckDigit);

	m_mapBarCodeEnc[SVUpcE].push_back(SVValueEncNum);
	m_mapBarCodeEnc[SVUpcE].push_back(SVValueEncUpcEAddOn);
	m_mapBarCodeEcc[SVUpcE].push_back(SVValueEccCheckDigit);

	m_mapBarCodeEnc[SVPharmaCode].push_back(SVValueEncNum);
	m_mapBarCodeEcc[SVPharmaCode].push_back(SVValueEccNone);

	m_mapBarCodeEcc[SVGS1Code].push_back(SVValueEccCheckDigit);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1Trunc);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1Limited);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1Expanded);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1Stacked);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1StackedOmni);
	m_mapBarCodeEnc[SVGS1Code].push_back(SVValueENCGS1ExpandedStacked);

	m_mapBarCodeEcc[SVQRCode].push_back(SVValueAny);
	m_mapBarCodeEcc[SVQRCode].push_back(SVValueEccH);
	m_mapBarCodeEcc[SVQRCode].push_back(SVValueEccL);
	m_mapBarCodeEcc[SVQRCode].push_back(SVValueEccM);
	m_mapBarCodeEcc[SVQRCode].push_back(SVValueEccQ);

	m_mapBarCodeEnc[SVQRCode].push_back(SVValueEncQRCodeModel1);
	m_mapBarCodeEnc[SVQRCode].push_back(SVValueEncQRCodeModel2);
	m_mapBarCodeEnc[SVQRCode].push_back(SVValueAny);
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

double SVBarCodeAttributesDialog::SetEncoding(SvVol::SVDoubleValueObjectClass &svdEncoding)
{
	svdEncoding.GetValue(m_dEncoding);

	return m_dEncoding;
}

double SVBarCodeAttributesDialog::SetErrCorrection(SvVol::SVDoubleValueObjectClass &svdErrCorrection)
{
	svdErrCorrection.GetValue(m_dErrorCorrection);

	return m_dErrorCorrection;
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

	int iIndex;
	for (long MilID : m_mapBarCodeEcc[lBarCodeType])
	{
		iIndex = pErrCorrectionBox->AddString(GetInfoNameByMil(m_aMilEcc, MilID).c_str());
		pErrCorrectionBox->SetItemData(iIndex, MilID);
	}
	for (long MilID : m_mapBarCodeEnc[lBarCodeType])
	{
		iIndex = pEncodingBox->AddString(GetInfoNameByMil(m_aMilEnc, MilID).c_str());
		pEncodingBox->SetItemData(iIndex, MilID);
	}

	pErrCorrectionBox->SetCurSel(0);
	for (iIndex = 0; iIndex < pErrCorrectionBox->GetCount(); iIndex++)
	{
		if (pErrCorrectionBox->GetItemData(iIndex) == static_cast<DWORD_PTR>(m_dErrorCorrection))
		{
			pErrCorrectionBox->SetCurSel(iIndex);
		}
	}
	OnSelchangeBarcodeErrorCorrection();

	pEncodingBox->SetCurSel(0);
	for (iIndex = 0; iIndex < pEncodingBox->GetCount(); iIndex++)
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
	SVBarCodeProperties *pProps;

	pProps = (SVBarCodeProperties *)GetParent();
	InitSelections(pProps->m_dlgBarCodeGeneral.GetBarCodeType());
}

void SVBarCodeAttributesDialog::OnSelchangeBarcodeErrorCorrection()
{
	CComboBox *pErrCorrectionBox = (CComboBox *)GetDlgItem(IDC_BARCODE_ERROR_CORRECTION);
	m_dErrorCorrection = (double)pErrCorrectionBox->GetItemData(pErrCorrectionBox->GetCurSel());
}

void SVBarCodeAttributesDialog::OnSelchangeBarcodeEncoding()
{
	CComboBox *pEncodingBox = (CComboBox *)GetDlgItem(IDC_BARCODE_ENCODING);
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

std::string SVBarCodeAttributesDialog::GetInfoNameByMil(const SVBarCodeEccEncVector& rMilVector, long MilID)
{
	std::string Result;

	for (auto& rElement : rMilVector)
	{
		if (MilID == rElement.m_Mil)
		{
			Result = rElement.m_Name;
			break;
		}
	}
	return Result;
}
