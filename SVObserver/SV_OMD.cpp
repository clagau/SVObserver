//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OMD
//* .File Name       : $Workfile:   SV_OMD.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SV_OMD.h"
#include "SVOCRDialog.h"
#include "SVOCRAnalyzerResult.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRMiscDlg property page

IMPLEMENT_DYNCREATE(SVOCRMiscDlg, CPropertyPage)

SVOCRMiscDlg::SVOCRMiscDlg() : CPropertyPage(SVOCRMiscDlg::IDD)
{
	//{{AFX_DATA_INIT(SVOCRMiscDlg)
	m_samplesEdit = _T("");
	m_featuresEdit = 0;
	//}}AFX_DATA_INIT
}

SVOCRMiscDlg::~SVOCRMiscDlg()
{
}

HRESULT SVOCRMiscDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData (TRUE);

	SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	if (pOCRAnalyzerResult)
	{
		CString csFontFileName;
		m_fontFileNameEdit.GetWindowText( csFontFileName );

		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->msvfnvocFontFileName ), csFontFileName );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pOCRAnalyzerResult->GetTool() );
		}
	}

	if (pOCRParameters)
	{
		pOCRParameters->samples = m_samplesInt;

		pOCRParameters->fFeaturesUsed = ((float) m_featuresInt) / (float) 100.0;
	}

	return l_hrOk;
}

void SVOCRMiscDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRMiscDlg)
	DDX_Control(pDX, IDC_FONTFILENAME, m_fontFileNameEdit);
	DDX_Control(pDX, IDC_FEATURES, m_featureCntrl);
	DDX_Control(pDX, IDC_SLIDER1, m_featuresSlider);
	DDX_Text(pDX, IDC_SAMPLES, m_samplesEdit);
	DDX_Text(pDX, IDC_FEATURES, m_featuresEdit);
	DDV_MinMaxInt(pDX, m_featuresEdit, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRMiscDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRMiscDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_FEATURES, OnChangeFeatures)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRMiscDlg message handlers

void SVOCRMiscDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData (TRUE);

	m_samplesInt = pNMUpDown->iPos;
	m_samplesEdit.Format ("%d", 8 << m_samplesInt);
	UpdateData (FALSE);

	*pResult = 0;
}


BOOL SVOCRMiscDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
	SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	SetTaskObject( pOCRAnalyzerResult );

	CString csTemp;
	pOCRAnalyzerResult->GetFontFileName( csTemp );
	m_fontFileNameEdit.SetWindowText( csTemp );

	CSpinButtonCtrl* spin1 = (CSpinButtonCtrl *) GetDlgItem (IDC_SPIN1);

	m_samplesInt = pOCRParameters->samples;

	spin1->SetRange (0, 5);		// 8 - 256

	spin1->SetPos (m_samplesInt);
	m_samplesEdit.Format ("%d", 8<<m_samplesInt);

	m_featuresInt = (int)(pOCRParameters->fFeaturesUsed * 100.0 + .5);

	m_featuresEdit = m_featuresInt;
	m_featuresSlider.SetRange (0, 100);
	m_featuresSlider.SetPos (m_featuresInt);
	m_featuresSlider.SetTicFreq (10);
	m_featureCntrl.SetLimitText (3);

	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVOCRMiscDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

	UpdateData (TRUE);

	m_featuresInt = m_featuresSlider.GetPos ();
	m_featuresEdit = m_featuresInt;
	UpdateData (FALSE);

//	*pResult = 0;
}


void SVOCRMiscDlg::OnChangeFeatures() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	TCHAR		tmp [100];

	m_featureCntrl.GetWindowText (tmp, sizeof(tmp));

	m_featuresInt = atoi (tmp);

	m_featuresSlider.SetPos (m_featuresInt);
}

void SVOCRMiscDlg::OnFontBrowseCmd() 
{
	CString csTemp;
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCR_FONT_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp()->GetProfileString(	_T( "Settings" ),
	                                                _T( "WitFontFilePath" ),
	                                                _T( "C:\\RUN" ) );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	m_fontFileNameEdit.GetWindowText( csTemp );
	svfncFileName.SetFullFileName( csTemp );
	if ( svfncFileName.SelectFile() )
	{
		csTemp = svfncFileName.GetFullFileName();
		if ( csTemp.IsEmpty() )
		{
			AfxMessageBox(_T("ERROR: No Font File Specified"));
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".wit" ) ) != 0 )
			{
				CString s;
				s.Format( _T( "ERROR: Font File Requires '.Wit' Extension: %s" ),
					        svfncFileName.GetFullFileName() );
				AfxMessageBox(s);
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName(), rStatus ) )
				{
					CString s;
					s.Format( _T( "ERROR: Font File Does Not Exist: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else if ( rStatus.m_size <= 0 )
				{
					CString s;
					s.Format( _T( "ERROR: Font File Empty: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else
				{
					m_fontFileNameEdit.SetWindowText( svfncFileName.GetFullFileName() );

					UpdateData( FALSE );
				}
			}
		}
	}
}


BOOL SVOCRMiscDlg::UpdateOCRParameters ()
{
	BOOL l_bRet = TRUE;
	//
	// Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
	//
	if (!GetSafeHwnd())
	{
		return TRUE;
	}
	
	l_bRet = SetInspectionData() == S_OK;

	return l_bRet;
}

BOOL SVOCRMiscDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters ();
	return CPropertyPage::OnKillActive();
}

