//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OGD
//* .File Name       : $Workfile:   SV_OGD.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 12:05:38  $
//******************************************************************************

#include "stdafx.h"
#include "SV_OGD.h"
#include "SVOCRDialog.h"
#include "SVOCRAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRGeometryDlg dialog


SVOCRGeometryDlg::SVOCRGeometryDlg(): 
	CPropertyPage(SVOCRGeometryDlg::IDD)
{
	//pOCRAnalyzerResult = NULL;

	//{{AFX_DATA_INIT(SVOCRGeometryDlg)
	m_scaleInvarientButton = FALSE;
	//}}AFX_DATA_INIT
}


void SVOCRGeometryDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGeometryDlg)
	DDX_Control(pDX, IDC_EDIT_MAX_BLOB_PIXELS, m_editMaxBlobPixels);
	DDX_Control(pDX, IDC_EDIT_MIN_BLOB_PIXELS, m_editMinBlobPixels);
	DDX_Control(pDX, IDC_SENSITIVITY_EDIT, m_sensitivityEdit);
	DDX_Control(pDX, IDC_ROTMODE_CMB, m_rotationModeCombo);
	DDX_Control(pDX, IDC_MINWIDTH_EDIT, m_minWidthEdit);
	DDX_Control(pDX, IDC_MINSCALE_EDIT, m_minScaleEdit);
	DDX_Control(pDX, IDC_MINHEIGHT_EDIT, m_minHeightEdit);
	DDX_Control(pDX, IDC_MINANGLE_EDIT, m_minAngleEdit);
	DDX_Control(pDX, IDC_MAXWIDTH_EDIT, m_maxWidthEdit);
	DDX_Control(pDX, IDC_MAXSCALE_EDIT, m_maxScaleEdit);
	DDX_Control(pDX, IDC_MAXHEIGHT_EDIT, m_maxHeightEdit);
	DDX_Control(pDX, IDC_MAXANGLE_EDIT, m_maxAngleEdit);
	DDX_Check(pDX, IDC_SCALEINVARIENT_BUTTON, m_scaleInvarientButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGeometryDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRGeometryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRGeometryDlg message handlers

BOOL SVOCRGeometryDlg::OnInitDialog() 
{
	CString	tmp;

	CPropertyPage::OnInitDialog();
	
    SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	while (1)
	{
		m_sensitivityEdit.SetLimitText (10);
		m_minWidthEdit.SetLimitText (10);
		m_minScaleEdit.SetLimitText (10);
		m_minHeightEdit.SetLimitText (10);
		m_minAngleEdit.SetLimitText (10);
		m_maxWidthEdit.SetLimitText (10);
		m_maxScaleEdit.SetLimitText (10);
		m_maxHeightEdit.SetLimitText (10);
		m_maxAngleEdit.SetLimitText (10);

		m_editMinBlobPixels.SetLimitText (10);
		m_editMaxBlobPixels.SetLimitText (10);

		tmp.Format ("%0.2f",pOCRParameters->forientationSensitivity);
		m_sensitivityEdit.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->minWidth);
		m_minWidthEdit.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->maxWidth);
		m_maxWidthEdit.SetWindowText (tmp);


		tmp.Format ("%0.2f",pOCRParameters->fminScale);
		m_minScaleEdit.SetWindowText (tmp);

		tmp.Format ("%0.2f",pOCRParameters->fmaxScale);
		m_maxScaleEdit.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->minHeight);
		m_minHeightEdit.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->maxHeight);
		m_maxHeightEdit.SetWindowText (tmp);


		tmp.Format ("%0.2f",pOCRParameters->fminAngle);
		m_minAngleEdit.SetWindowText (tmp);

		tmp.Format ("%0.2f",pOCRParameters->fmaxAngle);
		m_maxAngleEdit.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->nMinPixelsInBlob);
		m_editMinBlobPixels.SetWindowText (tmp);

		tmp.Format ("%d",pOCRParameters->nMaxPixelsInBlob);
		m_editMaxBlobPixels.SetWindowText (tmp);

		m_rotaionModeComboContents[0].LoadString (IDS_INVARIENT);
		m_rotaionModeComboContents[1].LoadString (IDS_NONINVARIENT);
		m_rotaionModeComboContents[2].LoadString (IDS_RANGEOFANGLES);

		int index;

		index = m_rotationModeCombo.AddString (m_rotaionModeComboContents[0]);
		m_rotationModeCombo.SetItemData(index, 0);

		index = m_rotationModeCombo.AddString (m_rotaionModeComboContents[1]);
		m_rotationModeCombo.SetItemData(index, 1);

		index = m_rotationModeCombo.AddString (m_rotaionModeComboContents[2]);
		m_rotationModeCombo.SetItemData(index, 2);

		m_rotationModeCombo.
			SelectString(-1, m_rotaionModeComboContents[pOCRParameters->rotation]);

		if (pOCRParameters->scaleInvariant == 0)
		{
			m_scaleInvarientButton = 0;
		}
		else
		{
			m_scaleInvarientButton = 1;
		}

		UpdateData (FALSE);
		break;
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*-------
void SVOCRGeometryDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateOCRParameters ();


	CPropertyPage::OnOK();
}
------*/

/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRGeometryDlg::UpdateOCRParameters ()
{
	BOOL l_bRet = TRUE;
	TCHAR	tmp [100];
	int		index;

    //
    // Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
    //
    if(!GetSafeHwnd())
    {
       return TRUE;
    }


	UpdateData (TRUE);

    SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *POCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	m_sensitivityEdit.GetWindowText (tmp, sizeof(tmp));
	POCRParameters->forientationSensitivity = (float) atof (tmp);

	m_minWidthEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->minWidth = atoi (tmp);

	m_maxWidthEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->maxWidth = atoi (tmp);

	m_minHeightEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->minHeight = atoi (tmp);

	m_maxHeightEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->maxHeight = atoi (tmp);

	m_minScaleEdit.GetWindowText (tmp, sizeof (tmp));
	float fTmpVal = (float) atof (tmp);
	if ( fTmpVal <= 0.0099 )
	{
		AfxMessageBox("Minimum Scale Value must be greater then 0.0099");
		l_bRet = FALSE;
	}
	POCRParameters->fminScale = (float) atof (tmp);

	m_maxScaleEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->fmaxScale = (float) atof (tmp);

	m_maxAngleEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->fmaxAngle = (float) atof (tmp);
	
	m_minAngleEdit.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->fminAngle = (float) atof (tmp);

    m_editMinBlobPixels.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->nMinPixelsInBlob = atoi( tmp );

    m_editMaxBlobPixels.GetWindowText (tmp, sizeof (tmp));
	POCRParameters->nMaxPixelsInBlob = atoi( tmp );

	index = m_rotationModeCombo.GetCurSel ();
	POCRParameters->rotation = static_cast< int >(
		m_rotationModeCombo.GetItemData( index ) );

	POCRParameters->scaleInvariant = 
		m_scaleInvarientButton;

	return l_bRet;
}

BOOL SVOCRGeometryDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL l_bRet = UpdateOCRParameters ();
	TCHAR tmp[100];
	m_minScaleEdit.GetWindowText(tmp,sizeof(tmp));
	float fNewValue = (float) atof(tmp);

	if ( !l_bRet )
	{
		// something is wrong on property sheet, do not let them leave this tab
		return FALSE;
	}

	return CPropertyPage::OnKillActive();
}

