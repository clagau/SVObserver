// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraFormat7Dlg
// * .File Name       : $Workfile:   SVCameraFormat7Dlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   28 Jul 2014 10:11:44  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraFormat7Dlg.h"
#include "svobserver.h"
#include "SVAcquisitionClass.h"
#include "SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVOMFCLibrary/DisplayHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(SVCameraFormat7Dlg, CDialog)
	//{{AFX_MSG_MAP(SVCameraFormat7Dlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaPosSpinHeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LEFT, OnDeltaPosSpinLeft)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TOP, OnDeltaPosSpinTop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaPosSpinWidth)
	ON_BN_CLICKED(IDC_TAKE_PICTURE, OnTakePicture)
	ON_EN_CHANGE(IDC_EDIT_TOP, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_LEFT, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeROI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVCameraFormat7Dlg, CDialog)
	ON_EVENT(SVCameraFormat7Dlg, IDC_DIALOGIMAGE, 8, SVCameraFormat7Dlg::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()
#pragma endregion Declarations

#pragma region Construction
SVCameraFormat7Dlg::SVCameraFormat7Dlg(CWnd* pParent /*=NULL*/)
: CDialog(SVCameraFormat7Dlg::IDD, pParent)
, m_iHeight( 0 )
, m_iLeft( 0 )
, m_iTop( 0 )
, m_iWidth( 0 )
, m_pFormat( nullptr )
, m_pDevice( nullptr )
, m_handleToOverlay( m_invalidHandle )
{
	//{{AFX_DATA_INIT(SVCameraFormat7Dlg)
	//}}AFX_DATA_INIT
}

SVCameraFormat7Dlg::~SVCameraFormat7Dlg()
{
	m_pImageHandle.clear();
}
#pragma endregion Construction

#pragma region Public Methods
void SVCameraFormat7Dlg::SetFormat( SVCameraFormat* pFormat )
{
	m_pFormat = pFormat;
}

void SVCameraFormat7Dlg::SetFormat7Image( const SVImageInfoClass& rInfo)
{
	m_ImageInfo = rInfo;
}

void SVCameraFormat7Dlg::SetAcquisitionDevice( SVAcquisitionClassPtr pDevice )
{
	m_pDevice = pDevice;
}
#pragma endregion Public Methods

#pragma region Protected Methods
#pragma region Virtual
void SVCameraFormat7Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCameraFormat7Dlg)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_Image);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_SpinHeight);
	DDX_Control(pDX, IDC_SPIN_LEFT, m_SpinLeft);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);
	DDX_Control(pDX, IDC_SPIN_TOP, m_SpinTop);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_iLeft);
	DDX_Text(pDX, IDC_EDIT_TOP, m_iTop);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SVCameraFormat7Dlg message handlers

BOOL SVCameraFormat7Dlg::OnInitDialog()
{
	ASSERT( nullptr != m_pFormat );

	m_iLeft = m_pFormat->lHPos;
	m_iTop = m_pFormat->lVPos;
	m_iWidth = m_pFormat->lWidth;
	m_iHeight = m_pFormat->lHeight;

	CDialog::OnInitDialog();

	m_SpinTop.SetRange(0, static_cast<short>(m_pFormat->lHeightMax - m_pFormat->lVPosStep) );
	m_SpinLeft.SetRange(0, static_cast<short>(m_pFormat->lWidthMax - m_pFormat->lHPosStep) );
	m_SpinWidth.SetRange( static_cast<short>(m_pFormat->lHStep), static_cast<short>(m_pFormat->lWidthMax) );
	m_SpinHeight.SetRange( static_cast<short>(m_pFormat->lVStep), static_cast<short>(m_pFormat->lHeightMax) );
	UDACCEL accel[10];
	UINT nNum = m_SpinTop.GetAccel( 10, accel );
	accel[1].nInc = m_pFormat->lVPosStep * 3;
	accel[2].nInc = m_pFormat->lVPosStep * 5;
	m_SpinTop.SetAccel( 3, accel );
	accel[1].nInc = m_pFormat->lHPosStep * 3;
	accel[2].nInc = m_pFormat->lHPosStep * 5;
	m_SpinLeft.SetAccel( 3, accel );
	accel[1].nInc = m_pFormat->lHStep * 3;
	accel[2].nInc = m_pFormat->lHStep * 5;
	m_SpinWidth.SetAccel( 3, accel );
	accel[1].nInc = m_pFormat->lVStep * 3;
	accel[2].nInc = m_pFormat->lVStep * 5;
	m_SpinHeight.SetAccel( 3, accel );

	if( m_pImageHandle.empty() )
	{
		SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, m_pImageHandle );
	}

	if( m_pDevice != nullptr )
	{
		m_pDevice->SingleGrab( m_pImageHandle );
	}

	// Set the Image
	m_Image.AddTab("Camera image");
	setImages();
	SetGraphicROI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVCameraFormat7Dlg::OnOK()
{
	UpdateData(TRUE);
	m_pFormat->lHPos = m_iLeft;
	m_pFormat->lVPos = m_iTop;
	m_pFormat->lWidth = m_iWidth;
	m_pFormat->lHeight = m_iHeight;
	CDialog::OnOK();
}

void SVCameraFormat7Dlg::OnCancel()
{
	CDialog::OnCancel();
}
#pragma endregion Virtual

void SVCameraFormat7Dlg::OnDeltaPosSpinHeight(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	OnDeltaPosSpin( pNMHDR );

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lVStep;

	if ( m_iHeight + pnm->iDelta < m_pFormat->lVStep )
	{
		*pResult = S_FALSE;
	}

	if ( m_iTop + m_iHeight + pnm->iDelta > m_pFormat->lHeightMax )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iHeight = std::max< int >( m_pFormat->lVStep, std::min< int >( m_iHeight + pnm->iDelta, m_pFormat->lHeightMax ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVCameraFormat7Dlg::OnDeltaPosSpinWidth(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	OnDeltaPosSpin( pNMHDR );

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lHStep;

	if ( m_iWidth + pnm->iDelta < m_pFormat->lHStep )
	{
		*pResult = S_FALSE;
	}

	if ( m_iLeft + m_iWidth + pnm->iDelta > m_pFormat->lWidthMax )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iWidth = std::max< int >( m_pFormat->lHStep, min< int >( m_iWidth + pnm->iDelta, m_pFormat->lWidthMax ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVCameraFormat7Dlg::OnDeltaPosSpinLeft(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	OnDeltaPosSpin( pNMHDR );

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lHPosStep;

	if ( m_iLeft + pnm->iDelta < 0 )
	{
		*pResult = S_FALSE;
	}

	if ( m_iLeft + m_iWidth + pnm->iDelta > m_pFormat->lWidthMax )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iLeft = std::max< int >( 0, min< int >( m_iLeft + pnm->iDelta, m_pFormat->lWidthMax - m_pFormat->lHPosStep ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVCameraFormat7Dlg::OnDeltaPosSpinTop(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	OnDeltaPosSpin( pNMHDR );

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lVPosStep;

	if ( m_iTop + pnm->iDelta < 0 )
	{
		*pResult = S_FALSE;
	}

	if ( m_iTop + m_iHeight + pnm->iDelta > m_pFormat->lHeightMax )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iTop = std::max< int >( 0, std::min< int >( m_iTop + pnm->iDelta, m_pFormat->lHeightMax - m_pFormat->lVPosStep ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVCameraFormat7Dlg::OnTakePicture()
{
	if( m_pDevice != nullptr )
	{
		m_pDevice->SingleGrab( m_pImageHandle );
	}

	setImages();
}

void SVCameraFormat7Dlg::OnChangeROI()
{
	if ( nullptr != m_SpinHeight.GetSafeHwnd()
		&& nullptr != m_SpinWidth.GetSafeHwnd()
		&& nullptr != m_SpinTop.GetSafeHwnd()
		&& nullptr != m_SpinLeft.GetSafeHwnd() )
	{
		UpdateData();

		CRect l_oRect;

		l_oRect.top = m_iTop;
		l_oRect.left = m_iLeft;
		l_oRect.bottom = m_iTop + m_iHeight;
		l_oRect.right = m_iLeft + m_iWidth;

		Normalize( l_oRect );

		m_iTop    = l_oRect.top;
		m_iLeft   = l_oRect.left;
		m_iWidth  = l_oRect.Width();
		m_iHeight = l_oRect.Height();

		UpdateData(FALSE);

		SetGraphicROI();
	}
}

void SVCameraFormat7Dlg::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	std::map<long,_variant_t> ParaMap;
	int count = Seidenader::SVOMFCLibrary::DisplayHelper::FillParameterMap(ParaMap,ParameterList,ParameterValue);

	m_iWidth = ParaMap[CDSVPictureDisplay::P_X2].lVal - ParaMap[CDSVPictureDisplay::P_X1].lVal;
	m_iHeight = ParaMap[CDSVPictureDisplay::P_Y2].lVal - ParaMap[CDSVPictureDisplay::P_Y1].lVal;
	m_iLeft = ParaMap[CDSVPictureDisplay::P_X1].lVal;
	m_iTop = ParaMap[CDSVPictureDisplay::P_Y1].lVal;
	UpdateData(FALSE);
	OnChangeROI();
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVCameraFormat7Dlg::SetGraphicROI()
{
	std::map<long,long> ParMap;
	ParMap[ CDSVPictureDisplay::P_Type ] = CDSVPictureDisplay::RectangleROI;
	ParMap[ CDSVPictureDisplay::P_X1 ] = m_iLeft;
	ParMap[ CDSVPictureDisplay::P_Y1 ] = m_iTop;
	ParMap[ CDSVPictureDisplay::P_X2 ] = m_iLeft + m_iWidth;
	ParMap[ CDSVPictureDisplay::P_Y2 ] = m_iTop + m_iHeight;
	ParMap[ CDSVPictureDisplay::P_Color ] = SVColor::Green;
	ParMap[ CDSVPictureDisplay::P_SelectedColor ] = SVColor::Green;
	ParMap[ CDSVPictureDisplay::P_AllowEdit ] = CDSVPictureDisplay::AllowAll;

	if (m_invalidHandle == m_handleToOverlay )
	{
		m_Image.AddOverlay(0, ParMap, &m_handleToOverlay);
	}
	else
	{
		m_Image.EditOverlay(0, m_handleToOverlay, ParMap);
	}
}

void SVCameraFormat7Dlg::OnDeltaPosSpin( NMHDR* pNMHDR )
{
	NMUPDOWN* pnm = reinterpret_cast< NMUPDOWN* >( pNMHDR );
}

void SVCameraFormat7Dlg::Normalize( CRect &l_roRect )
{
	if ( l_roRect.Width() < m_pFormat->lHStep )
	{
		l_roRect.right = m_pFormat->lHStep + l_roRect.left;
	}

	if ( l_roRect.Height() < m_pFormat->lVStep )
	{
		l_roRect.bottom = m_pFormat->lVStep + l_roRect.top;
	}

	// normalize to step
	l_roRect.top    = (l_roRect.top / m_pFormat->lVPosStep) * m_pFormat->lVPosStep;
	l_roRect.left   = (l_roRect.left / m_pFormat->lHPosStep) * m_pFormat->lHPosStep;
	l_roRect.right  = (l_roRect.Width()  / m_pFormat->lHStep) * m_pFormat->lHStep + l_roRect.left;
	l_roRect.bottom = (l_roRect.Height() / m_pFormat->lVStep) * m_pFormat->lVStep + l_roRect.top ;

	// crop
	l_roRect.NormalizeRect();
	l_roRect.top    = __max(0, l_roRect.top);
	l_roRect.left   = __max(0, l_roRect.left);
	l_roRect.right  = __min(m_pFormat->lWidthMax, l_roRect.right );
	l_roRect.bottom = __min(m_pFormat->lHeightMax, l_roRect.bottom );

	if ( l_roRect.Width() < m_pFormat->lHStep )
	{
		l_roRect.left += l_roRect.Width() - m_pFormat->lHStep;
	}

	if ( l_roRect.Height() < m_pFormat->lVStep )
	{
		l_roRect.top += l_roRect.Height() - m_pFormat->lVStep;
	}

	ASSERT( l_roRect.Width() >= 0 );
	ASSERT( l_roRect.Height() >= 0 );
}

void SVCameraFormat7Dlg::setImages()
{
	m_Image.setImage( m_pImageHandle );
	m_Image.Refresh();
}
#pragma endregion Private Methods

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCameraFormat7Dlg.cpp_v  $
 * 
 *    Rev 1.4   28 Jul 2014 10:11:44   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   bugfix: only set ROI position on the available grid by mouse moving of the ROI.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2014 13:06:44   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup (e.g. use static_cast instead of c-style cast)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2014 19:01:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to follow coding standards in preparation for upcoming changes related to SVPictureDisplay ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:45:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 May 2013 11:27:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetFormat7Image method arguments to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:49:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Mar 2011 15:04:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Mar 2011 07:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Mar 2011 13:23:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Feb 2011 14:33:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Nov 2010 13:30:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Jul 2010 16:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 11:17:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 Jul 2007 11:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2007 12:01:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5.1.0   14 Jul 2009 12:06:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code based on data manager and acquisition changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Mar 2006 14:35:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update dialog to have an image buffer attached to it so the user can use the mouse to adjust the ROI size and position.  The acquired image will get copied into the dialog buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 13:20:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Aug 2003 13:45:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general fixes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Aug 2003 09:00:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general "first iteration" code changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2003 12:43:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified code to grab and display camera image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jul 2003 10:34:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/