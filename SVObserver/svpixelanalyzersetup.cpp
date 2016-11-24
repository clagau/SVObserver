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
#pragma region Includes
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
#include "SVObserver.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPixelAnalyzerSetupClass::SVPixelAnalyzerSetupClass(SVPixelAnalyzerClass* apAnalyzer, CWnd* pParent /*=nullptr*/)
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

SVPixelAnalyzerSetupClass::~SVPixelAnalyzerSetupClass()
{
}

HRESULT SVPixelAnalyzerSetupClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( nullptr != m_pAnalyzer )
	{
		l_hrOk = AddInputRequest( &( m_pAnalyzer->m_pixelCountColor ), msvGrayscaleToCount );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pAnalyzer->GetTool()->GetUniqueObjectID() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

unsigned long SVPixelAnalyzerSetupClass::init (SVPixelAnalyzerClass *apAnalyzer, CWnd* apParent)
{
    SVObjectTypeInfoStruct  pixelAnalyzerToolInfo;
	unsigned long ret(0);
	m_pParent = apParent;
	m_pAnalyzer = apAnalyzer;
	if (nullptr == m_pAnalyzer)
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16082);
		ret = - SvOi::Err_16082;
	
	}
	else
	{
		m_pTool = (SVToolClass *) m_pAnalyzer->GetOwner ();
		if (!m_pTool)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16083);
			ret = - SvOi::Err_16083;
		}
	}
	return ret; 
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
	SVIPDoc* l_pIPDoc = TheSVObserverApp.GetIPDoc(m_pTool->GetInspection()->GetUniqueObjectID());

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->SetModifiedFlag();
    }

	CDialog::OnOK();
}

BOOL SVPixelAnalyzerSetupClass::OnInitDialog() 
{
	
	BYTE    byGrayscale;
	CDialog::OnInitDialog();
	SetTaskObject( m_pTool );

	if (nullptr == m_pTool)
	{
		SvStl::MessageMgrStd  Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16084);
	}
	else
	{
		msvulMinGrayscale = 0;
		try
		{
			msvulMaxGrayscale = (1 << m_pAnalyzer->GetInputPixelDepth ()) - 1;
		}
		catch (	const SvStl::MessageContainer& rContain )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( rContain.getMessage());
			return TRUE;

		}
		msvHighGrayscaleValue.Format (_T("%d"), msvulMaxGrayscale);
		msvLowGrayscaleValue.Format (_T("%d"), msvulMinGrayscale);

		m_pAnalyzer->m_pixelCountColor.GetValue(byGrayscale);

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
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}


unsigned long SVPixelAnalyzerSetupClass::SetBlack ()
{
	unsigned long ret(0);
	msvWhiteRad.SetCheck(BST_UNCHECKED);
	msvBlackRad.SetCheck (BST_CHECKED);
	msvOtherRad.SetCheck(BST_UNCHECKED);
	msvGrayscaleSld.EnableWindow (FALSE);
	msvGrayScaleEdt.EnableWindow (FALSE);

	SetGrayscale (msvulMinGrayscale);

	return ret;
	}


unsigned long SVPixelAnalyzerSetupClass::SetWhite ()
{
	unsigned long ret(0);
	msvWhiteRad.SetCheck(BST_CHECKED);
	msvBlackRad.SetCheck (BST_UNCHECKED);
	msvOtherRad.SetCheck(BST_UNCHECKED);
	msvGrayscaleSld.EnableWindow (FALSE);
	msvGrayScaleEdt.EnableWindow (FALSE);

	SetGrayscale (msvulMaxGrayscale);


	return ret;

}


unsigned long SVPixelAnalyzerSetupClass::SetOther (long alGrayscale)
{
	unsigned long ret(0);
	msvWhiteRad.SetCheck(BST_UNCHECKED);
	msvBlackRad.SetCheck (BST_UNCHECKED);
	msvOtherRad.SetCheck(BST_CHECKED);
	msvGrayscaleSld.EnableWindow (TRUE);
	msvGrayScaleEdt.EnableWindow (TRUE);

	SetGrayscale (alGrayscale);
	return ret;
}


unsigned long SVPixelAnalyzerSetupClass::SetGrayscale (long alGrayscale)
{
	unsigned long ret(0);
	msvGrayscaleToCount = alGrayscale;
	msvGrayscaleSliderValue = -alGrayscale;
	SetInspectionData();
	return ret;
	
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

    m_pAnalyzer->m_pixelCountColor.GetValue(byGrayscale);
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

	pAnalyzerResult = m_pAnalyzer->GetResultObject();

	if (nullptr == pAnalyzerResult)
	{
		SvStl::MessageMgrStd  Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16085);
	} 
	else if (S_OK != SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), m_pParent) )
	{
		SvStl::MessageMgrStd  Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16086);;
	}

}

