//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeDataMatrixDialog
//* .File Name       : $Workfile:   SVBarCodeDataMatrixDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:41:06  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVBarCodeDataMatrixDialog.h"
#include "Definitions/SVMatroxSimpleEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeDataMatrixDialog dialog

SVBarCodeDataMatrixDialog::SVBarCodeDataMatrixDialog(CWnd* /*=nullptr*/)
	: CPropertyPage(SVBarCodeDataMatrixDialog::IDD)
{
	//{{AFX_DATA_INIT(SVBarCodeDataMatrixDialog)
	//}}AFX_DATA_INIT
}

SVBarCodeDataMatrixDialog::~SVBarCodeDataMatrixDialog()
{
}

void SVBarCodeDataMatrixDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBarCodeDataMatrixDialog)
	DDX_Text(pDX, IDC_BARCODE_CELLX_EDIT, m_szCellX);
	DDX_Slider(pDX, IDC_BARCODE_CELLX, m_iCellX);
	DDX_Check(pDX, IDC_BARCODE_CELLX_AUTO, m_bCellX);
	DDX_Slider(pDX, IDC_BARCODE_CELLY, m_iCellY);
	DDX_Check(pDX, IDC_BARCODE_CELLY_AUTO, m_bCellY);
	DDX_Text(pDX, IDC_BARCODE_CELLY_EDIT, m_szCellY);
	DDX_Slider(pDX, IDC_BARCODE_MAXCELLSIZE, m_iMaxCellSize);
	DDX_Check(pDX, IDC_BARCODE_MAXCELLSIZE_AUTO, m_bMaxCellSize);
	DDX_Text(pDX, IDC_BARCODE_MAXCELLSIZE_EDIT, m_szMaxCellSize);
	DDX_Slider(pDX, IDC_BARCODE_MINCELLSIZE, m_iMinCellSize);
	DDX_Check(pDX, IDC_BARCODE_MINCELLSIZE_AUTO, m_bMinCellSize);
	DDX_Text(pDX, IDC_BARCODE_MINCELLSIZE_EDIT, m_szMinCellSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVBarCodeDataMatrixDialog, CPropertyPage)
	//{{AFX_MSG_MAP(SVBarCodeDataMatrixDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_BARCODE_MINCELLSIZE_EDIT, OnChangeBarCodeMinCellSizeEdit)
	ON_EN_CHANGE(IDC_BARCODE_MAXCELLSIZE_EDIT, OnChangeBarCodeMaxCellSizeEdit)
	ON_EN_CHANGE(IDC_BARCODE_CELLX_EDIT, OnChangeBarCodeCellXEdit)
	ON_EN_CHANGE(IDC_BARCODE_CELLY_EDIT, OnChangeBarCodeCellYEdit)
	ON_BN_CLICKED(IDC_BARCODE_MINCELLSIZE_AUTO, OnBarCodeMinCellSizeAuto)
	ON_BN_CLICKED(IDC_BARCODE_MAXCELLSIZE_AUTO, OnBarCodeMaxCellSizeAuto)
	ON_BN_CLICKED(IDC_BARCODE_CELLX_AUTO, OnBarCodeCellXAuto)
	ON_BN_CLICKED(IDC_BARCODE_CELLY_AUTO, OnBarCodeCellYAuto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeDataMatrixDialog message handlers

BOOL SVBarCodeDataMatrixDialog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CSliderCtrl* pControl {nullptr};

	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	pControl = static_cast<CSliderCtrl*> (GetDlgItem(IDC_BARCODE_MINCELLSIZE));
	pControl->SetRangeMin(1, FALSE);
	pControl->SetRangeMax(255, TRUE);

	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	pControl = static_cast<CSliderCtrl*> (GetDlgItem(IDC_BARCODE_MAXCELLSIZE));
	pControl->SetRangeMin(1, FALSE);
	pControl->SetRangeMax(255, TRUE);

	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	pControl = static_cast<CSliderCtrl*> (GetDlgItem(IDC_BARCODE_CELLX));
	pControl->SetRangeMin(1, FALSE);
	pControl->SetRangeMax(255, TRUE);

	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	pControl = static_cast<CSliderCtrl*> (GetDlgItem(IDC_BARCODE_CELLY));
	pControl->SetRangeMin(1, FALSE);
	pControl->SetRangeMax(255, TRUE);

	EnableControls(IDC_BARCODE_MINCELLSIZE, IDC_BARCODE_MINCELLSIZE_EDIT, !m_bMinCellSize);
	EnableControls(IDC_BARCODE_MAXCELLSIZE, IDC_BARCODE_MAXCELLSIZE_EDIT, !m_bMaxCellSize);
	EnableControls(IDC_BARCODE_CELLX, IDC_BARCODE_CELLX_EDIT, !m_bCellX);
	EnableControls(IDC_BARCODE_CELLY, IDC_BARCODE_CELLY_EDIT, !m_bCellY);

	UpdateData(FALSE);

	return TRUE;
}

void SVBarCodeDataMatrixDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
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
	case IDC_BARCODE_MINCELLSIZE:
		m_iMinCellSize = iNewPos;
		m_szMinCellSize.Format(_T("%d"), m_iMinCellSize);
		break;

	case IDC_BARCODE_MAXCELLSIZE:
		m_iMaxCellSize = iNewPos;
		m_szMaxCellSize.Format(_T("%d"), m_iMaxCellSize);
		break;

	case IDC_BARCODE_CELLX:
		m_iCellX = iNewPos;
		m_szCellX.Format(_T("%d"), m_iCellX);
		break;

	case IDC_BARCODE_CELLY:
		m_iCellY = iNewPos;
		m_szCellY.Format(_T("%d"), m_iCellY);
		break;
	}

	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnChangeBarCodeMinCellSizeEdit()
{
	UpdateData(TRUE);
	m_iMinCellSize = atoi(m_szMinCellSize);
	if (m_iMinCellSize < 1)
	{
		m_iMinCellSize = 1;
		m_szMinCellSize = _T("1");
	}
	if (m_iMinCellSize > 255)
	{
		m_iMinCellSize = 255;
		m_szMinCellSize = _T("255");
	}
	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnChangeBarCodeMaxCellSizeEdit()
{
	UpdateData(TRUE);
	m_iMaxCellSize = atoi(m_szMaxCellSize);
	if (m_iMaxCellSize < 1)
	{
		m_iMaxCellSize = 1;
		m_szMaxCellSize = _T("1");
	}
	if (m_iMaxCellSize > 255)
	{
		m_iMaxCellSize = 255;
		m_szMaxCellSize = _T("255");
	}
	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnChangeBarCodeCellXEdit()
{
	UpdateData(TRUE);
	m_iCellX = atoi(m_szCellX);
	if (m_iCellX < 1)
	{
		m_iCellX = 1;
		m_szCellX = _T("1");
	}
	if (m_iCellX > 255)
	{
		m_iCellX = 255;
		m_szCellX = _T("255");
	}
	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnChangeBarCodeCellYEdit()
{
	UpdateData(TRUE);
	m_iCellY = atoi(m_szCellY);
	if (m_iCellY < 1)
	{
		m_iCellY = 1;
		m_szCellY = _T("1");
	}
	if (m_iCellY > 255)
	{
		m_iCellY = 255;
		m_szCellY = _T("255");
	}
	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::EnableControls(int iIdSlider, int iIdEdit, BOOL bEnable)
{
	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	CSliderCtrl* pSlider = static_cast<CSliderCtrl*> (GetDlgItem(iIdSlider));
	CEdit* pEdit = static_cast<CEdit*> (GetDlgItem(iIdEdit));
	pSlider->EnableWindow(bEnable);
	pEdit->EnableWindow(bEnable);
}

void SVBarCodeDataMatrixDialog::OnBarCodeMinCellSizeAuto()
{
	UpdateData(TRUE);

	if (m_bMinCellSize)
	{
		m_iMinCellSize = SVValueDefault;
		m_szMinCellSize = _T("Auto");
	}
	else
	{
		m_iMinCellSize = 1;
		m_szMinCellSize.Format("%d", m_iMinCellSize);
	}

	EnableControls(IDC_BARCODE_MINCELLSIZE, IDC_BARCODE_MINCELLSIZE_EDIT, !m_bMinCellSize);

	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnBarCodeMaxCellSizeAuto()
{
	UpdateData(TRUE);

	if (m_bMaxCellSize)
	{
		m_iMaxCellSize = SVValueDefault;
		m_szMaxCellSize = _T("Auto");
	}
	else
	{
		m_iMaxCellSize = 1;
		m_szMaxCellSize.Format("%d", m_iMaxCellSize);
	}

	EnableControls(IDC_BARCODE_MAXCELLSIZE, IDC_BARCODE_MAXCELLSIZE_EDIT, !m_bMaxCellSize);

	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnBarCodeCellXAuto()
{
	UpdateData(TRUE);

	if (m_bCellX)
	{
		m_iCellX = SVValueAny;
		m_szCellX = _T("Auto");
	}
	else
	{
		m_iCellX = 1;
		m_szCellX.Format("%d", m_iCellX);
	}

	EnableControls(IDC_BARCODE_CELLX, IDC_BARCODE_CELLX_EDIT, !m_bCellX);

	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::OnBarCodeCellYAuto()
{
	UpdateData(TRUE);

	if (m_bCellY)
	{
		m_iCellY = SVValueAny;
		m_szCellY = _T("Auto");
	}
	else
	{
		m_iCellY = 1;
		m_szCellY.Format("%d", m_iCellY);
	}

	EnableControls(IDC_BARCODE_CELLY, IDC_BARCODE_CELLY_EDIT, !m_bCellY);

	UpdateData(FALSE);
}

void SVBarCodeDataMatrixDialog::SetCellX(long cellX)
{
	m_iCellX = cellX;
	if (m_iCellX == SVValueAny)
	{
		m_szCellX = _T("Auto");
		m_bCellX = TRUE;
	}
	else
	{
		m_szCellX.Format("%d", m_iCellX);
		m_bCellX = FALSE;
	}
}

void SVBarCodeDataMatrixDialog::SetCellY(long cellY)
{
	m_iCellY = cellY;
	if (m_iCellY == SVValueAny)
	{
		m_szCellY = _T("Auto");
		m_bCellY = TRUE;
	}
	else
	{
		m_szCellY.Format("%d", m_iCellY);
		m_bCellY = FALSE;
	}
}

void SVBarCodeDataMatrixDialog::SetMinCellSize(long minCellSize)
{
	m_iMinCellSize = minCellSize;
	if (m_iMinCellSize == SVValueDefault)
	{
		m_szMinCellSize = _T("Auto");
		m_bMinCellSize = TRUE;
	}
	else
	{
		m_szMinCellSize.Format("%d", m_iMinCellSize);
		m_bMinCellSize = FALSE;
	}
}

void SVBarCodeDataMatrixDialog::SetMaxCellSize(long maxCellSize)
{
	m_iMaxCellSize = maxCellSize;
	if (m_iMaxCellSize == SVValueDefault)
	{
		m_szMaxCellSize = _T("Auto");
		m_bMaxCellSize = TRUE;
	}
	else
	{
		m_szMaxCellSize.Format("%d", m_iMaxCellSize);
		m_bMaxCellSize = FALSE;
	}
}

int SVBarCodeDataMatrixDialog::GetCellX()
{
	if (m_bCellX)
		return SVValueAny;
	else
		return m_iCellX;
}

int SVBarCodeDataMatrixDialog::GetCellY()
{
	if (m_bCellY)
		return SVValueAny;
	else
		return m_iCellY;
}

int SVBarCodeDataMatrixDialog::GetMinCellSize()
{
	if (m_bMinCellSize)
		return SVValueDefault;
	else
		return m_iMinCellSize;
}

int SVBarCodeDataMatrixDialog::GetMaxCellSize()
{
	if (m_bMaxCellSize)
		return SVValueDefault;
	else
		return m_iMaxCellSize;
}


