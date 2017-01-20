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
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const SVBarCodeAttributesDialog::SVBarCodeErrorCorrectionEncodingStruct* SVBarCodeAttributesDialog::SVBarCodeEccEncArray::GetInfoByMil(long Mil)
{
	SVBarCodeAttributesDialog::SVBarCodeErrorCorrectionEncodingStruct* pInfo = nullptr;
	for ( int i = 0; i < GetSize(); i++ )
	{
		if ( ElementAt(i).m_Mil == Mil )
		{
			pInfo = &(ElementAt(i));
			break;
		}
	}
	return pInfo;
}

SVBarCodeAttributesDialog::SVBarCodeEccEncArray::~SVBarCodeEccEncArray()
{
}

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeAttributesDialog dialog


SVBarCodeAttributesDialog::SVBarCodeAttributesDialog(CWnd* pParent /*=nullptr*/)
	: CPropertyPage(SVBarCodeAttributesDialog::IDD)
{
	//{{AFX_DATA_INIT(SVBarCodeAttributesDialog)
	m_iEncoding = -1;
	m_iCorrection = -1;
	//}}AFX_DATA_INIT

	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccNone, _T("None") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueAny, _T("Any") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEcc050, _T("ECC-50") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEcc080, _T("ECC-80") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEcc100, _T("ECC-100") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEcc140, _T("ECC-140") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEcc200, _T("ECC-200") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccCheckDigit, _T("Check Digit") ) );

	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon, _T("Reed-Solomon") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon1, _T("Reed-Solomon 1") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon2, _T("Reed-Solomon 2") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon3, _T("Reed-Solomon 3") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon4, _T("Reed-Solomon 4") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon5, _T("Reed-Solomon 5") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon6, _T("Reed-Solomon 6") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon7, _T("Reed-Solomon 7") ) );
	m_aMilEcc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEccReedSolomon8, _T("Reed-Solomon 8") ) );

	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueAny, _T("Any") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEncStandard, _T("Standard") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueEncNum, _T("Numeric") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCAlpha, _T("Alpha") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCAlphaNum, _T("Alpha-Numeric") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCAlphaNumPunc, _T("Alpha-Numeric with Punctuation") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCAscii, _T("ASCII") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCIso8, _T("ISO-8") ) );

	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCMode2, _T("Mode 2") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCMode3, _T("Mode 3") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCMode4, _T("Mode 4") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCMode5, _T("Mode 5") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCMode6, _T("Mode 6") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRss14, _T("RSS-14") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRss14Trunc, _T("RSS-14 Truncated") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRssLimited, _T("RSS Limited") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRssExpanded, _T("RSS Expanded") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRss14Stacked, _T("RSS-14 Stacked") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRss14StackedOmni, _T("RSS-14 Stacked Omni") ) );
	m_aMilEnc.Add( SVBarCodeErrorCorrectionEncodingStruct( SVValueENCRssExpandedStacked, _T("RSS Expanded Stacked") ) );

	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEccNone );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueAny );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEcc050 );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEcc080 );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEcc100 );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEcc140 );
	m_mapBarCodeEcc[ SVDataMatrix ].push_back( SVValueEcc200 );

	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueEncNum );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueENCAlpha );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueENCAlphaNum );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueENCAlphaNumPunc );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueENCAscii );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueENCIso8 );
	m_mapBarCodeEnc[ SVDataMatrix ].push_back( SVValueAny );


	m_mapBarCodeEcc[ SVEan13 ].push_back( SVValueEccCheckDigit );
	m_mapBarCodeEnc[ SVEan13 ].push_back( SVValueEncNum );


	m_mapBarCodeEcc[ SVCode39 ].push_back( SVValueEccNone );
	m_mapBarCodeEcc[ SVCode39 ].push_back( SVValueEccCheckDigit );
	m_mapBarCodeEnc[ SVCode39 ].push_back( SVValueEncStandard );
	m_mapBarCodeEnc[ SVCode39 ].push_back( SVValueENCAscii );


	m_mapBarCodeEcc[ SVInterleaved25 ].push_back( SVValueEccNone );
	m_mapBarCodeEcc[ SVInterleaved25 ].push_back( SVValueEccCheckDigit );
	m_mapBarCodeEnc[ SVInterleaved25 ].push_back( SVValueEncNum );


	m_mapBarCodeEcc[ SVCode128 ].push_back( SVValueEccCheckDigit );
	m_mapBarCodeEnc[ SVCode128 ].push_back( SVValueENCAscii );

	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueAny );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon1 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon2 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon3 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon4 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon5 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon6 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon7 );
	m_mapBarCodeEcc[ SVPDF417 ].push_back( SVValueEccReedSolomon8 );
	m_mapBarCodeEnc[ SVPDF417 ].push_back( SVValueEncStandard );

	m_mapBarCodeEcc[ SVBC412 ].push_back( SVValueEccNone );
	m_mapBarCodeEnc[ SVBC412 ].push_back( SVValueEncStandard );

	m_mapBarCodeEcc[ SVCodeABar ].push_back( SVValueEccNone );
	m_mapBarCodeEnc[ SVCodeABar ].push_back( SVValueEncStandard );

	m_mapBarCodeEcc[ SVMaxiCode ].push_back( SVValueEccReedSolomon );
	m_mapBarCodeEnc[ SVMaxiCode ].push_back( SVValueENCMode2 );
	m_mapBarCodeEnc[ SVMaxiCode ].push_back( SVValueENCMode3 );
	m_mapBarCodeEnc[ SVMaxiCode ].push_back( SVValueENCMode4 );
	m_mapBarCodeEnc[ SVMaxiCode ].push_back( SVValueENCMode5 );
	m_mapBarCodeEnc[ SVMaxiCode ].push_back( SVValueENCMode6 );

	m_mapBarCodeEcc[ SVRssCode ].push_back( SVValueEccNone );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueAny );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRss14 );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRss14Trunc );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRssLimited );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRssExpanded );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRss14Stacked );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRss14StackedOmni );
	m_mapBarCodeEnc[ SVRssCode ].push_back( SVValueENCRssExpandedStacked );
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

double SVBarCodeAttributesDialog::SetEncoding(SVDoubleValueObjectClass &svdEncoding)
{
  svdEncoding.GetValue (m_dEncoding);

  return m_dEncoding;
}

double SVBarCodeAttributesDialog::SetErrCorrection (SVDoubleValueObjectClass &svdErrCorrection)
{
  svdErrCorrection.GetValue (m_dErrorCorrection);

  return m_dErrorCorrection;
}

void SVBarCodeAttributesDialog::InitSelections(long lBarCodeType)
{
	CComboBox* pErrCorrectionBox;
	CComboBox* pEncodingBox;
	
	pErrCorrectionBox = (CComboBox*) GetDlgItem (IDC_BARCODE_ERROR_CORRECTION);
	pEncodingBox = (CComboBox*) GetDlgItem (IDC_BARCODE_ENCODING);
	
	pErrCorrectionBox->Clear();
	pEncodingBox->Clear();
	
	pErrCorrectionBox->ResetContent();
	pEncodingBox->ResetContent();
	
	int iIndex;
	std::vector<long>::iterator iter;
	for ( iter = m_mapBarCodeEcc[ lBarCodeType ].begin(); iter != m_mapBarCodeEcc[ lBarCodeType ].end(); ++iter )
	{
		iIndex = pErrCorrectionBox->AddString( m_aMilEcc.GetInfoByMil( *iter )->m_Name.c_str() );
		pErrCorrectionBox->SetItemData( iIndex, *iter );
	}
	for ( iter = m_mapBarCodeEnc[ lBarCodeType ].begin(); iter != m_mapBarCodeEnc[ lBarCodeType ].end(); ++iter )
	{
		iIndex = pEncodingBox->AddString( m_aMilEnc.GetInfoByMil( *iter )->m_Name.c_str() );
		pEncodingBox->SetItemData( iIndex, *iter );
	}
	
	pErrCorrectionBox->SetCurSel(0);
	for (iIndex = 0; iIndex < pErrCorrectionBox->GetCount(); iIndex++)
	{
		if (pErrCorrectionBox->GetItemData(iIndex) == static_cast<DWORD_PTR>(m_dErrorCorrection))
		{
			pErrCorrectionBox->SetCurSel(iIndex);
		}
	}
	
	pEncodingBox->SetCurSel(0);
	for (iIndex = 0; iIndex < pEncodingBox->GetCount(); iIndex++)
	{
		if (pEncodingBox->GetItemData(iIndex) == static_cast<DWORD_PTR>(m_dEncoding))
		{
			pEncodingBox->SetCurSel(iIndex);
		}
	}
}

void SVBarCodeAttributesDialog::InitSelections()
{
  SVBarCodeProperties *pProps;

  pProps = (SVBarCodeProperties *) GetParent ();
  InitSelections (pProps->m_dlgBarCodeGeneral.GetBarCodeType());
}

void SVBarCodeAttributesDialog::OnSelchangeBarcodeErrorCorrection() 
{
  CComboBox *pErrCorrectionBox;

  pErrCorrectionBox = (CComboBox *) GetDlgItem (IDC_BARCODE_ERROR_CORRECTION);
  m_dErrorCorrection = (double) pErrCorrectionBox->GetItemData(pErrCorrectionBox->GetCurSel());
}

void SVBarCodeAttributesDialog::OnSelchangeBarcodeEncoding() 
{
  CComboBox *pEncodingBox;

  pEncodingBox = (CComboBox *) GetDlgItem (IDC_BARCODE_ENCODING);
  m_dEncoding = (double) pEncodingBox->GetItemData(pEncodingBox->GetCurSel());	
}

DWORD SVBarCodeAttributesDialog::SetEncoding(DWORD dwEncoding)
{
  m_dEncoding = (double) dwEncoding;
  return dwEncoding;
}

DWORD SVBarCodeAttributesDialog::SetErrCorrection(DWORD dwErrCorrection)
{
  m_dErrorCorrection = (double) dwErrCorrection;
  return dwErrCorrection;
}

