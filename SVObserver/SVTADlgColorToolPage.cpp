//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorToolPage
//* .File Name       : $Workfile:   SVTADlgColorToolPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgColorToolPage.h"
#include "SVColorTool.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVIPDoc.h"
#include "SVSetupDialogManager.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorToolPageClass property page

SVTADlgColorToolPageClass::SVTADlgColorToolPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID ) 
: CPropertyPage(SVTADlgColorToolPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVTADlgColorToolPageClass)
	m_convertToHSI = FALSE;
	//}}AFX_DATA_INIT

	m_pTool = dynamic_cast<SVColorToolClass *>(SvOi::getObject(rTaskObjectID));
	if (m_pTool)
	{
		m_pConvertToHSI = m_pTool->GetConvertToHSIVariable();
	}
}

SVTADlgColorToolPageClass::~SVTADlgColorToolPageClass()
{
}

HRESULT SVTADlgColorToolPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( m_pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( m_pConvertToHSI, m_convertToHSI );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
		}

		UpdateData( FALSE );
	}
	return l_hrOk;
}

void SVTADlgColorToolPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorToolPageClass)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	DDX_Check(pDX, IDC_CONVERT_TO_HSI, m_convertToHSI);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVTADlgColorToolPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgColorToolPageClass)
	ON_BN_CLICKED(IDC_CONVERT_TO_HSI, OnConvertToHsi)
	ON_BN_CLICKED(IDC_TRAIN_COLOR, OnTrainColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorToolPageClass message handlers

BOOL SVTADlgColorToolPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetTaskObject( m_pTool );

	if( m_pTool && m_pConvertToHSI )
	{
		// Get the convert to HSI state
		m_pConvertToHSI->GetValue( m_convertToHSI );
		m_dialogImage.AddTab(_T("Tool Input")); 
		setImages();
		UpdateData( FALSE ); // set data to dialog
	}
	else
		// Not valid call...
		if( GetParent() )
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVTADlgColorToolPageClass::OnConvertToHsi() 
{
	SetInspectionData();
}

void SVTADlgColorToolPageClass::OnTrainColor() 
{
	SVSetupDialogManager::Instance().SetupDialog( m_pTool->GetClassID(), m_pTool->GetUniqueObjectID(), this );
}

void SVTADlgColorToolPageClass::setImages()
{
	if (nullptr != m_pTool)
	{
		// Get the Image to Display
		SVImageClass* pImage = m_pTool->GetRGBImage();
		if( nullptr != pImage )
		{
			m_dialogImage.setImage( pImage, 0 );
			m_dialogImage.Refresh();
		}
	}
}

