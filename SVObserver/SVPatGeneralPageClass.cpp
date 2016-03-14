//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatGeneralPageClass
//* .File Name       : $Workfile:   SVPatGeneralPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVPatGeneralPageClass.h"
#include "SVPatternAnalyzerClass.h"
#include "SVResult.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPatGeneralPageClass property page

IMPLEMENT_DYNCREATE(SVPatGeneralPageClass, CPropertyPage)

SVPatGeneralPageClass::SVPatGeneralPageClass() 
: CPropertyPage(SVPatGeneralPageClass::IDD), m_pPatAnalyzer(NULL)
{
	//{{AFX_DATA_INIT(SVPatGeneralPageClass)
	m_bAngleSearch = FALSE;
	m_dSearchAngle = 0.0;
	m_dAngleDeltaNegative = 0.0;
	m_dAngleDeltaPositive = 0.0;
	m_dAngleTolerance = 5.0;
	m_dAngularAccuracy = 0.1;
	m_lAcceptance = 0;
	m_lCertainty = 0;
	m_nOccurances = 0;
	m_bAccuracy = FALSE;
	//}}AFX_DATA_INIT
	m_dInterpolationMode = SVNearestNeighbor;
	m_nPosAccuracyIndex = 1; 
	m_nSpeedIndex = 3;
}

SVPatGeneralPageClass::~SVPatGeneralPageClass()
{
}

void SVPatGeneralPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatGeneralPageClass)
	DDX_Control(pDX, IDC_PAT_ANGULAR_ACCURACY_SLIDER, m_AngularAccuracySliderCtrl);
	DDX_Control(pDX, IDC_PAT_ANGLE_TOLERANCE_SLIDER, m_AngleToleranceSliderCtrl);
	DDX_Control(pDX, IDC_PAT_ANGLE_DELTA_POSITIVE_SLIDER, m_AngleDeltaPositiveSliderCtrl);
	DDX_Control(pDX, IDC_PAT_ANGLE_DELTA_NEGATIVE_SLIDER, m_AngleDeltaNegativeSliderCtrl);
	DDX_Control(pDX, IDC_PAT_SEARCH_ANGLE_SLIDER, m_SearchAngleSliderCtrl);
	DDX_Check(pDX, IDC_PAT_ANG_MODE, m_bAngleSearch);
	DDX_Text(pDX, IDC_PAT_SEARCH_ANGLE_VALUE, m_dSearchAngle);
	DDX_Text(pDX, IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE, m_dAngleDeltaNegative);
	DDX_Text(pDX, IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE, m_dAngleDeltaPositive);
	DDX_Text(pDX, IDC_PAT_ANGLE_TOLERANCE_VALUE, m_dAngleTolerance);
	DDX_Text(pDX, IDC_PAT_ANGULAR_ACCURACY_VALUE, m_dAngularAccuracy);
	DDX_Text(pDX, IDC_PAT_MDL_ACC_THRESHOLD, m_lAcceptance);
	DDX_Text(pDX, IDC_PAT_MDL_CERT_THRESHOLD, m_lCertainty);
	DDX_Text(pDX, IDC_PAT_OCCURANCE_NUMBER, m_nOccurances);
	DDX_Check(pDX, IDC_PAT_ANGULAR_ACCURACY, m_bAccuracy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPatGeneralPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPatGeneralPageClass)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_PAT_OCCURANCE_NUMBER, OnKillOccuranceNumber)
	ON_EN_KILLFOCUS(IDC_PAT_MDL_CERT_THRESHOLD, OnKillCertainty)
	ON_EN_KILLFOCUS(IDC_PAT_MDL_ACC_THRESHOLD, OnKillAcceptance)
	ON_EN_KILLFOCUS(IDC_PAT_SEARCH_ANGLE_VALUE, OnKillValue1)
	ON_EN_KILLFOCUS(IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE, OnKillValue2)
	ON_EN_KILLFOCUS(IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE, OnKillValue3)
	ON_EN_KILLFOCUS(IDC_PAT_ANGLE_TOLERANCE_VALUE, OnKillValue4)
	ON_EN_KILLFOCUS(IDC_PAT_ANGULAR_ACCURACY_VALUE, OnKillValue5)
	ON_BN_CLICKED(IDC_PAT_ANG_MODE, OnAngularSearch)
	ON_BN_CLICKED(IDC_PAT_ANGULAR_ACCURACY, OnAngAccuracy)
	ON_BN_CLICKED(IDC_PAT_ANG_INTERPOLE_MODE1, OnInterpoleMode1)
	ON_BN_CLICKED(IDC_PAT_ANG_INTERPOLE_MODE2, OnInterpoleMode2)
	ON_BN_CLICKED(IDC_PAT_ANG_INTERPOLE_MODE3, OnInterpoleMode3)
	ON_BN_CLICKED(IDC_PAT_SET_RANGE, OnSetRangeClicked)
	ON_CBN_SELCHANGE(IDC_PAT_POS_ACCURACY, OnSelchangePosAccuracy)
	ON_CBN_SELCHANGE(IDC_PAT_SPEED, OnSelchangeSpeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPatGeneralPageClass message handlers

void SVPatGeneralPageClass::OnCancel() 
{
}

void SVPatGeneralPageClass::OnOK() 
{
	CPropertyPage::OnOK();
}

void SVPatGeneralPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
	int iNewPos;

	iNewPos = pSlider->GetPos();

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
	case IDC_PAT_SEARCH_ANGLE_SLIDER :
		{
			m_dSearchAngle = ((double)iNewPos) / 10.0;
			CString		strPos;
			strPos.Format("%.1f", m_dSearchAngle);
			SetDlgItemText(IDC_PAT_SEARCH_ANGLE_VALUE, strPos);
		break;
		}
	case IDC_PAT_ANGLE_DELTA_NEGATIVE_SLIDER :
		{
			m_dAngleDeltaNegative = ((double)iNewPos) / 10.0;
			CString		strPos;
			strPos.Format("%.1f", m_dAngleDeltaNegative);
			SetDlgItemText(IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE, strPos);
		break;
		}
	case IDC_PAT_ANGLE_DELTA_POSITIVE_SLIDER :
		{
			m_dAngleDeltaPositive = ((double)iNewPos) / 10.0;
			CString		strPos;
			strPos.Format("%.1f", m_dAngleDeltaPositive);
			SetDlgItemText(IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE, strPos);
		break;
		}
	case IDC_PAT_ANGLE_TOLERANCE_SLIDER :
		{
			m_dAngleTolerance = ((double)iNewPos) / 10.0;
			CString		strPos;
			strPos.Format("%.1f", m_dAngleTolerance);
			SetDlgItemText(IDC_PAT_ANGLE_TOLERANCE_VALUE, strPos);
		break;
		}
	case IDC_PAT_ANGULAR_ACCURACY_SLIDER :
		{
			m_dAngularAccuracy = ((double)iNewPos) / 10.0;
			CString		strPos;
			strPos.Format("%.1f", m_dAngularAccuracy);
			SetDlgItemText(IDC_PAT_ANGULAR_ACCURACY_VALUE, strPos);
		break;
		}
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL SVPatGeneralPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pSheet = (SVPatAnalyzeSetupDlgSheet *)GetParent();

	m_SearchAngleSliderCtrl.SetRange(0, 3600, TRUE);
	m_SearchAngleSliderCtrl.SetPageSize(36);
	m_SearchAngleSliderCtrl.SetPos((int)(m_dSearchAngle* 10.0));
	m_AngleDeltaNegativeSliderCtrl.SetRange(0, 1800, TRUE);	
	m_AngleDeltaNegativeSliderCtrl.SetPageSize(18);
	m_AngleDeltaNegativeSliderCtrl.SetPos((int)(m_dAngleDeltaNegative * 10.0));
	m_AngleDeltaPositiveSliderCtrl.SetRange(0, 1800, TRUE);	
	m_AngleDeltaPositiveSliderCtrl.SetPageSize(18);
	m_AngleDeltaPositiveSliderCtrl.SetPos((int)(m_dAngleDeltaPositive * 10.0));
	m_AngleToleranceSliderCtrl.SetRange(1, 1800, TRUE);	
	m_AngleToleranceSliderCtrl.SetPageSize(18);
	m_AngleToleranceSliderCtrl.SetPos((int)(m_dAngleTolerance * 10.0));
	m_AngularAccuracySliderCtrl.SetRange(1, 1800, TRUE);	
	m_AngularAccuracySliderCtrl.SetPageSize(18);
	m_AngularAccuracySliderCtrl.SetPos((int)(m_dAngularAccuracy * 10.0));
	
	EnableAngleControls(m_bAngleSearch);

	EnableAngleAccuracyControls(m_bAccuracy);

	if(	m_dInterpolationMode == SVNearestNeighbor )
		((CButton *)GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE1))->SetCheck(1);
	else if(m_dInterpolationMode == SVBilinear)
		((CButton *)GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE2))->SetCheck(1);
	else
		((CButton *)GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE3))->SetCheck(1);

// Set Combo Box selections.
	((CComboBox *)GetDlgItem(IDC_PAT_POS_ACCURACY))->SetCurSel(m_nPosAccuracyIndex);
	((CComboBox *)GetDlgItem(IDC_PAT_SPEED))->SetCurSel(m_nSpeedIndex);
// Set limits for edit control Text
	SetControlTextLength(); 

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPatGeneralPageClass::SetControlTextLength() 
{
	((CEdit *)GetDlgItem(IDC_PAT_OCCURANCE_NUMBER))->SetLimitText(2);
	((CEdit *)GetDlgItem(IDC_PAT_MDL_CERT_THRESHOLD))->SetLimitText(3);
	((CEdit *)GetDlgItem(IDC_PAT_MDL_ACC_THRESHOLD))->SetLimitText(3);
	((CEdit *)GetDlgItem(IDC_PAT_SEARCH_ANGLE_VALUE))->SetLimitText(5);
	((CEdit *)GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE))->SetLimitText(5);
	((CEdit *)GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE))->SetLimitText(5);
	((CEdit *)GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_VALUE))->SetLimitText(5);
	((CEdit *)GetDlgItem(IDC_PAT_ANGULAR_ACCURACY_VALUE))->SetLimitText(5);
}

void SVPatGeneralPageClass::EnableAngleControls(BOOL bEnable) 
{
	GetDlgItem(IDC_PAT_SEARCH_ANGLE_TXT)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_TXT)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_TXT)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_TXT)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_PAT_SEARCH_ANGLE_SLIDER)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_SLIDER)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_SLIDER)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_SLIDER)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_PAT_SEARCH_ANGLE_VALUE)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_VALUE)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_PAT_SEARCH_ANGLE_MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_SEARCH_ANGLE_MAX)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_NEGATIVE_MAX)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_DELTA_POSITIVE_MAX)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGLE_TOLERANCE_MAX)->EnableWindow(bEnable);

	GetDlgItem(IDC_PAT_ANG_INTERPOLATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE1)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE2)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANG_INTERPOLE_MODE3)->EnableWindow(bEnable);

	GetDlgItem(IDC_PAT_ANGULAR_ACCURACY)->EnableWindow(bEnable);
}

void SVPatGeneralPageClass::EnableAngleAccuracyControls(BOOL bEnable) 
{
	GetDlgItem(IDC_PAT_ANGULAR_ACCURACY_SLIDER)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGULAR_ACCURACY_VALUE)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGULAR_ACCURACY_MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_PAT_ANGULAR_ACCURACY_MAX)->EnableWindow(bEnable);
}

void SVPatGeneralPageClass::OnKillOccuranceNumber() 
{
	ProcessOnKillfocus(IDC_PAT_OCCURANCE_NUMBER);
}

void SVPatGeneralPageClass::OnKillCertainty() 
{
	ProcessOnKillfocus(IDC_PAT_MDL_CERT_THRESHOLD);
}

void SVPatGeneralPageClass::OnKillAcceptance() 
{
	ProcessOnKillfocus(IDC_PAT_MDL_ACC_THRESHOLD);
}

void SVPatGeneralPageClass::OnKillValue1() 
{
	SetSliderPosition(IDC_PAT_SEARCH_ANGLE_VALUE);
}

void SVPatGeneralPageClass::OnKillValue2() 
{
	SetSliderPosition(IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE);
}

void SVPatGeneralPageClass::OnKillValue3() 
{
	SetSliderPosition(IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE);
}

void SVPatGeneralPageClass::OnKillValue4() 
{
	SetSliderPosition(IDC_PAT_ANGLE_TOLERANCE_VALUE);
}

void SVPatGeneralPageClass::OnKillValue5() 
{
	SetSliderPosition(IDC_PAT_ANGULAR_ACCURACY_VALUE);
}

void SVPatGeneralPageClass::OnAngularSearch() 
{
	m_bAngleSearch = (BOOL)((CButton *)GetDlgItem(IDC_PAT_ANG_MODE))->GetCheck();
	EnableAngleControls(m_bAngleSearch);
	if(!m_bAngleSearch)
	{
		((CButton *)GetDlgItem(IDC_PAT_ANGULAR_ACCURACY))->SetCheck(0);
		m_bAccuracy = FALSE;
		EnableAngleAccuracyControls(m_bAccuracy);
	}
}

void SVPatGeneralPageClass::OnAngAccuracy() 
{
	m_bAccuracy = (BOOL)((CButton *)GetDlgItem(IDC_PAT_ANGULAR_ACCURACY))->GetCheck();
	EnableAngleAccuracyControls(m_bAccuracy);
}

BOOL SVPatGeneralPageClass::OnKillActive() 
{
	UINT IdList[3] = {IDC_PAT_MDL_ACC_THRESHOLD, IDC_PAT_MDL_CERT_THRESHOLD, 
					IDC_PAT_OCCURANCE_NUMBER};
	for(int i = 0; i < 3; i++)
		if(!ProcessOnKillfocus(IdList[i]))
			return FALSE;
	if(m_bAngleSearch)
	{
		UINT IdAngleValue[5] = {IDC_PAT_SEARCH_ANGLE_VALUE, IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE, 
						IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE, IDC_PAT_ANGLE_TOLERANCE_VALUE, IDC_PAT_ANGULAR_ACCURACY_VALUE};
		for( int i = 0; i < 5; i++)
			SetSliderPosition(IdAngleValue[i]);
	}
	return CPropertyPage::OnKillActive();
}

BOOL SVPatGeneralPageClass::ProcessOnKillfocus(UINT nId) 
{
	if(GetActiveWindow() != (CWnd *)m_pSheet)
		return TRUE;

	CString		strValue;
	GetDlgItemText(nId, strValue);
	switch(nId)
	{
		case IDC_PAT_OCCURANCE_NUMBER:
		{
			long	lSize = atol(strValue.Left(10));
			if(lSize < 2)
				SetDlgItemText(nId, "1");
			break;
		}
		case IDC_PAT_MDL_ACC_THRESHOLD:
		case IDC_PAT_MDL_CERT_THRESHOLD:
		{
			double	dThreshold = atof(strValue.Left(10));
			if(dThreshold > 100 || strValue.IsEmpty())
			{
				AfxMessageBox(IDS_PAT_THRESHOLD_ERROR);
				GetDlgItem(nId)->SetFocus();
				((CEdit *)GetDlgItem(nId))->SetSel(0, -1);
				return FALSE;
			}
		}
	}
		
	return TRUE;
}

void SVPatGeneralPageClass::SetSliderPosition(UINT nId)
{
	CString		strValue;
	GetDlgItemText(nId, strValue);
	double	dValue = atof(strValue.Left(10));
	dValue *= 10.0;

	CSliderCtrl	*pSlider;

	switch (nId)
	{
		case IDC_PAT_SEARCH_ANGLE_VALUE:
			pSlider	= (CSliderCtrl	*)&m_SearchAngleSliderCtrl;
			break;
		case IDC_PAT_ANGLE_DELTA_NEGATIVE_VALUE:
			pSlider	= (CSliderCtrl	*)&m_AngleDeltaNegativeSliderCtrl;
			break;
		case IDC_PAT_ANGLE_DELTA_POSITIVE_VALUE:
			pSlider	= (CSliderCtrl	*)&m_AngleDeltaPositiveSliderCtrl;
			break;
		case IDC_PAT_ANGLE_TOLERANCE_VALUE:
			pSlider	= (CSliderCtrl	*)&m_AngleToleranceSliderCtrl;
			break;
		case IDC_PAT_ANGULAR_ACCURACY_VALUE:
			pSlider	= (CSliderCtrl	*)&m_AngularAccuracySliderCtrl;
			break;
	}

	int	nPos = (int)dValue;
	if (nPos < pSlider->GetRangeMin())
		nPos = pSlider->GetRangeMin();

	if (nPos > pSlider->GetRangeMax())
		nPos = pSlider->GetRangeMax();
	
	dValue = ((double)nPos) / 10.0;
	strValue.Format("%.1f", dValue);
	SetDlgItemText(nId, strValue);
	pSlider->SetPos(nPos);
}

void SVPatGeneralPageClass::OnInterpoleMode1() 
{
	m_dInterpolationMode = SVNearestNeighbor;
}

void SVPatGeneralPageClass::OnInterpoleMode2() 
{
	m_dInterpolationMode = SVBilinear; // (double)M_BILINEAR;
}

void SVPatGeneralPageClass::OnInterpoleMode3() 
{
	m_dInterpolationMode = SVBiCubic; // (double)M_BICUBIC;
}

void SVPatGeneralPageClass::OnSelchangePosAccuracy() 
{
	int	nSel = ((CComboBox *)GetDlgItem(IDC_PAT_POS_ACCURACY))->GetCurSel();
	if(nSel != CB_ERR)
		m_nPosAccuracyIndex = nSel;
}

void SVPatGeneralPageClass::OnSelchangeSpeed() 
{
	int	nSel = ((CComboBox *)GetDlgItem(IDC_PAT_SPEED))->GetCurSel();
	if(nSel != CB_ERR)
		m_nSpeedIndex = nSel;
}

void SVPatGeneralPageClass::OnSetRangeClicked()
{
	// Show result dlg
	if ( m_pPatAnalyzer )
	{
		SVResultClass* pAnalyzerResult = m_pPatAnalyzer->GetResultObject();

		if (pAnalyzerResult)
		{
			SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), this );
		}
	}
}

