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


#include "stdafx.h"
#include "SVTADlgColorToolPage.h"
#include "SVColorTool.h"
#include "SVValueObject.h"
#include "SVIPDoc.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorToolPageClass property page

SVTADlgColorToolPageClass::SVTADlgColorToolPageClass( SVToolClass* PTool ) : CPropertyPage(SVTADlgColorToolPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVTADlgColorToolPageClass)
	m_convertToHSI = FALSE;
	//}}AFX_DATA_INIT

	m_pTool = (SVColorToolClass *)PTool;
	m_pConvertToHSI = m_pTool->GetConvertToHSIVariable();
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

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pTool );
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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgColorToolPage.cpp_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:21:16   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:18:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Apr 2011 16:39:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Aug 2005 09:45:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2005 08:15:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 15:49:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also use reset objects rather than recreating them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:12:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 13:36:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex to SetValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:41:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
