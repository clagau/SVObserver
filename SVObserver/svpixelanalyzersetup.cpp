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
#include "SVOCore/SVTool.h"
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
	m_GrayscaleToCount = 0;
	m_HighGrayscaleValue = _T("");
	m_LowGrayscaleValue = _T("");
	m_GrayscaleSliderValue = 0;
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
		l_hrOk = AddInputRequest( &( m_pAnalyzer->m_pixelCountColor ), m_GrayscaleToCount );

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
	DDX_Control(pDX, IDC_GRAYSCALE_SLD, m_GrayscaleSld);
	DDX_Control(pDX, IDC_OTHER_RAD, m_OtherRad);
	DDX_Control(pDX, IDC_WHITE_RAD, m_WhiteRad);
	DDX_Control(pDX, IDC_BLACK_RAD, m_BlackRad);
	DDX_Control(pDX, IDC_GRAYSCALE_EDT, m_GrayScaleEdt);
	DDX_Text(pDX, IDC_GRAYSCALE_EDT, m_GrayscaleToCount);
	DDX_Text(pDX, IDC_HIGHGRAYSCALE, m_HighGrayscaleValue);
	DDX_Text(pDX, IDC_LOWGRAYSCALE, m_LowGrayscaleValue);
	DDX_Slider(pDX, IDC_GRAYSCALE_SLD, m_GrayscaleSliderValue);
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
		m_HighGrayscaleValue.Format (_T("%d"), msvulMaxGrayscale);
		m_LowGrayscaleValue.Format (_T("%d"), msvulMinGrayscale);

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

			m_GrayscaleSld.SetRangeMin (-((long)msvulMaxGrayscale), FALSE);
			m_GrayscaleSld.SetRangeMax (-((long)msvulMinGrayscale), FALSE);
			UpdateData (FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}


unsigned long SVPixelAnalyzerSetupClass::SetBlack ()
{
	unsigned long ret(0);
	m_WhiteRad.SetCheck(BST_UNCHECKED);
	m_BlackRad.SetCheck (BST_CHECKED);
	m_OtherRad.SetCheck(BST_UNCHECKED);
	m_GrayscaleSld.EnableWindow (FALSE);
	m_GrayScaleEdt.EnableWindow (FALSE);

	SetGrayscale (msvulMinGrayscale);

	return ret;
	}


unsigned long SVPixelAnalyzerSetupClass::SetWhite ()
{
	unsigned long ret(0);
	m_WhiteRad.SetCheck(BST_CHECKED);
	m_BlackRad.SetCheck (BST_UNCHECKED);
	m_OtherRad.SetCheck(BST_UNCHECKED);
	m_GrayscaleSld.EnableWindow (FALSE);
	m_GrayScaleEdt.EnableWindow (FALSE);

	SetGrayscale (msvulMaxGrayscale);


	return ret;

}


unsigned long SVPixelAnalyzerSetupClass::SetOther (long alGrayscale)
{
	unsigned long ret(0);
	m_WhiteRad.SetCheck(BST_UNCHECKED);
	m_BlackRad.SetCheck (BST_UNCHECKED);
	m_OtherRad.SetCheck(BST_CHECKED);
	m_GrayscaleSld.EnableWindow (TRUE);
	m_GrayScaleEdt.EnableWindow (TRUE);

	SetGrayscale (alGrayscale);
	return ret;
}


unsigned long SVPixelAnalyzerSetupClass::SetGrayscale (long alGrayscale)
{
	unsigned long ret(0);
	m_GrayscaleToCount = alGrayscale;
	m_GrayscaleSliderValue = -alGrayscale;
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
	SetGrayscale (-m_GrayscaleSliderValue);
}

void SVPixelAnalyzerSetupClass::OnChangeGrayscaleEdt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData (TRUE);
	SetGrayscale (m_GrayscaleToCount);
	
}

void SVPixelAnalyzerSetupClass::OnCancel() 
{
	CDialog::OnCancel();
}


void SVPixelAnalyzerSetupClass::OnPixelSetRange() 
{
	SvOi::IObjectClass* pAnalyzerResult;

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

