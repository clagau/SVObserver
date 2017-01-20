//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeGeneralDialog
//* .File Name       : $Workfile:   svbarcodegeneraldialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 14:32:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVBarCodeGeneralDialog.h"
#include "SVBarCodeProperties.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVBarCodeGeneralDialog::SVBarCodeInfoArray::~SVBarCodeInfoArray()
{
}

const SVBarCodeGeneralDialog::SVBarCodeInfoStruct* SVBarCodeGeneralDialog::SVBarCodeInfoArray::GetInfoByIndex(int iIndex)
{
	SVBarCodeGeneralDialog::SVBarCodeInfoStruct* pInfo = nullptr;
	for ( int i = 0; i < GetSize(); i++ )
	{
		if ( ElementAt(i).iIndex == iIndex )
		{
			pInfo = &(ElementAt(i));
			break;
		}
	}

	return pInfo;
}

const SVBarCodeGeneralDialog::SVBarCodeInfoStruct* SVBarCodeGeneralDialog::SVBarCodeInfoArray::GetInfoByMilID(long lMilID)
{
	SVBarCodeGeneralDialog::SVBarCodeInfoStruct* pInfo = nullptr;
	for ( int i = 0; i < GetSize(); i++ )
	{
		if ( ElementAt(i).lMil == lMilID )
		{
			pInfo = &(ElementAt(i));
			break;
		}
	}
	return pInfo;
}

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeGeneralDialog dialog

SVBarCodeGeneralDialog::SVBarCodeGeneralDialog(CWnd* pParent /*=nullptr*/)
	: CPropertyPage(SVBarCodeGeneralDialog::IDD)
	, m_lStringFormat(0)
	, m_iThresholdRadio(0)
{
	//{{AFX_DATA_INIT(SVBarCodeGeneralDialog)
	m_iBarCodeColor = -1;
	m_iOrientation = 0;
	m_szOrientation = _T("");
	m_iSkewNegative = 0;
	m_szSkewNegative = _T("");
	m_iSkewPositive = 0;
	m_szSkewPositive = _T("");
	m_szStringSize = _T("");
	m_iThreshold = 0;
	m_szThreshold = _T("");
	m_iBarCodeType = -1;
	m_iSearchSpeed = -1;
	m_bWarnOnFail = FALSE;
	m_lTimeout = 0;
	//}}AFX_DATA_INIT

	m_lInitialBarCodeType = SVDataMatrix;
	m_bUnEvenGrid = false;

	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVDataMatrix, _T("Data Matrix"), SVValueAny, SVValueAny )); // 2D
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVEan13, _T("EAN-13"), SVValueEncNum, SVValueEccCheckDigit ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVCode39, _T("3 of 9"), SVValueEncStandard, SVValueEccNone ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVInterleaved25, _T("Interleaved 2 of 5"), SVValueEncNum, SVValueEccNone ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVCode128, _T("Code 128"), SVValueEncAscii, SVValueEccCheckDigit ));

	// Added for SVObserver 4.20
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVPDF417, _T("PDF417"), SVValueEncStandard, SVValueAny ));	// 2D
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVBC412, _T("BC412"), SVValueEncStandard, SVValueEccNone ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVCodeABar, _T("Codabar"), SVValueEncStandard, SVValueEccNone ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVMaxiCode, _T("Maxicode"), SVValueEncMode2, SVValueEccReedSolomon ));	// 2D
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVPostNet, _T("Postnet"), SVValueEncStandard, SVValueAny ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVPlanet, _T("Planet"), SVValueEncStandard, SVValueAny ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVUpcA, _T("UPC-A"), SVValueEncStandard, SVValueAny ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVUpcE, _T("UPC-E"), SVValueEncStandard, SVValueAny ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVPharmaCode, _T("Pharmacode"), SVValueEncStandard, SVValueAny ));
	m_aBarCodeInfo.Add(SVBarCodeInfoStruct( SVRssCode, _T("RSS"), SVValueEncRss14, SVValueAny ));
}

SVBarCodeGeneralDialog::~SVBarCodeGeneralDialog()
{
}

void SVBarCodeGeneralDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBarCodeGeneralDialog)
	DDX_Control(pDX, IDC_SPIN_TIMEOUT, m_spnTimeout);
	DDX_Control(pDX, IDC_EDT_TIMEOUT, m_ctlTimeout);
	DDX_Control(pDX, IDC_BARCODETYPE, m_cbBarcodeType);
	DDX_CBIndex(pDX, IDC_BARCODE_COLOR, m_iBarCodeColor);
	DDX_Slider(pDX, IDC_BARCODE_ORIENTATION, m_iOrientation);
	DDX_Text(pDX, IDC_BARCODE_ORIENTATION_EDIT, m_szOrientation);
	DDX_Slider(pDX, IDC_BARCODE_SKEWNEGATIVE, m_iSkewNegative);
	DDX_Text(pDX, IDC_BARCODE_SKEWNEGATIVE_EDIT, m_szSkewNegative);
	DDX_Slider(pDX, IDC_BARCODE_SKEWPOSITIVE, m_iSkewPositive);
	DDX_Text(pDX, IDC_BARCODE_SKEWPOSITIVE_EDIT, m_szSkewPositive);
	DDX_CBString(pDX, IDC_BARCODE_STRINGSIZE, m_szStringSize);
	DDX_Slider(pDX, IDC_BARCODE_THRESHOLD, m_iThreshold);
	DDX_Text(pDX, IDC_BARCODE_THRESHOLD_EDIT, m_szThreshold);
	DDX_CBIndex(pDX, IDC_BARCODETYPE, m_iBarCodeType);
	DDX_CBIndex(pDX, IDC_SEARCH_SPEED, m_iSearchSpeed);
	DDX_Text(pDX, IDC_EDT_TIMEOUT, m_lTimeout);
	DDV_MinMaxLong(pDX, m_lTimeout, 1, 10000);
	DDX_Check(pDX, IDC_WARN_ON_FAIL_CHECK, m_bWarnOnFail);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_STRING_FORMAT, m_lStringFormat);
	DDX_Control(pDX, IDC_STRING_FORMAT, m_StringFormatCombo);
	DDX_Radio(pDX, IDC_THRESHOLD_NORMAL_RADIO, m_iThresholdRadio);
	DDX_Control(pDX, IDC_CHK_UNEVEN_GRID, m_UnevenCheck);
}


BEGIN_MESSAGE_MAP(SVBarCodeGeneralDialog, CPropertyPage)
	//{{AFX_MSG_MAP(SVBarCodeGeneralDialog)
	ON_WM_HSCROLL()
	ON_EN_UPDATE(IDC_BARCODE_ORIENTATION_EDIT, OnUpdateBarcodeOrientationEdit)
	ON_EN_UPDATE(IDC_BARCODE_SKEWNEGATIVE_EDIT, OnUpdateBarcodeSkewNegativeEdit)
	ON_EN_UPDATE(IDC_BARCODE_SKEWPOSITIVE_EDIT, OnUpdateBarcodeSkewPositiveEdit)
	ON_EN_UPDATE(IDC_BARCODE_THRESHOLD_EDIT, OnUpdateBarcodeThresholdEdit)
	ON_CBN_EDITCHANGE(IDC_BARCODE_STRINGSIZE, OnEditChangeBarCodeStringSize)
	ON_CBN_SELCHANGE(IDC_BARCODE_COLOR, OnSelChangeBarCodeColor)
	ON_CBN_SELCHANGE(IDC_SEARCH_SPEED, OnSelChangeSearchSpeed)
	ON_CBN_SELCHANGE(IDC_BARCODETYPE, OnSelChangeBarCodeType)
	ON_CBN_SELCHANGE(IDC_BARCODE_STRINGSIZE, OnSelChangeBarCodeStringSize)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIMEOUT, OnDeltaposSpinTimeout)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_THRESHOLD_NORMAL_RADIO, IDC_THRESHOLD_ADAPTIVE_RADIO, &SVBarCodeGeneralDialog::OnBnClickedThresholdNormalRadio)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_UNEVEN_GRID, &SVBarCodeGeneralDialog::OnBnClickedChkUnevenGrid)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeGeneralDialog message handlers



long SVBarCodeGeneralDialog::GetBarCodeType()
{
	
	long lMil=CB_ERR;
	
	if ( m_iBarCodeType != CB_ERR )
	{
		const SVBarCodeInfoStruct* pInfo = m_aBarCodeInfo.GetInfoByIndex( m_iBarCodeType );
		ASSERT( pInfo );
		if ( pInfo )
			lMil = pInfo->lMil;
	}
	
	if ( lMil == CB_ERR )
		lMil = 0;
	
	return lMil;
	
}

long SVBarCodeGeneralDialog::SetBarCodeType(SVLongValueObjectClass& svlBarCodeType)
{
	long lBarCodeType;
	
	svlBarCodeType.GetValue (lBarCodeType);

	m_lInitialBarCodeType = lBarCodeType;
	const SVBarCodeInfoStruct* pInfo = m_aBarCodeInfo.GetInfoByMilID( lBarCodeType );
	ASSERT( pInfo );
	if ( pInfo )
	{
		m_iBarCodeType = pInfo->iIndex;
	}
	
	return lBarCodeType;
}

double SVBarCodeGeneralDialog::GetBarCodeStringSize()
{
  if (!m_szStringSize.Compare(_T("<Any>")))
    return (double) SVValueDefault;
  return (double) atol (m_szStringSize);
}

double SVBarCodeGeneralDialog::SetBarCodeStringSize(SVDoubleValueObjectClass& svdStringSize)
{
  double dStringSize;

  svdStringSize.GetValue(dStringSize);
  if (dStringSize == (double) SVValueDefault)
    m_szStringSize = _T("<Any>");
  else
    m_szStringSize.Format(_T("%d"), (int) dStringSize);

  return dStringSize;
}

double SVBarCodeGeneralDialog::GetBarCodeSearchSpeed()
{
  switch (m_iSearchSpeed)
  {
  case 0 :
    return (double) SVValueVeryLow;
    break;

  case 1 :
    return (double) SVValueLow;
    break;

  case 2 :
    return (double) SVValueMedium;
    break;

  case 3 :
    return (double) SVValueHigh;
    break;

  case 4 :
    return (double) SVValueVeryHigh;
    break;

  default :
    return 0.0;
    break;

  }
}

double SVBarCodeGeneralDialog::SetBarCodeSearchSpeed(SVDoubleValueObjectClass& svdSearchSpeed)
{
  double dSearchSpeed;

  svdSearchSpeed.GetValue (dSearchSpeed);

  switch ((long) dSearchSpeed)
  {
  case  SVValueVeryLow :
    m_iSearchSpeed = 0;
    break;

  case SVValueLow :
    m_iSearchSpeed = 1;
    break;

  case SVValueMedium :
    m_iSearchSpeed = 2;
    break;

  case SVValueHigh :
    m_iSearchSpeed = 3;
    break;

  case SVValueVeryHigh :
    m_iSearchSpeed = 4;
    break;

  default :
    break;
  }
  return dSearchSpeed;
}

double SVBarCodeGeneralDialog::GetForegroundColor()
{
  switch (m_iBarCodeColor)
  {
    case 0 :
      return (double) SVValueForegroundBlack;
      break;

    case 1 :
      return (double) SVValueForegroundWhite;
      break;

    default :
      return (double) SVValueForegroundBlack;
      break;
  }
}

double SVBarCodeGeneralDialog::SetForegroundColor(SVDoubleValueObjectClass& svdForegroundColor)
{
  double dBarCodeColor;

  svdForegroundColor.GetValue (dBarCodeColor);

  switch ((long) dBarCodeColor)
  {
	case SVValueForegroundBlack:
      m_iBarCodeColor = 0;
      break;

	case SVValueForegroundWhite:
      m_iBarCodeColor = 1;
      break;

    default :
      m_iBarCodeColor = 0;
      break;
  }
  return dBarCodeColor;
}

double SVBarCodeGeneralDialog::GetOrientation()
{
  return (double) m_iOrientation;
}

double SVBarCodeGeneralDialog::SetOrientation(SVDoubleValueObjectClass &svdOrientation)
{
  double dOrientation;

  svdOrientation.GetValue(dOrientation);
  if (dOrientation == SVValueDefault)
    dOrientation = 0.0;

  m_iOrientation = (int) dOrientation;
  m_szOrientation.Format("%d", (int) dOrientation);
  return dOrientation;
}

double SVBarCodeGeneralDialog::GetSkewNegative()
{
  return (double) m_iSkewNegative;
}

double SVBarCodeGeneralDialog::SetSkewNegative(SVDoubleValueObjectClass &svdSkewNegative)
{
  double dSkewNegative;

  svdSkewNegative.GetValue(dSkewNegative);
  if (dSkewNegative == SVValueDefault)
    dSkewNegative = 0.0;

  m_iSkewNegative = (int) dSkewNegative;
  m_szSkewNegative.Format("%d", (int) dSkewNegative);
  return dSkewNegative;
}

double SVBarCodeGeneralDialog::GetSkewPositive()
{
  return (double) m_iSkewPositive;
}

double SVBarCodeGeneralDialog::SetSkewPositive(SVDoubleValueObjectClass &svdSkewPositive)
{
  double dSkewPositive;

  svdSkewPositive.GetValue(dSkewPositive);
  if (dSkewPositive == SVValueDefault)
    dSkewPositive = 0.0;

  m_iSkewPositive = (int) dSkewPositive;
  m_szSkewPositive.Format("%d", (int) dSkewPositive);
  return dSkewPositive;
}

double SVBarCodeGeneralDialog::GetThreshold()
{
  return (double) m_iThreshold;
}

double SVBarCodeGeneralDialog::SetThreshold(SVDoubleValueObjectClass &svdThreshold)
{
  double dThreshold;

  svdThreshold.GetValue(dThreshold);
  if (dThreshold == SVValueDefault)
    dThreshold = 0.0;

  m_iThreshold = (int) dThreshold;
  m_szThreshold.Format("%d", (int) dThreshold);
  return dThreshold;
}

bool SVBarCodeGeneralDialog::SetBarcodeStringFormat( SVEnumerateValueObjectClass& p_sveStringFormat )
{

	p_sveStringFormat.GetEnumTypes( m_EnumVect );

	DWORD l_dwValue;
	bool l_bRet = S_OK == p_sveStringFormat.GetValue( l_dwValue );
	m_lStringFormat = l_dwValue;
	return l_bRet;
}

long SVBarCodeGeneralDialog::GetBarcodeStringFormat( )
{
	return m_lStringFormat;
}

bool SVBarCodeGeneralDialog::SetBarcodeThresholdType( SVLongValueObjectClass& p_svlThresholdType )
{
	long l_lType = 0;
	bool l_bRet = S_OK == p_svlThresholdType.GetValue( l_lType );
	if( l_lType > SVBCThresholdAdaptive )
	{
		l_lType = SVBCThresholdNormal ;
	}
	m_iThresholdRadio = l_lType;
	return l_bRet;
}
int SVBarCodeGeneralDialog::GetBarcodeThresholdType( )
{
	return m_iThresholdRadio;
}

bool SVBarCodeGeneralDialog::SetUnEvenGrid(SVBoolValueObjectClass p_svbUnEvenGrid)
{
	BOOL bVal;
	bool l_bRet = S_OK == p_svbUnEvenGrid.GetValue(bVal);
	
	m_bUnEvenGrid = bVal;
	return l_bRet;
}

BOOL SVBarCodeGeneralDialog::GetUnEvenGrid()
{
	return m_bUnEvenGrid;
}

BOOL SVBarCodeGeneralDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// String Format 
	m_StringFormatCombo.ResetContent();
	for( int i = 0 ; i < static_cast< int >( m_EnumVect.size() ); i++ )
	{
		m_StringFormatCombo.InsertString( i, m_EnumVect[i].first.c_str() );
	}
	m_StringFormatCombo.SetCurSel( m_lStringFormat );


	for (int i = 0; i < m_aBarCodeInfo.GetSize(); i++)
	{
		SVBarCodeInfoStruct& rInfo = m_aBarCodeInfo.ElementAt(i);
		int iIndex = m_cbBarcodeType.AddString( rInfo.strName );
		m_cbBarcodeType.SetItemData( iIndex, static_cast<DWORD_PTR>(rInfo.lMil) );
		rInfo.iIndex = iIndex;
	}

	const SVBarCodeInfoStruct* pInfo = m_aBarCodeInfo.GetInfoByMilID( m_lInitialBarCodeType );
	ASSERT( pInfo );
	if ( pInfo )
	{
		m_iBarCodeType = pInfo->iIndex;
	}

	CSliderCtrl *pControl;
	
	pControl = (CSliderCtrl *) GetDlgItem (IDC_BARCODE_ORIENTATION);
	pControl->SetRangeMin (0, FALSE);
	pControl->SetRangeMax (360, TRUE);
	
	pControl = (CSliderCtrl *) GetDlgItem (IDC_BARCODE_SKEWNEGATIVE);
	pControl->SetRangeMin (0, FALSE);
	pControl->SetRangeMax (180, TRUE);
	
	pControl = (CSliderCtrl *) GetDlgItem (IDC_BARCODE_SKEWPOSITIVE);
	pControl->SetRangeMin (0, FALSE);
	pControl->SetRangeMax (180, TRUE);

	// Threshold...
	pControl = (CSliderCtrl *) GetDlgItem (IDC_BARCODE_THRESHOLD);
	pControl->SetRangeMin (0, FALSE);
	pControl->SetRangeMax (255, TRUE);

	m_spnTimeout.SetBuddy(GetWindow(IDC_EDT_TIMEOUT));
	
	UpdateData (FALSE);

	OnBnClickedThresholdNormalRadio(0);

 	UpdateUnEvenGrid();
	
	return TRUE;  
}

void SVBarCodeGeneralDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

  CSliderCtrl *pSlider = (CSliderCtrl *) pScrollBar;
  int iNewPos;

  iNewPos = pSlider->GetPos ();

  switch (nSBCode)
  {
    case SB_LEFT :
      iNewPos = pSlider->GetRangeMin();
      break;

    case SB_ENDSCROLL :
      break;

    case SB_LINELEFT :
      iNewPos--;
      break;

    case SB_LINERIGHT :
      iNewPos++;
      break;

    case SB_PAGELEFT :
      iNewPos -= 10;
      break;

    case SB_PAGERIGHT :
      iNewPos += 10;
      break;

    case SB_RIGHT :
      iNewPos = pSlider->GetRangeMax();
      break;

    case SB_THUMBPOSITION :
      iNewPos = nPos;
      break;

    case SB_THUMBTRACK :
      iNewPos = nPos;
      break;
  }
  if (iNewPos < pSlider->GetRangeMin())
    iNewPos = pSlider->GetRangeMin();

  if (iNewPos > pSlider->GetRangeMax())
    iNewPos = pSlider->GetRangeMax();

  switch (pSlider->GetDlgCtrlID())
  {
  case IDC_BARCODE_ORIENTATION :
    m_iOrientation = iNewPos;
    m_szOrientation.Format(_T("%d"), m_iOrientation);
    break;

  case IDC_BARCODE_SKEWNEGATIVE :
    m_iSkewNegative = iNewPos;
    m_szSkewNegative.Format(_T("%d"), m_iSkewNegative);
    break;

  case IDC_BARCODE_SKEWPOSITIVE :
    m_iSkewPositive = iNewPos;
    m_szSkewPositive.Format(_T("%d"), m_iSkewPositive);
    break;

  case IDC_BARCODE_THRESHOLD :
    m_iThreshold = iNewPos;
    m_szThreshold.Format(_T("%d"), m_iThreshold);
    break;
  }

  UpdateData (FALSE);
}

void SVBarCodeGeneralDialog::OnUpdateBarcodeOrientationEdit() 
{
	UpdateData (TRUE);
  m_iOrientation = atoi (m_szOrientation);
  if (m_iOrientation < 0)
  {
    m_iOrientation = 0;
    m_szOrientation = _T("0");
  }
  if (m_iOrientation > 360)
  {
    m_iOrientation = 360;
    m_szOrientation = _T("360");
  }
  UpdateData (FALSE);
}

void SVBarCodeGeneralDialog::OnUpdateBarcodeSkewNegativeEdit() 
{
	UpdateData (TRUE);
  m_iSkewNegative = atoi (m_szSkewNegative);
  if (m_iSkewNegative > 180)
  {
    m_iSkewNegative = 180;
    m_szSkewNegative = _T("180");
  }
  if (m_iSkewNegative < 0)
  {
    m_iSkewNegative = 0;
    m_szSkewNegative = _T("0");
  }
  UpdateData (FALSE);	
}

void SVBarCodeGeneralDialog::OnUpdateBarcodeSkewPositiveEdit() 
{
	UpdateData (TRUE);
  m_iSkewPositive = atoi (m_szSkewPositive);
  if (m_iSkewPositive > 180)
  {
    m_iSkewPositive = 180;
    m_szSkewPositive = _T("180");
  }
  if (m_iSkewPositive < 0)
  {
    m_iSkewPositive = 0;
    m_szSkewPositive = _T("0");
  }
  UpdateData (FALSE);	
}

void SVBarCodeGeneralDialog::OnUpdateBarcodeThresholdEdit() 
{
	UpdateData (TRUE);
  m_iThreshold = atoi (m_szThreshold);
  if (m_iThreshold > 255)
  {
    m_iThreshold = 255;
    m_szThreshold = _T("255");
  }
  if (m_iThreshold < 0)
  {
    m_iThreshold = 0;
    m_szThreshold = _T("0");
  }
  UpdateData (FALSE);	
}

void SVBarCodeGeneralDialog::OnEditChangeBarCodeStringSize() 
{
	UpdateData (TRUE);
}

void SVBarCodeGeneralDialog::OnSelChangeBarCodeStringSize() 
{
  UpdateData (TRUE);	
}

void SVBarCodeGeneralDialog::OnSelChangeBarCodeColor() 
{
	UpdateData (TRUE);
}

void SVBarCodeGeneralDialog::OnSelChangeSearchSpeed() 
{
	UpdateData (TRUE);	
}

void SVBarCodeGeneralDialog::OnSelChangeBarCodeType() 
{
	SVBarCodeProperties* pPropPage;
	DWORD dwEncoding=0;
	DWORD dwErrCorrection=0;
	
	UpdateData (TRUE);
	
	pPropPage = (SVBarCodeProperties*) GetParent();
	
	const SVBarCodeInfoStruct* pInfo = m_aBarCodeInfo.GetInfoByMilID( GetBarCodeType() );
	ASSERT( pInfo );
	if ( pInfo )
	{
		dwEncoding = pInfo->lDefaultEncoding;
		dwErrCorrection = pInfo->lDefaultErrorCorrection;
	}

	pPropPage->m_dlgBarCodeAttributes.SetEncoding(dwEncoding);
	pPropPage->m_dlgBarCodeAttributes.SetErrCorrection(dwErrCorrection);

	m_UnevenCheck.SetCheck(FALSE);

	//if Barcode type == DataMatrix enable uneven grid step
	if ( GetBarCodeType() == SVDataMatrix )
	{		
		GetDlgItem(IDC_CHK_UNEVEN_GRID)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHK_UNEVEN_GRID)->EnableWindow(FALSE);
	}


	// Threshold options limited depending on code.
	if( (GetBarCodeType() == SVPostNet) || (GetBarCodeType() == SVPlanet) )
	{
		switch( m_iThresholdRadio )
		{	// Adaptive does not work for postnet or planet.
			case SVBCThresholdAdaptive:
			{
				m_iThresholdRadio = SVBCThresholdAuto ;
			}
			case SVBCThresholdAuto:
			{
				GetDlgItem( IDC_BARCODE_THRESHOLD )->EnableWindow( FALSE );
				GetDlgItem( IDC_BARCODE_THRESHOLD_EDIT )->EnableWindow( FALSE );
				break;
			}
			case SVBCThresholdNormal:
			{
				GetDlgItem( IDC_BARCODE_THRESHOLD )->EnableWindow( TRUE );
				GetDlgItem( IDC_BARCODE_THRESHOLD_EDIT )->EnableWindow( TRUE );
				break;
			}
		}

		GetDlgItem( IDC_THRESHOLD_ADAPTIVE_RADIO )->EnableWindow( FALSE );
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem( IDC_THRESHOLD_ADAPTIVE_RADIO )->EnableWindow( TRUE );
	}

}

void SVBarCodeGeneralDialog::SetWarnOnFail(BOOL bWarnOnFail)
{
	m_bWarnOnFail = bWarnOnFail;
}

BOOL SVBarCodeGeneralDialog::GetWarnedOnFail()
{
	return m_bWarnOnFail;
}

long SVBarCodeGeneralDialog::SetTimeout( SVLongValueObjectClass &svlTimeout )
{
  long l_lTimeout;

  svlTimeout.GetValue(l_lTimeout);

  m_lTimeout = (int) l_lTimeout;
  return l_lTimeout;
}

long SVBarCodeGeneralDialog::GetTimeout()
{
	long lValue;

	lValue = m_lTimeout;

	return lValue;
}

void SVBarCodeGeneralDialog::OnDeltaposSpinTimeout(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);

	m_lTimeout = m_lTimeout + ( -1 * pNMUpDown->iDelta );
	
	// check to make sure value does not drop below 0
	if ( m_lTimeout < 1 )
	{
		m_lTimeout = 1;
	}

	if ( m_lTimeout > 10000 )
	{
		m_lTimeout = 10000;
	}

	//set the data in IPDoc.

	UpdateData(FALSE);	
	*pResult = 0;
}


void SVBarCodeGeneralDialog::OnBnClickedThresholdNormalRadio( UINT nID )
{
	UpdateData();
	switch( m_iThresholdRadio )
	{
		case SVBCThresholdNormal:
		{
			// Enable slider
			GetDlgItem( IDC_BARCODE_THRESHOLD )->EnableWindow( TRUE );
			GetDlgItem( IDC_BARCODE_THRESHOLD_EDIT )->EnableWindow( TRUE );
			break;
		}
		case SVBCThresholdAuto:
		case SVBCThresholdAdaptive:
		{
			// Disable slider
			GetDlgItem( IDC_BARCODE_THRESHOLD )->EnableWindow( FALSE );
			GetDlgItem( IDC_BARCODE_THRESHOLD_EDIT )->EnableWindow( FALSE );
			break;
		}
		default:
		{
			break;
		}
	}
}

void SVBarCodeGeneralDialog::OnBnClickedChkUnevenGrid()
{
	m_bUnEvenGrid = m_UnevenCheck.GetCheck();
}

void SVBarCodeGeneralDialog::UpdateUnEvenGrid()
{
	if ( GetBarCodeType() == SVDataMatrix )
	{
		m_UnevenCheck.SetCheck(m_bUnEvenGrid);
	}
	else
	{
		m_bUnEvenGrid = FALSE;
		m_UnevenCheck.SetCheck(m_bUnEvenGrid);
		m_UnevenCheck.EnableWindow(FALSE);
	}

}


