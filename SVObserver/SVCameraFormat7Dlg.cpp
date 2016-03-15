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
#include "SVImageLibrary/MatroxImageData.h"
#include "SVOGui\SVColor.h"
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

	// Set the Image
	m_Image.AddTab("Camera image");
	OnTakePicture();
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
	MatroxImageData data(m_pImageHandle);
	m_Image.setImage( &data );
	m_Image.Refresh();
}
#pragma endregion Private Methods

