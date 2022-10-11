//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/ISVOApp_Helper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
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

SVPixelAnalyzerDlg::SVPixelAnalyzerDlg(uint32_t inspectionId, uint32_t taskObjectId, CWnd* pParent /*=nullptr*/)
: CDialog(SVPixelAnalyzerDlg::IDD, pParent)
, m_InspectionID{ inspectionId }
, m_TaskObjectID{ taskObjectId }
, m_Values{ BoundValues{ inspectionId, taskObjectId } }
{
}

SVPixelAnalyzerDlg::~SVPixelAnalyzerDlg()
{
}

void SVPixelAnalyzerDlg::SetInspectionData()
{
	m_Values.Set<BYTE>(SvPb::PixelColorIndexEId, static_cast<BYTE> (m_GrayscaleToCount));
	m_Values.Commit();
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

	if (SvDef::InvalidObjectId == m_TaskObjectID)
	{
		SvStl::MessageManager  Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Exception.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
	}
	else
	{
		msvulMinGrayscale = m_Values.GetMinValue<BYTE>(SvPb::PixelColorIndexEId);
		msvulMaxGrayscale = m_Values.GetMaxValue<BYTE>(SvPb::PixelColorIndexEId);
		m_HighGrayscaleValue.Format (_T("%lu"), msvulMaxGrayscale);
		m_LowGrayscaleValue.Format (_T("%lu"), msvulMinGrayscale);

		BYTE Grayscale = m_Values.Get<BYTE>(SvPb::PixelColorIndexEId);

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


void SVPixelAnalyzerDlg::SetBlack()
{
	m_WhiteRad.SetCheck(BST_UNCHECKED);
	m_BlackRad.SetCheck (BST_CHECKED);
	m_OtherRad.SetCheck(BST_UNCHECKED);
	m_GrayscaleSld.EnableWindow (false);
	m_GrayScaleEdt.EnableWindow (false);
	SetGrayscale(msvulMinGrayscale);
}


void SVPixelAnalyzerDlg::SetWhite ()
{
	m_WhiteRad.SetCheck(BST_CHECKED);
	m_BlackRad.SetCheck (BST_UNCHECKED);
	m_OtherRad.SetCheck(BST_UNCHECKED);
	m_GrayscaleSld.EnableWindow (false);
	m_GrayScaleEdt.EnableWindow (false);
	SetGrayscale(msvulMaxGrayscale);
}


void SVPixelAnalyzerDlg::SetOther (long lGrayscale)
{
	m_WhiteRad.SetCheck(BST_UNCHECKED);
	m_BlackRad.SetCheck (BST_UNCHECKED);
	m_OtherRad.SetCheck(BST_CHECKED);
	m_GrayscaleSld.EnableWindow (true);
	m_GrayScaleEdt.EnableWindow (true);

	SetGrayscale (lGrayscale);
}


void SVPixelAnalyzerDlg::SetGrayscale(long alGrayscale)
{
	m_GrayscaleToCount = alGrayscale;
	m_GrayscaleSliderValue = -alGrayscale;
	UpdateData(false);

	SetInspectionData();
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
	BYTE    byGrayscale = m_Values.Get<BYTE>(SvPb::PixelColorIndexEId);
    SetOther (byGrayscale);	
}


void SVPixelAnalyzerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateData(true);
	SetGrayscale(-m_GrayscaleSliderValue);
}

void SVPixelAnalyzerDlg::OnChangeGrayscaleEdt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData(true);
	SetGrayscale(m_GrayscaleToCount);
	
}

void SVPixelAnalyzerDlg::OnCancel() 
{
	CDialog::OnCancel();
}


void SVPixelAnalyzerDlg::OnPixelSetRange() 
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(m_TaskObjectID);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVResultObjectType);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK != hr || !responseCmd.has_getobjectidresponse())
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log);
		MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
	}
	else
	{
		SvOi::SetupDialogManager(responseCmd.getobjectidresponse().classid(), m_InspectionID, responseCmd.getobjectidresponse().objectid(), GetSafeHwnd()); //if directly call of SetupDialog this-pointer is possible.
	}
}

} //namespace SvOg