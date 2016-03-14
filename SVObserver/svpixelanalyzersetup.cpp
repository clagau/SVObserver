//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPixelAnalyzerSetup
//* .File Name       : $Workfile:   svpixelanalyzersetup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Sep 2013 11:11:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVPixelAnalyzerSetup.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVPixelAnalyzer.h"        // Required for SVPixelAnalyzerClass
#include "SVTool.h"
#include "SVToolSet.h"              // Required for scripting
#include "SVResult.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPixelAnalyzerSetupClass::SVPixelAnalyzerSetupClass(SVPixelAnalyzerClass* apAnalyzer, 
                                                     CWnd* pParent /*=NULL*/)
	: CDialog(SVPixelAnalyzerSetupClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPixelAnalyzerSetupClass)
	msvGrayscaleToCount = 0;
	msvHighGrayscaleValue = _T("");
	msvLowGrayscaleValue = _T("");
	msvGrayscaleSliderValue = 0;
	//}}AFX_DATA_INIT

    init (apAnalyzer, pParent);
}


HRESULT SVPixelAnalyzerSetupClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( msvpAnalyzer != NULL )
	{
		l_hrOk = AddInputRequest( &( msvpAnalyzer->pixelCountColor ), msvGrayscaleToCount );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( msvpAnalyzer->GetTool() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

unsigned long SVPixelAnalyzerSetupClass::init (SVPixelAnalyzerClass *apAnalyzer, 
                                               CWnd* apParent)
{
    SVObjectTypeInfoStruct  pixelAnalyzerToolInfo;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        msvpParent = apParent;
        msvpAnalyzer = apAnalyzer;
        if (!msvpAnalyzer)
        {
            msvError.msvlErrorCd = -1098;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1098);
        }

        msvpTool = (SVToolClass *) msvpAnalyzer->GetOwner ();
        if (!msvpTool)
        {
            msvError.msvlErrorCd = -1096;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1096);
        }
        break;
    }

    return msvError.GetLastErrorCd ();
}


void SVPixelAnalyzerSetupClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPixelAnalyzerSetupClass)
	DDX_Control(pDX, IDC_GRAYSCALE_SLD, msvGrayscaleSld);
	DDX_Control(pDX, IDC_OTHER_RAD, msvOtherRad);
	DDX_Control(pDX, IDC_WHITE_RAD, msvWhiteRad);
	DDX_Control(pDX, IDC_BLACK_RAD, msvBlackRad);
	DDX_Control(pDX, IDC_GRAYSCALE_EDT, msvGrayScaleEdt);
	DDX_Text(pDX, IDC_GRAYSCALE_EDT, msvGrayscaleToCount);
	DDX_Text(pDX, IDC_HIGHGRAYSCALE, msvHighGrayscaleValue);
	DDX_Text(pDX, IDC_LOWGRAYSCALE, msvLowGrayscaleValue);
	DDX_Slider(pDX, IDC_GRAYSCALE_SLD, msvGrayscaleSliderValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPixelAnalyzerSetupClass, CDialog)
	//{{AFX_MSG_MAP(SVPixelAnalyzerSetupClass)
	ON_BN_CLICKED(IDC_WHITE_RAD, OnWhiteRad)
	ON_BN_CLICKED(IDC_BLACK_RAD, OnBlackRad)
	ON_BN_CLICKED(IDC_OTHER_RAD, OnOtherRad)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_GRAYSCALE_EDT, OnChangeGrayscaleEdt)
	ON_BN_CLICKED(IDC_PIXEL_SET_RANGE, OnPixelSetRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPixelAnalyzerSetupClass message handlers

void SVPixelAnalyzerSetupClass::OnOK() 
{
	// TODO: Add extra validation here
    ASSERT(msvpTool);
    if(msvpTool)
    {
		SVIPDoc* l_pIPDoc = NULL;

		if( msvpTool->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( msvpTool->GetInspection()->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}
    }
    	
	CDialog::OnOK();
}


BOOL SVPixelAnalyzerSetupClass::OnInitDialog() 
{
    BYTE    byGrayscale;

    msvError.ClearLastErrorCd ();

    while (1)
    {
       	CDialog::OnInitDialog();

				SetTaskObject( msvpTool );

        if (!msvpTool)
        {
            msvError.msvlErrorCd = -1103;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1103);
        }


        msvulMinGrayscale = 0;
        msvulMaxGrayscale = (1 << msvpAnalyzer->GetInputPixelDepth ()) - 1;

	    msvHighGrayscaleValue.Format (_T("%d"), msvulMaxGrayscale);
		msvLowGrayscaleValue.Format (_T("%d"), msvulMinGrayscale);

        msvpAnalyzer->pixelCountColor.GetValue(byGrayscale);

        if (byGrayscale == msvulMinGrayscale) //Black
        {
            SetBlack ();
        }
        else
        if (byGrayscale == msvulMaxGrayscale) //White
        {
            SetWhite ();
        }
        else
        {
            SetOther (byGrayscale);

        }
            
        msvGrayscaleSld.SetRangeMin (-((long)msvulMaxGrayscale), FALSE);
        msvGrayscaleSld.SetRangeMax (-((long)msvulMinGrayscale), FALSE);

        UpdateData (FALSE);
        break;
    }
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


unsigned long SVPixelAnalyzerSetupClass::SetBlack ()
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_UNCHECKED);
        msvBlackRad.SetCheck (BST_CHECKED);
		msvOtherRad.SetCheck(BST_UNCHECKED);
        msvGrayscaleSld.EnableWindow (FALSE);
        msvGrayScaleEdt.EnableWindow (FALSE);

        SetGrayscale (msvulMinGrayscale);
        break;
    }


    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetWhite ()
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_CHECKED);
        msvBlackRad.SetCheck (BST_UNCHECKED);
		msvOtherRad.SetCheck(BST_UNCHECKED);
        msvGrayscaleSld.EnableWindow (FALSE);
        msvGrayScaleEdt.EnableWindow (FALSE);

        SetGrayscale (msvulMaxGrayscale);
        break;
    }

    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetOther (long alGrayscale)
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_UNCHECKED);
        msvBlackRad.SetCheck (BST_UNCHECKED);
		msvOtherRad.SetCheck(BST_CHECKED);
        msvGrayscaleSld.EnableWindow (TRUE);
        msvGrayScaleEdt.EnableWindow (TRUE);
        
        SetGrayscale (alGrayscale);
        break;
    }

    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetGrayscale (long alGrayscale)
{
	msvError.ClearLastErrorCd ();
	
	msvGrayscaleToCount = alGrayscale;
	msvGrayscaleSliderValue = -alGrayscale;

	SetInspectionData();
	
	return msvError.GetLastErrorCd ();
}



void SVPixelAnalyzerSetupClass::OnWhiteRad() 
{
    SetWhite ();	
}


void SVPixelAnalyzerSetupClass::OnBlackRad() 
{
    SetBlack ();	
}


void SVPixelAnalyzerSetupClass::OnOtherRad() 
{
    BYTE    byGrayscale;

    msvpAnalyzer->pixelCountColor.GetValue(byGrayscale);
    SetOther (byGrayscale);	
}


void SVPixelAnalyzerSetupClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateData (TRUE);
	SetGrayscale (-msvGrayscaleSliderValue);
}

void SVPixelAnalyzerSetupClass::OnChangeGrayscaleEdt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData (TRUE);
	SetGrayscale (msvGrayscaleToCount);
	
}

void SVPixelAnalyzerSetupClass::OnCancel() 
{
	CDialog::OnCancel();
}


void SVPixelAnalyzerSetupClass::OnPixelSetRange() 
{
	// TODO: Add your control notification handler code here
    SVResultClass* pAnalyzerResult;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        pAnalyzerResult = msvpAnalyzer->GetResultObject();

        if (!pAnalyzerResult)
        {
            msvError.msvlErrorCd = -1121;
            SV_TRAP_ERROR_BRK (msvError, 1121);
        }

		if (SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), msvpParent) != S_OK)
        {
            msvError.msvlErrorCd = -1122;
            SV_TRAP_ERROR_BRK (msvError, 1122);
        }

        break;
    }
}

