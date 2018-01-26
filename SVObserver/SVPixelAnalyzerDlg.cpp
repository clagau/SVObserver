//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPixelAnalyzerDlg
//* .File Name       : $Workfile:   SVPixelAnalyzerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Sep 2013 11:11:00  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVPixelAnalyzerDlg.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVPixelAnalyzer.h"        // Required for SVPixelAnalyzerClass
#include "InspectionEngine/SVTool.h"
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

BEGIN_MESSAGE_MAP(SVPixelAnalyzerDlg, CDialog)
	//{{AFX_MSG_MAP(SVPixelAnalyzerDlg)
	ON_BN_CLICKED(IDC_WHITE_RAD, OnWhiteRad)
	ON_BN_CLICKED(IDC_BLACK_RAD, OnBlackRad)
	ON_BN_CLICKED(IDC_OTHER_RAD, OnOtherRad)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_GRAYSCALE_EDT, OnChangeGrayscaleEdt)
	ON_BN_CLICKED(IDC_PIXEL_SET_RANGE, OnPixelSetRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVPixelAnalyzerDlg::SVPixelAnalyzerDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=nullptr*/)
: CDialog(SVPixelAnalyzerDlg::IDD, pParent)
, m_rInspectionID{ rInspectionID }
, m_rTaskObjectID{ rTaskObjectID }
, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
{
}

SVPixelAnalyzerDlg::~SVPixelAnalyzerDlg()
{
}

HRESULT SVPixelAnalyzerDlg::SetInspectionData()
{
	HRESULT Result{ E_FAIL };

	UpdateData(true);

	m_Values.Set<BYTE>(SVPixelColorIndexObjectGuid, static_cast<BYTE> (m_GrayscaleToCount));
	Result = m_Values.Commit();

	return Result;
}

void SVPixelAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPixelAnalyzerDlg)
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

void SVPixelAnalyzerDlg::OnOK() 
{
	CDialog::OnOK();
}

BOOL SVPixelAnalyzerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Values.Init();

	if (GUID_NULL == m_rTaskObjectID)
	{
		SvStl::MessageMgrStd  Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16080);
	}
	else
	{
		m_pAnalyzer = dynamic_cast<SVPixelAnalyzerClass*> (SVObjectManagerClass::Instance().GetObject(m_rTaskObjectID));
		msvulMinGrayscale = 0;
		if (nullptr != m_pAnalyzer)
		{
			try
			{
				msvulMaxGrayscale = (1 << m_pAnalyzer->GetInputPixelDepth()) - 1;
			}
			catch (const SvStl::MessageContainer& rContain)
			{
				SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
				MesMan.setMessage(rContain.getMessage());
				return TRUE;

			}
		}
		m_HighGrayscaleValue.Format (_T("%d"), msvulMaxGrayscale);
		m_LowGrayscaleValue.Format (_T("%d"), msvulMinGrayscale);

		BYTE Grayscale = m_Values.Get<BYTE>(SVPixelColorIndexObjectGuid);

		if (Grayscale == msvulMinGrayscale) //Black
		{
			SetBlack ();
		}
		else
			if (Grayscale == msvulMaxGrayscale) //White
			{
				SetWhite ();
			}
			else
			{
				SetOther(Grayscale);
			}

			m_GrayscaleSld.SetRangeMin (-((long)msvulMaxGrayscale), FALSE);
			m_GrayscaleSld.SetRangeMax (-((long)msvulMinGrayscale), FALSE);
			UpdateData (FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}


unsigned long SVPixelAnalyzerDlg::SetBlack()
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


unsigned long SVPixelAnalyzerDlg::SetWhite ()
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


unsigned long SVPixelAnalyzerDlg::SetOther (long alGrayscale)
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


unsigned long SVPixelAnalyzerDlg::SetGrayscale (long alGrayscale)
{
	unsigned long ret(0);
	m_GrayscaleToCount = alGrayscale;
	m_GrayscaleSliderValue = -alGrayscale;
	UpdateData(false);

	SetInspectionData();
	return ret;
}



void SVPixelAnalyzerDlg::OnWhiteRad() 
{
    SetWhite ();	
}


void SVPixelAnalyzerDlg::OnBlackRad() 
{
    SetBlack ();	
}


void SVPixelAnalyzerDlg::OnOtherRad() 
{
	BYTE    byGrayscale{ 0 };

	if (nullptr != m_pAnalyzer)
	{
		m_pAnalyzer->m_pixelCountColor.GetValue(byGrayscale);
	}
    SetOther (byGrayscale);	
}


void SVPixelAnalyzerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateData (TRUE);
	SetGrayscale (-m_GrayscaleSliderValue);
}

void SVPixelAnalyzerDlg::OnChangeGrayscaleEdt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData (TRUE);
	SetGrayscale (m_GrayscaleToCount);
	
}

void SVPixelAnalyzerDlg::OnCancel() 
{
	CDialog::OnCancel();
}


void SVPixelAnalyzerDlg::OnPixelSetRange() 
{
	if (nullptr != m_pAnalyzer)
	{
		SvOi::IObjectClass* pAnalyzerResult;

		pAnalyzerResult = m_pAnalyzer->GetResultObject();

		if (nullptr == pAnalyzerResult)
		{
			SvStl::MessageMgrStd  Exception(SvStl::LogAndDisplay);
			Exception.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16085);
		}
		else if (S_OK != SVSetupDialogManager::Instance().SetupDialog(pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), this))
		{
			SvStl::MessageMgrStd  Exception(SvStl::LogAndDisplay);
			Exception.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16086);;
		}
	}
}

