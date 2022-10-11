//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/SVMatroxSimpleEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVBarCodeGeneralDialog::SVBarCodeGeneralDialog(CWnd* /*=nullptr*/)
		: CPropertyPage(SVBarCodeGeneralDialog::IDD)
		, m_iThresholdRadio(0)
		, m_lInitialBarCodeType{ SVDataMatrix }
	{
		//{{AFX_DATA_INIT(SVBarCodeGeneralDialog)
		//}}AFX_DATA_INIT
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
		ON_CBN_SELCHANGE(IDC_STRING_FORMAT, OnSelChangeStringFormat)
		ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIMEOUT, OnDeltaposSpinTimeout)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_THRESHOLD_NORMAL_RADIO, IDC_THRESHOLD_ADAPTIVE_RADIO, &SVBarCodeGeneralDialog::OnBnClickedThresholdNormalRadio)
		//}}AFX_MSG_MAP
		ON_BN_CLICKED(IDC_CHK_UNEVEN_GRID, &SVBarCodeGeneralDialog::OnBnClickedChkUnevenGrid)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVBarCodeGeneralDialog message handlers



	long SVBarCodeGeneralDialog::GetBarCodeType()
	{
		if (m_iBarCodeType != CB_ERR)
		{
			const auto& rInfo = m_barCodeTypeInfos.typeparameters(m_iBarCodeType);
			return rInfo.type();
		}

		return 0;
	}

	void SVBarCodeGeneralDialog::SetBarCodeType(long barCodeType)
	{
		m_lInitialBarCodeType = barCodeType;
		m_iBarCodeType = GetIndexByMilID(barCodeType);
	}

	double SVBarCodeGeneralDialog::GetBarCodeStringSize()
	{
		if (!m_szStringSize.Compare(_T("<Any>")))
			return (double)SVValueDefault;
		return (double)atol(m_szStringSize);
	}

	void SVBarCodeGeneralDialog::SetBarCodeStringSize(long stringSize)
	{
		if (stringSize == SVValueDefault)
		{
			m_szStringSize = _T("<Any>");
		}
		else
		{
			m_szStringSize.Format(_T("%ld"), stringSize);
		}
	}

	double SVBarCodeGeneralDialog::GetBarCodeSearchSpeed()
	{
		switch (m_iSearchSpeed)
		{
		case 0:
			return (double)SVValueVeryLow;
			break;

		case 1:
			return (double)SVValueLow;
			break;

		case 2:
			return (double)SVValueMedium;
			break;

		case 3:
			return (double)SVValueHigh;
			break;

		case 4:
			return (double)SVValueVeryHigh;
			break;

		default:
			return 0.0;
			break;

		}
	}

	void SVBarCodeGeneralDialog::SetBarCodeSearchSpeed(long searchSpeed)
	{
		switch (searchSpeed)
		{
		case  SVValueVeryLow:
			m_iSearchSpeed = 0;
			break;

		case SVValueLow:
			m_iSearchSpeed = 1;
			break;

		case SVValueMedium:
			m_iSearchSpeed = 2;
			break;

		case SVValueHigh:
			m_iSearchSpeed = 3;
			break;

		case SVValueVeryHigh:
			m_iSearchSpeed = 4;
			break;

		default:
			break;
		}
	}

	double SVBarCodeGeneralDialog::GetForegroundColor()
	{
		switch (m_iBarCodeColor)
		{
		case 0:
			return (double)SVValueForegroundBlack;
			break;

		case 1:
			return (double)SVValueForegroundWhite;
			break;

		default:
			return (double)SVValueForegroundBlack;
			break;
		}
	}

	void SVBarCodeGeneralDialog::SetForegroundColor(long foregroundColor)
	{
		switch (foregroundColor)
		{
		case SVValueForegroundBlack:
			m_iBarCodeColor = 0;
			break;

		case SVValueForegroundWhite:
			m_iBarCodeColor = 1;
			break;

		default:
			m_iBarCodeColor = 0;
			break;
		}
	}

	double SVBarCodeGeneralDialog::GetOrientation()
	{
		return (double)m_iOrientation;
	}

	void SVBarCodeGeneralDialog::SetOrientation(long orientation)
	{
		if (orientation == SVValueDefault)
		{
			m_iOrientation = 0;
		}
		else
		{
			m_iOrientation = orientation;
		}
		
		m_szOrientation.Format("%d", m_iOrientation);
	}

	double SVBarCodeGeneralDialog::GetSkewNegative()
	{
		return (double)m_iSkewNegative;
	}

	void SVBarCodeGeneralDialog::SetSkewNegative(long skewNegative)
	{
		if (skewNegative == SVValueDefault)
		{
			skewNegative = 0;
		}

		m_iSkewNegative = skewNegative;
		m_szSkewNegative.Format("%ld", skewNegative);
	}

	double SVBarCodeGeneralDialog::GetSkewPositive()
	{
		return (double)m_iSkewPositive;
	}

	void SVBarCodeGeneralDialog::SetSkewPositive(long skewPositive)
	{
		if (skewPositive == SVValueDefault)
		{
			m_iSkewPositive = 0;
		}
		else
		{
			m_iSkewPositive = skewPositive;
		}
		m_szSkewPositive.Format("%ld", skewPositive);
	}

	double SVBarCodeGeneralDialog::GetThreshold()
	{
		return (double)m_iThreshold;
	}

	void SVBarCodeGeneralDialog::SetThreshold(long threshold)
	{
		if (threshold == SVValueDefault)
		{
			m_iThreshold = 0;
		}
		else
		{
			m_iThreshold = threshold;
		}
		m_szThreshold.Format("%d", m_iThreshold);
	}

	void SVBarCodeGeneralDialog::SetBarcodeStringFormat(SvDef::NameValueVector&& rStringFormatList, long stringFormatPos)
	{
		m_stringFormatList = std::move(rStringFormatList);
		m_stringFormatPos = stringFormatPos;
	}

	CString SVBarCodeGeneralDialog::GetBarcodeStringFormat()
	{
		return m_StringFormat;
	}

	void SVBarCodeGeneralDialog::SetBarcodeThresholdType(long thresholdType)
	{
		if (thresholdType > SVBCThresholdAdaptive)
		{
			m_iThresholdRadio = SVBCThresholdNormal;
		}
		else
		{
			m_iThresholdRadio = thresholdType;
		}
	}
	int SVBarCodeGeneralDialog::GetBarcodeThresholdType()
	{
		return m_iThresholdRadio;
	}

	void SVBarCodeGeneralDialog::SetUnEvenGrid(bool unEvenGrid)
	{
		m_bUnEvenGrid = unEvenGrid;
	}

	BOOL SVBarCodeGeneralDialog::GetUnEvenGrid()
	{
		return m_bUnEvenGrid;
	}

	BOOL SVBarCodeGeneralDialog::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_StringFormatCombo.SetEnumTypes(m_stringFormatList);
		m_StringFormatCombo.SetCurSelItemData(m_stringFormatPos);
		m_StringFormatCombo.GetLBText(m_stringFormatPos, m_StringFormat);

		for (const auto& rInfo : m_barCodeTypeInfos.typeparameters())
		{
			int iIndex = m_cbBarcodeType.AddString(rInfo.name().c_str());
			m_cbBarcodeType.SetItemData(iIndex, static_cast<DWORD_PTR>(rInfo.type()));
		}
		m_iBarCodeType = GetIndexByMilID(m_lInitialBarCodeType);

		CSliderCtrl *pControl;

		pControl = (CSliderCtrl *)GetDlgItem(IDC_BARCODE_ORIENTATION);
		pControl->SetRangeMin(0, FALSE);
		pControl->SetRangeMax(360, TRUE);

		pControl = (CSliderCtrl *)GetDlgItem(IDC_BARCODE_SKEWNEGATIVE);
		pControl->SetRangeMin(0, FALSE);
		pControl->SetRangeMax(180, TRUE);

		pControl = (CSliderCtrl *)GetDlgItem(IDC_BARCODE_SKEWPOSITIVE);
		pControl->SetRangeMin(0, FALSE);
		pControl->SetRangeMax(180, TRUE);

		// Threshold...
		pControl = (CSliderCtrl *)GetDlgItem(IDC_BARCODE_THRESHOLD);
		pControl->SetRangeMin(0, FALSE);
		pControl->SetRangeMax(255, TRUE);

		m_spnTimeout.SetBuddy(GetWindow(IDC_EDT_TIMEOUT));

		UpdateData(FALSE);

		OnBnClickedThresholdNormalRadio(0);

		UpdateUnEvenGrid();

		return TRUE;
	}

	void SVBarCodeGeneralDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

		CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
		int iNewPos;

		iNewPos = pSlider->GetPos();

		switch (nSBCode)
		{
		case SB_LEFT:
			iNewPos = pSlider->GetRangeMin();
			break;

		case SB_ENDSCROLL:
			break;

		case SB_LINELEFT:
			iNewPos--;
			break;

		case SB_LINERIGHT:
			iNewPos++;
			break;

		case SB_PAGELEFT:
			iNewPos -= 10;
			break;

		case SB_PAGERIGHT:
			iNewPos += 10;
			break;

		case SB_RIGHT:
			iNewPos = pSlider->GetRangeMax();
			break;

		case SB_THUMBPOSITION:
			iNewPos = nPos;
			break;

		case SB_THUMBTRACK:
			iNewPos = nPos;
			break;
		}
		if (iNewPos < pSlider->GetRangeMin())
			iNewPos = pSlider->GetRangeMin();

		if (iNewPos > pSlider->GetRangeMax())
			iNewPos = pSlider->GetRangeMax();

		switch (pSlider->GetDlgCtrlID())
		{
		case IDC_BARCODE_ORIENTATION:
			m_iOrientation = iNewPos;
			m_szOrientation.Format(_T("%d"), m_iOrientation);
			break;

		case IDC_BARCODE_SKEWNEGATIVE:
			m_iSkewNegative = iNewPos;
			m_szSkewNegative.Format(_T("%d"), m_iSkewNegative);
			break;

		case IDC_BARCODE_SKEWPOSITIVE:
			m_iSkewPositive = iNewPos;
			m_szSkewPositive.Format(_T("%d"), m_iSkewPositive);
			break;

		case IDC_BARCODE_THRESHOLD:
			m_iThreshold = iNewPos;
			m_szThreshold.Format(_T("%d"), m_iThreshold);
			break;
		}

		UpdateData(FALSE);
	}

	void SVBarCodeGeneralDialog::OnUpdateBarcodeOrientationEdit()
	{
		UpdateData(TRUE);
		m_iOrientation = atoi(m_szOrientation);
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
		UpdateData(FALSE);
	}

	void SVBarCodeGeneralDialog::OnUpdateBarcodeSkewNegativeEdit()
	{
		UpdateData(TRUE);
		m_iSkewNegative = atoi(m_szSkewNegative);
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
		UpdateData(FALSE);
	}

	void SVBarCodeGeneralDialog::OnUpdateBarcodeSkewPositiveEdit()
	{
		UpdateData(TRUE);
		m_iSkewPositive = atoi(m_szSkewPositive);
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
		UpdateData(FALSE);
	}

	void SVBarCodeGeneralDialog::OnUpdateBarcodeThresholdEdit()
	{
		UpdateData(TRUE);
		m_iThreshold = atoi(m_szThreshold);
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
		UpdateData(FALSE);
	}

	void SVBarCodeGeneralDialog::OnEditChangeBarCodeStringSize()
	{
		UpdateData(TRUE);
	}

	void SVBarCodeGeneralDialog::OnSelChangeBarCodeStringSize()
	{
		UpdateData(TRUE);
	}

	void SVBarCodeGeneralDialog::OnSelChangeStringFormat()
	{
		int CurrentSelection = m_StringFormatCombo.GetCurSel();
		if (0 <= CurrentSelection)
		{
			m_StringFormatCombo.GetLBText(CurrentSelection, m_StringFormat);
		}
	}

	void SVBarCodeGeneralDialog::OnSelChangeBarCodeColor()
	{
		UpdateData(TRUE);
	}

	void SVBarCodeGeneralDialog::OnSelChangeSearchSpeed()
	{
		UpdateData(TRUE);
	}

	void SVBarCodeGeneralDialog::OnSelChangeBarCodeType()
	{
		DWORD dwEncoding = 0;
		DWORD dwErrCorrection = 0;

		UpdateData(TRUE);

		SVBarCodeProperties* pPropPage = static_cast<SVBarCodeProperties*>(GetParent());

		const auto* pInfo = GetInfoByMilID(GetBarCodeType());
		assert(pInfo);
		if (pInfo)
		{
			dwEncoding = pInfo->defaultenctype();
			dwErrCorrection = pInfo->defaultecctype();
		}

		pPropPage->m_dlgBarCodeAttributes.SetEncoding(dwEncoding);
		pPropPage->m_dlgBarCodeAttributes.SetErrCorrection(dwErrCorrection);

		m_UnevenCheck.SetCheck(FALSE);

		//if Barcode type == DataMatrix enable uneven grid step
		if (GetBarCodeType() == SVDataMatrix || SVQRCode == GetBarCodeType())
		{
			GetDlgItem(IDC_CHK_UNEVEN_GRID)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHK_UNEVEN_GRID)->EnableWindow(FALSE);
		}


		// Threshold options limited depending on code.
		if ((GetBarCodeType() == SVPostNet) || (GetBarCodeType() == SVPlanet))
		{
			switch (m_iThresholdRadio)
			{	// Adaptive does not work for postnet or planet.
			case SVBCThresholdAdaptive:
			{
				m_iThresholdRadio = SVBCThresholdAuto;
			}
			case SVBCThresholdAuto:
			{
				GetDlgItem(IDC_BARCODE_THRESHOLD)->EnableWindow(FALSE);
				GetDlgItem(IDC_BARCODE_THRESHOLD_EDIT)->EnableWindow(FALSE);
				break;
			}
			case SVBCThresholdNormal:
			{
				GetDlgItem(IDC_BARCODE_THRESHOLD)->EnableWindow(TRUE);
				GetDlgItem(IDC_BARCODE_THRESHOLD_EDIT)->EnableWindow(TRUE);
				break;
			}
			}

			GetDlgItem(IDC_THRESHOLD_ADAPTIVE_RADIO)->EnableWindow(FALSE);
			UpdateData(FALSE);
		}
		else
		{
			GetDlgItem(IDC_THRESHOLD_ADAPTIVE_RADIO)->EnableWindow(TRUE);
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

	void SVBarCodeGeneralDialog::SetTimeout(long lTimeout)
	{
		m_lTimeout = lTimeout;
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

		m_lTimeout = m_lTimeout + (-1 * pNMUpDown->iDelta);

		// check to make sure value does not drop below 0
		if (m_lTimeout < 1)
		{
			m_lTimeout = 1;
		}

		if (m_lTimeout > 10000)
		{
			m_lTimeout = 10000;
		}

		//set the data in IPDoc.

		UpdateData(FALSE);
		*pResult = 0;
	}


void SVBarCodeGeneralDialog::OnBnClickedThresholdNormalRadio(UINT)
	{
		UpdateData();
		switch (m_iThresholdRadio)
		{
		case SVBCThresholdNormal:
		{
			// Enable slider
			GetDlgItem(IDC_BARCODE_THRESHOLD)->EnableWindow(TRUE);
			GetDlgItem(IDC_BARCODE_THRESHOLD_EDIT)->EnableWindow(TRUE);
			break;
		}
		case SVBCThresholdAuto:
		case SVBCThresholdAdaptive:
		{
			// Disable slider
			GetDlgItem(IDC_BARCODE_THRESHOLD)->EnableWindow(FALSE);
			GetDlgItem(IDC_BARCODE_THRESHOLD_EDIT)->EnableWindow(FALSE);
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
		m_bUnEvenGrid = m_UnevenCheck.GetCheck() ? true : false;
	}

	void SVBarCodeGeneralDialog::UpdateUnEvenGrid()
	{
		if (GetBarCodeType() == SVDataMatrix || SVQRCode == GetBarCodeType())
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

	const SvPb::BarCodeTypeParameter* SVBarCodeGeneralDialog::GetInfoByMilID(long MilID) const
	{
		auto iter = std::find_if(m_barCodeTypeInfos.typeparameters().cbegin(), m_barCodeTypeInfos.typeparameters().cend(), [MilID](const auto& rEntry) { return rEntry.type() == MilID; });
		if (iter != m_barCodeTypeInfos.typeparameters().cend())
		{
			return &*iter;
		}

		assert(false);
		return nullptr;
	}

	long SVBarCodeGeneralDialog::GetIndexByMilID(long MilID) const
	{
		auto iter = std::find_if(m_barCodeTypeInfos.typeparameters().cbegin(), m_barCodeTypeInfos.typeparameters().cend(), [MilID](const auto& rEntry) { return rEntry.type() == MilID; });
		auto index = std::distance(m_barCodeTypeInfos.typeparameters().cbegin(), iter);
		if (m_barCodeTypeInfos.typeparameters().cend() != iter)
		{
			return static_cast<long>(index);
		}
		else
		{
			return -1;
		}
	}
}