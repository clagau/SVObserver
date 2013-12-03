//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatAdvancedPageClass
//* .File Name       : $Workfile:   SVPatAdvancedPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:01:54  $
//******************************************************************************

#include "stdafx.h"
#include <cmath>
#include <cstdlib>
#include "SVPatAdvancedPageClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const LPCTSTR scAUTO = _T("Auto");
static const LPCTSTR scENABLE = _T("Enable");
static const LPCTSTR scDISABLE = _T("Disable");

IMPLEMENT_DYNCREATE(SVPatAdvancedPageClass, CPropertyPage)

SVPatAdvancedPageClass::SVPatAdvancedPageClass() 
: CPropertyPage(SVPatAdvancedPageClass::IDD)
{
	m_lAdditionalCandidates = 1;
	m_dCandidateSpacingXMin = 1.0;
	m_dCandidateSpacingYMin = 1.0;
	m_dPreliminaryAcceptanceThreshold = 1.0;

	//{{AFX_DATA_INIT(SVPatAdvancedPageClass)
	m_AdditionalCandidatesStr = scAUTO;
	m_CandidateSpacingXMinStr = scAUTO;
	m_CandidateSpacingYMinStr = scAUTO;
	m_PreliminaryAcceptanceThresholdStr = scAUTO;
	//}}AFX_DATA_INIT
}

SVPatAdvancedPageClass::~SVPatAdvancedPageClass()
{
}

void SVPatAdvancedPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatAdvancedPageClass)
	DDX_Control(pDX, IDC_PAT_FAST_FIND, m_FastFindComboBox);
	DDX_Control(pDX, IDC_PAT_MODEL_STEP, m_ModelStepComboBox);
	DDX_Control(pDX, IDC_PAT_BEGINNING_RESOLUTION_LEVEL, m_BeginningResolutionLevelComboBox);
	DDX_Control(pDX, IDC_PAT_FINAL_RESOLUTION_LEVEL, m_FinalResolutionLevelComboBox);
	DDX_Control(pDX, IDC_PAT_ADDITIONAL_CANDIDATES_AUTO, m_AdditionalCandidatesAutoCheckBox);
	DDX_Control(pDX, IDC_PAT_CANDIDATE_SPACING_XMIN_AUTO, m_CandidateSpacingXMinAutoCheckBox);
	DDX_Control(pDX, IDC_PAT_CANDIDATE_SPACING_YMIN_AUTO, m_CandidateSpacingYMinAutoCheckBox);
	DDX_Control(pDX, IDC_PAT_ACCEPTANCE_THRESHOLD_AUTO, m_PreliminaryAcceptanceThresholdAutoCheckBox);
	DDX_Control(pDX, IDC_PAT_ACCEPTANCE_THRESHOLD_SLIDER, m_PreliminaryAcceptanceThresholdSliderCtrl);
	DDX_Control(pDX, IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE, m_AcceptanceThresholdEditCtrl);
	DDX_Control(pDX, IDC_PAT_ADDITIONAL_CANDIDATES_VALUE, m_AdditionalCandidatesEditCtrl);
	DDX_Control(pDX, IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE, m_CandidateSpacingXMinEditCtrl);
	DDX_Control(pDX, IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE, m_CandidateSpacingYMinEditCtrl);
	DDX_Text(pDX, IDC_PAT_ADDITIONAL_CANDIDATES_VALUE, m_AdditionalCandidatesStr);
	DDX_Text(pDX, IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE, m_CandidateSpacingXMinStr);
	DDX_Text(pDX, IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE, m_CandidateSpacingYMinStr);
	DDX_Text(pDX, IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE, m_PreliminaryAcceptanceThresholdStr);
	//}}AFX_DATA_MAP
}

void SVPatAdvancedPageClass::InitializeControlValues()
{
	// populate Fast Find ComboBox
	int index = m_FastFindComboBox.AddString(scENABLE);
	m_FastFindComboBox.SetItemData(index, SVValueEnable);
	index = m_FastFindComboBox.AddString(scDISABLE);
	m_FastFindComboBox.SetItemData(index, SVValueDisable);
	index = m_FastFindComboBox.AddString(scAUTO);
	m_FastFindComboBox.SetItemData(index, SVValueDefault);

	// Set Selected Item
	SetSelection(m_FastFindComboBox, m_lFastFind);

	// populate ModelStep ComboBox
	m_ModelStepComboBox.ResetContent();
	index = m_ModelStepComboBox.AddString(_T("1"));
	m_ModelStepComboBox.SetItemData(index, 1);
	index = m_ModelStepComboBox.AddString(_T("2"));
	m_ModelStepComboBox.SetItemData(index, 2);
	index = m_ModelStepComboBox.AddString(scAUTO);
	m_ModelStepComboBox.SetItemData(index, SVValueDefault);

	// Set Selected Item
	SetSelection(m_ModelStepComboBox, m_lModelStep);

	// populate Beginning and Final Resolution Level ComboBoxes
	m_BeginningResolutionLevelComboBox.ResetContent();
	m_FinalResolutionLevelComboBox.ResetContent();
	for (int i = 0;i <= 7;i++)
	{
		CString tmp;
		tmp.Format(_T("%d"), i);

		index = m_BeginningResolutionLevelComboBox.AddString(tmp);
		m_BeginningResolutionLevelComboBox.SetItemData(index, i);

		index = m_FinalResolutionLevelComboBox.AddString(tmp);
		m_FinalResolutionLevelComboBox.SetItemData(index, i);
	}
	index = m_BeginningResolutionLevelComboBox.AddString(scAUTO);
	m_BeginningResolutionLevelComboBox.SetItemData(index, SVValueDefault);
	index = m_FinalResolutionLevelComboBox.AddString(scAUTO);
	m_FinalResolutionLevelComboBox.SetItemData(index, SVValueDefault);

	// Set Selected Item
	SetSelection(m_BeginningResolutionLevelComboBox, m_lBeginningResolutionLevel);
	SetSelection(m_FinalResolutionLevelComboBox, m_lFinalResolutionLevel);
	
	// 
	SetControlTextLength();

	// Update CheckBoxes
	SetInitialCheckBoxesState();

	// Update Sliders
	m_PreliminaryAcceptanceThresholdSliderCtrl.SetRange(1, 100);
	m_PreliminaryAcceptanceThresholdSliderCtrl.SetTicFreq(10);

	if (m_dPreliminaryAcceptanceThreshold != SVValueDefault)
	{
		m_PreliminaryAcceptanceThresholdSliderCtrl.SetPos(static_cast<int>(m_dPreliminaryAcceptanceThreshold));
	}
	else
	{
		m_PreliminaryAcceptanceThresholdSliderCtrl.SetPos(1);
	}

	// Update Edit Strings
	UpdateAdditionalCandidatesString();
	UpdateCandidateSpacingXMinString();
	UpdateCandidateSpacingYMinString();
	UpdatePreliminaryAcceptanceThresholdString();

	//
	EnablePreliminaryAcceptanceThresholdEdit();
	EnableAdditionalCandidatesEdit();
	EnableCandidateSpacingXMinEdit();
	EnableCandidateSpacingYMinEdit();
}

void SVPatAdvancedPageClass::SetSelection(CComboBox& rComboBox, long value)
{
	// set selection based on value matched against item data
	for (int index = 0;index < rComboBox.GetCount();index++)
	{
		long data = static_cast<int>(rComboBox.GetItemData(index));
		if (data == value)
		{
			rComboBox.SetCurSel(index);
		}
	}
}

void SVPatAdvancedPageClass::SetControlTextLength() 
{
	m_AcceptanceThresholdEditCtrl.SetLimitText(3);
	m_AdditionalCandidatesEditCtrl.SetLimitText(3);
	m_CandidateSpacingXMinEditCtrl.SetLimitText(6);
	m_CandidateSpacingYMinEditCtrl.SetLimitText(6);
}

void SVPatAdvancedPageClass::SetInitialCheckBoxesState() 
{
	bool bChecked = (m_lAdditionalCandidates == SVValueDefault);
	m_AdditionalCandidatesAutoCheckBox.SetCheck((bChecked) ? BST_CHECKED : BST_UNCHECKED);

	bChecked = (m_dCandidateSpacingXMin == SVValueDefault);
	m_CandidateSpacingXMinAutoCheckBox.SetCheck((bChecked) ? BST_CHECKED : BST_UNCHECKED);

	bChecked = (m_dCandidateSpacingYMin == SVValueDefault);
	m_CandidateSpacingYMinAutoCheckBox.SetCheck((bChecked) ? BST_CHECKED : BST_UNCHECKED);

	bChecked = (m_dPreliminaryAcceptanceThreshold == SVValueDefault);
	m_PreliminaryAcceptanceThresholdAutoCheckBox.SetCheck((bChecked) ? BST_CHECKED : BST_UNCHECKED);
}

void SVPatAdvancedPageClass::UpdateAdditionalCandidatesString()
{
	bool bEnable = (m_lAdditionalCandidates != SVValueDefault);
	if (bEnable)
	{
		m_AdditionalCandidatesStr.Format(_T("%d"), m_lAdditionalCandidates);
	}
	else
	{
		m_AdditionalCandidatesStr = scAUTO;
	}
}

void SVPatAdvancedPageClass::UpdateCandidateSpacingXMinString()
{
	bool bEnable = (m_dCandidateSpacingXMin != SVValueDefault);
	if (bEnable)
	{
		m_CandidateSpacingXMinStr.Format(_T("%4.1lf"), m_dCandidateSpacingXMin);
	}
	else
	{
		m_CandidateSpacingXMinStr = scAUTO;
	}
}

void SVPatAdvancedPageClass::UpdateCandidateSpacingYMinString()
{
	bool bEnable = (m_dCandidateSpacingYMin != SVValueDefault);
	if (bEnable)
	{
		m_CandidateSpacingYMinStr.Format(_T("%4.1lf"), m_dCandidateSpacingYMin);
	}
	else
	{
		m_CandidateSpacingYMinStr = scAUTO;
	}
}

void SVPatAdvancedPageClass::UpdatePreliminaryAcceptanceThresholdString()
{
	bool bEnable = (m_dPreliminaryAcceptanceThreshold != SVValueDefault);
	if (bEnable)
	{
		m_PreliminaryAcceptanceThresholdStr.Format(_T("%d"), static_cast<long>(m_dPreliminaryAcceptanceThreshold));
	}
	else
	{
		m_PreliminaryAcceptanceThresholdStr = scAUTO;
	}
}

void SVPatAdvancedPageClass::EnablePreliminaryAcceptanceThresholdEdit()
{
	bool bEnable = (m_dPreliminaryAcceptanceThreshold != SVValueDefault);
	m_PreliminaryAcceptanceThresholdSliderCtrl.EnableWindow(bEnable);
	m_AcceptanceThresholdEditCtrl.EnableWindow(bEnable);
}

void SVPatAdvancedPageClass::EnableAdditionalCandidatesEdit()
{
	bool bEnable = (m_lAdditionalCandidates != SVValueDefault);
	m_AdditionalCandidatesEditCtrl.EnableWindow(bEnable);
}

void SVPatAdvancedPageClass::EnableCandidateSpacingXMinEdit()
{
	bool bEnable = (m_dCandidateSpacingXMin != SVValueDefault);
	m_CandidateSpacingXMinEditCtrl.EnableWindow(bEnable);
}

void SVPatAdvancedPageClass::EnableCandidateSpacingYMinEdit()
{
	bool bEnable = (m_dCandidateSpacingYMin != SVValueDefault);
	m_CandidateSpacingYMinEditCtrl.EnableWindow(bEnable);
}

BEGIN_MESSAGE_MAP(SVPatAdvancedPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPatAdvancedPageClass)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PAT_ACCEPTANCE_THRESHOLD_AUTO, OnAcceptanceThresholdAutoClicked)
	ON_BN_CLICKED(IDC_PAT_ADDITIONAL_CANDIDATES_AUTO, OnAdditionalCandidatesAutoClicked)
	ON_BN_CLICKED(IDC_PAT_CANDIDATE_SPACING_XMIN_AUTO, OnCandidateSpacingXMinAutoClicked)
	ON_BN_CLICKED(IDC_PAT_CANDIDATE_SPACING_YMIN_AUTO, OnCandidateSpacingYMinAutoClicked)
	ON_CBN_SELCHANGE(IDC_PAT_FAST_FIND, OnSelchangeFastFind)
	ON_CBN_SELCHANGE(IDC_PAT_MODEL_STEP, OnSelchangeModelStep)
	ON_CBN_SELCHANGE(IDC_PAT_BEGINNING_RESOLUTION_LEVEL, OnSelchangeBeginningResolutionLevel)
	ON_CBN_SELCHANGE(IDC_PAT_FINAL_RESOLUTION_LEVEL, OnSelchangeFinalResolutionLevel)
	ON_EN_KILLFOCUS(IDC_PAT_ADDITIONAL_CANDIDATES_VALUE, OnKillAdditionalCandidates)
	ON_EN_KILLFOCUS(IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE, OnKillCandidateSpacingXMin)
	ON_EN_KILLFOCUS(IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE, OnKillCandidateSpacingYMin)
	ON_EN_KILLFOCUS(IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE, OnKillPreliminaryAcceptanceThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPatAdvancedPageClass message handlers

void SVPatAdvancedPageClass::OnSelchangeFastFind()
{
	int	nSel = m_FastFindComboBox.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_lFastFind = static_cast<long>(m_FastFindComboBox.GetItemData(nSel));
	}
}

void SVPatAdvancedPageClass::OnAcceptanceThresholdAutoClicked()
{
	UpdateData(true);

	if (m_PreliminaryAcceptanceThresholdAutoCheckBox.GetCheck() == BST_CHECKED)
	{
		m_dPreliminaryAcceptanceThreshold = SVValueDefault;
	}
	else
	{
		m_dPreliminaryAcceptanceThreshold = 1.0;
	}

	UpdatePreliminaryAcceptanceThresholdString();
	EnablePreliminaryAcceptanceThresholdEdit();

	if (m_dPreliminaryAcceptanceThreshold != SVValueDefault)
	{
		SetSliderPosition(IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE);
	}
	UpdateData(false);
}

void SVPatAdvancedPageClass::OnAdditionalCandidatesAutoClicked()
{
	UpdateData(true);

	if (m_AdditionalCandidatesAutoCheckBox.GetCheck() == BST_CHECKED)
	{
		m_lAdditionalCandidates = SVValueDefault;
	}
	else
	{
		m_lAdditionalCandidates = 1; // could probably save the last set value and set it back to that...
	}
	UpdateAdditionalCandidatesString();
	EnableAdditionalCandidatesEdit();

	UpdateData(false);
}

void SVPatAdvancedPageClass::OnCandidateSpacingXMinAutoClicked()
{
	UpdateData(true);

	if (m_CandidateSpacingXMinAutoCheckBox.GetCheck() == BST_CHECKED)
	{
		m_dCandidateSpacingXMin = SVValueDefault;
	}
	else
	{
		m_dCandidateSpacingXMin = 1.0;
	}
	UpdateCandidateSpacingXMinString();
	EnableCandidateSpacingXMinEdit();

	UpdateData(false);
}

void SVPatAdvancedPageClass::OnCandidateSpacingYMinAutoClicked()
{
	UpdateData(true);

	if (m_CandidateSpacingYMinAutoCheckBox.GetCheck() == BST_CHECKED)
	{
		m_dCandidateSpacingYMin = SVValueDefault;
	}
	else
	{
		m_dCandidateSpacingYMin = 1.0;
	}
	UpdateCandidateSpacingYMinString();
	EnableCandidateSpacingYMinEdit();

	UpdateData(false);
}

void SVPatAdvancedPageClass::OnSelchangeModelStep()
{
	int	nSel = m_ModelStepComboBox.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_lModelStep = static_cast<long>(m_ModelStepComboBox.GetItemData(nSel));
	}
}

void SVPatAdvancedPageClass::OnSelchangeBeginningResolutionLevel()
{
	int	nSel = m_BeginningResolutionLevelComboBox.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_lBeginningResolutionLevel = static_cast<long>(m_BeginningResolutionLevelComboBox.GetItemData(nSel));
	}
}

void SVPatAdvancedPageClass::OnSelchangeFinalResolutionLevel()
{
	int	nSel = m_FinalResolutionLevelComboBox.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_lFinalResolutionLevel = static_cast<long>(m_FinalResolutionLevelComboBox.GetItemData(nSel));
	}
}

void SVPatAdvancedPageClass::OnKillAdditionalCandidates()
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_ADDITIONAL_CANDIDATES_VALUE))
		return;

	ProcessOnKillfocus(IDC_PAT_ADDITIONAL_CANDIDATES_VALUE);
	UpdateData(TRUE);
}

void SVPatAdvancedPageClass::OnKillCandidateSpacingXMin()
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE))
		return;

	ProcessOnKillfocus(IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE);
}

void SVPatAdvancedPageClass::OnKillCandidateSpacingYMin()
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE))
		return;

	ProcessOnKillfocus(IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE);
}

void SVPatAdvancedPageClass::OnKillPreliminaryAcceptanceThreshold()
{
	if(GetFocus() && (GetFocus()->GetDlgCtrlID() == IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE))
		return;

	ProcessOnKillfocus(IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE);
}

BOOL SVPatAdvancedPageClass::ProcessOnKillfocus(UINT nId) 
{
	if (GetActiveWindow() != (CWnd *)m_pSheet ||
		m_pSheet->GetActiveIndex() != 0)
		return TRUE;
	
	UINT nMsgID = 0;
	
	switch (nId)
	{
		case IDC_PAT_ADDITIONAL_CANDIDATES_VALUE:
		{
			ValidateAdditionalCandidatesValue(nMsgID);
			break;
		}

		case IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE:
		{
			if (ValidatePreliminaryAcceptanceThreshold(nMsgID))
			{
				AdjustSliderPreliminaryAcceptanceThreshold();
			}
			break;
		}

		case IDC_PAT_CANDIDATES_SPACING_XMIN_VALUE:
		{
			ValidateCandidatesSpacingXMinValue(nMsgID);
			break;
		}

		case IDC_PAT_CANDIDATES_SPACING_YMIN_VALUE:
		{
			ValidateCandidatesSpacingYMinValue(nMsgID);
			break;
		}
	}
	
	if (nMsgID)
	{
		AfxMessageBox(nMsgID);
		GetDlgItem(nId)->SetFocus();
		((CEdit *)GetDlgItem(nId))->SetSel(0, -1);
		return FALSE;
	}
	return TRUE;
}

bool SVPatAdvancedPageClass::ValidateEditableParameters(UINT nMsgID)
{
	bool bRetVal = ValidateAdditionalCandidatesValue(nMsgID);
	if (bRetVal)
	{
		bRetVal = ValidateCandidatesSpacingXMinValue(nMsgID);
	}
	if (bRetVal)
	{
		bRetVal = ValidateCandidatesSpacingYMinValue(nMsgID);
	}
	if (bRetVal)
	{
		bRetVal = ValidatePreliminaryAcceptanceThreshold(nMsgID);
	}
	return bRetVal;
}

bool SVPatAdvancedPageClass::ValidateAdditionalCandidatesValue(UINT nMsgID)
{
	UpdateData(true);

	bool bRetVal = true;
	if (m_AdditionalCandidatesAutoCheckBox.GetCheck() == BST_UNCHECKED)
	{
		long lAdditionalCandidates = _ttol(m_AdditionalCandidatesStr);

		bRetVal = (lAdditionalCandidates >= 1 && lAdditionalCandidates <= 100);
		if (!bRetVal)
		{
			nMsgID = IDS_PAT_ADDITIONALCANDIDATES_ERROR;
		}
		else
		{
			m_lAdditionalCandidates = lAdditionalCandidates;
		}
	}
	return bRetVal;
}

bool SVPatAdvancedPageClass::ValidateCandidatesSpacingXMinValue(UINT nMsgID)
{
	UpdateData(true);
	
	bool bRetVal = true;
	if (m_CandidateSpacingXMinAutoCheckBox.GetCheck() == BST_UNCHECKED)
	{
		double dCandidateSpacingXMin = _tstof(m_CandidateSpacingXMinStr);

		bRetVal = (dCandidateSpacingXMin >= 1.0 && dCandidateSpacingXMin <= 100.0);
		if (!bRetVal)
		{
			nMsgID = IDS_PAT_CANDIDATE_SPACING_ERROR;
		}
		else
		{
			m_dCandidateSpacingXMin = dCandidateSpacingXMin;
		}
	}
	return bRetVal;
}

bool SVPatAdvancedPageClass::ValidateCandidatesSpacingYMinValue(UINT nMsgID)
{
	UpdateData(true);
	
	bool bRetVal = true;
	if (m_CandidateSpacingYMinAutoCheckBox.GetCheck() == BST_UNCHECKED)
	{
		double dCandidateSpacingYMin = _tstof(m_CandidateSpacingYMinStr);

		bRetVal = (dCandidateSpacingYMin >= 1.0 && dCandidateSpacingYMin <= 100.0);
		if (!bRetVal)
		{
			nMsgID = IDS_PAT_CANDIDATE_SPACING_ERROR;
		}
		else
		{
			m_dCandidateSpacingYMin = dCandidateSpacingYMin;
		}
	}
	return bRetVal;
}

bool SVPatAdvancedPageClass::ValidatePreliminaryAcceptanceThreshold(UINT nMsgID)
{
	UpdateData(true);
	
	bool bRetVal = true;
	if (m_PreliminaryAcceptanceThresholdAutoCheckBox.GetCheck() == BST_UNCHECKED)
	{
		double dPreliminaryAcceptanceThreshold = _tstof(m_PreliminaryAcceptanceThresholdStr);

		bRetVal = (dPreliminaryAcceptanceThreshold >= 1.0 && dPreliminaryAcceptanceThreshold <= 100.0);
		if (!bRetVal)
		{
			nMsgID = IDS_PAT_PRELIMINARYACCEPTANCETHRESHOLD_ERROR;
		}
		else
		{
			m_dPreliminaryAcceptanceThreshold = dPreliminaryAcceptanceThreshold;
		}
	}
	return bRetVal;
}

void SVPatAdvancedPageClass::AdjustSliderPreliminaryAcceptanceThreshold() 
{
	// Slider is not enabled if value is "Auto"
	if (m_PreliminaryAcceptanceThresholdSliderCtrl.IsWindowEnabled())
	{
		SetSliderPosition(IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE);

		int nPos = m_PreliminaryAcceptanceThresholdSliderCtrl.GetPos();
		//m_dPreliminaryAcceptanceThreshold = static_cast<double>(nPos) / 10.0;
		m_dPreliminaryAcceptanceThreshold = static_cast<double>(nPos);
		UpdatePreliminaryAcceptanceThresholdString();

		UpdateData(false);
	}
}

void SVPatAdvancedPageClass::OnCancel() 
{
}

void SVPatAdvancedPageClass::OnOK() 
{
	UINT nMsgID = 0;
	if (ValidateEditableParameters(nMsgID))
	{
		CPropertyPage::OnOK();
	}
	else
	{
		AfxMessageBox(nMsgID);
	}
}

void SVPatAdvancedPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
	int iNewPos = pSlider->GetPos();

	switch (nSBCode)
	{
		case SB_LEFT:
		  iNewPos = pSlider->GetRangeMin();
		  break;

		case SB_ENDSCROLL :
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
		case IDC_PAT_ACCEPTANCE_THRESHOLD_SLIDER:
		{
			//m_dPreliminaryAcceptanceThreshold = ((double)iNewPos) / 10.0;
			m_dPreliminaryAcceptanceThreshold = static_cast<double>(iNewPos);
			UpdatePreliminaryAcceptanceThresholdString();
			UpdateData(false);
			break;
		}
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL SVPatAdvancedPageClass::OnKillActive()
{
	UINT nMsgID = 0;
	if (!ValidateEditableParameters(nMsgID))
	{
		AfxMessageBox(nMsgID);
		return false;
	}
	return true;
}

BOOL SVPatAdvancedPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pSheet = (SVPatAnalyzeSetupDlgSheet *)GetParent();
	
	InitializeControlValues();

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPatAdvancedPageClass::SetSliderPosition(UINT nId)
{
	CString	strValue;
	GetDlgItemText(nId, strValue);
	double dValue = atof(strValue.Left(10));

	CSliderCtrl* pSlider = NULL;

	switch (nId)
	{
		case IDC_PAT_ACCEPTANCE_THRESHOLD_VALUE:
			pSlider	= &m_PreliminaryAcceptanceThresholdSliderCtrl;
			break;
	}
	if (pSlider)
	{
		int	nPos = static_cast<int>(dValue);
		if (nPos < pSlider->GetRangeMin())
			nPos = pSlider->GetRangeMin();

		if (nPos > pSlider->GetRangeMax())
			nPos = pSlider->GetRangeMax();
		
		pSlider->SetPos(nPos);
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPatAdvancedPageClass.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:01:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:18:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Jan 2012 11:53:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  742
 * SCR Title:  Fix GUI issue with Pattern Analyzer Setup Advanced settings
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed issue wuth Additional Candidates going back to a prior value when clicking on the Preliminary Acceptance Threshold
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 11:50:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Aug 2010 13:51:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Feb 2010 14:39:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  684
 * SCR Title:  Fix issues with Advanced Tab on the Pattern Analyzer
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a call to OnInitDialog in SVPatAdvancedPageClass.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Dec 2008 08:12:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
