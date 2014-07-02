//******************************************************************************
//* COPYRIGHT (c) 20132 by Seidenader Vision Inc, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraROIDlg
//* .File Name       : $Workfile:   SVGigeCameraROIDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Jul 2014 13:06:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVGigeCameraROIDlg.h"
#include "SVAcquisitionClass.h"
#include "SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/DisplayHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(SVGigeCameraROIDlg, CDialog)
	//{{AFX_MSG_MAP(SVGigeCameraROIDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaPosSpinHeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LEFT, OnDeltaPosSpinLeft)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TOP, OnDeltaPosSpinTop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaPosSpinWidth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VERT, OnDeltaPosSpinBinningVert)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HORIZ, OnDeltaPosSpinBinningHoriz)
	ON_BN_CLICKED(IDC_TAKE_PICTURE, OnTakePicture)
	ON_EN_CHANGE(IDC_EDIT_TOP, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_LEFT, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeROI)
	ON_EN_CHANGE(IDC_EDIT_VERT, OnChangeBinningVert)
	ON_EN_CHANGE(IDC_EDIT_VERT, OnChangeBinningHoriz)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SVGigeCameraROIDlg, CDialog)
	ON_EVENT(SVGigeCameraROIDlg, IDC_DIALOGIMAGE, 8, SVGigeCameraROIDlg::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVGigeCameraROIDlg::SVGigeCameraROIDlg(ISVCameraDeviceImageFormatUpdater& rUpdater, CWnd* pParent /*=NULL*/)
: CDialog(SVGigeCameraROIDlg::IDD, pParent)
, m_rImageFormatUpdater(rUpdater)
, m_pDevice(nullptr) 
, m_pFormat(nullptr)
, m_verticalBinningParam(nullptr)
, m_horizontalBinningParam(nullptr)
, m_verticalBinningOriginalValue(1)
, m_horizontalBinningOriginalValue(1)
, m_iHeight( 0 )
, m_iLeft( 0 )
, m_iTop( 0 )
, m_iWidth( 0 )
, m_handleToOverlay( m_invalidHandle )
{
	//{{AFX_DATA_INIT(SVGigeCameraROIDlg)
	//}}AFX_DATA_INIT
}

SVGigeCameraROIDlg::~SVGigeCameraROIDlg()
{
	m_pImageHandle.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
void SVGigeCameraROIDlg::SetFormat( SVCameraFormat* pFormat )
{
	m_pFormat = pFormat;
}

void SVGigeCameraROIDlg::SetFormatImage( const SVImageInfoClass& rInfo)
{
	m_ImageInfo = rInfo;
}

void SVGigeCameraROIDlg::SetAcquisitionDevice( SVAcquisitionClassPtr pDevice )
{
	m_pDevice = pDevice;
}

const SVLongValueDeviceParam* SVGigeCameraROIDlg::GetVerticalBinningParam() const
{
	return m_verticalBinningParam; 
}

void SVGigeCameraROIDlg::SetVerticalBinningParam(SVLongValueDeviceParam* pParam)
{
	m_verticalBinningParam = pParam;
	m_verticalBinning.bExist = true;
	m_verticalBinning.minValue = pParam->info.min;
	m_verticalBinning.maxValue = pParam->info.max;
	m_verticalBinning.value = pParam->lValue;
	m_verticalBinningOriginalValue = m_verticalBinning.value;
}

const SVLongValueDeviceParam* SVGigeCameraROIDlg::GetHorizontalBinningParam() const
{
	return m_horizontalBinningParam;
}

void SVGigeCameraROIDlg::SetHorizontalBinningParam(SVLongValueDeviceParam* pParam)
{
	m_horizontalBinningParam = pParam;
	m_horizontalBinning.bExist = true;
	m_horizontalBinning.minValue = pParam->info.min;
	m_horizontalBinning.maxValue = pParam->info.max;
	m_horizontalBinning.value = pParam->lValue;
	m_horizontalBinningOriginalValue = m_horizontalBinning.value;
}
#pragma endregion Public Methods

#pragma region Protected Methods
#pragma region Virtual
void SVGigeCameraROIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVGigeCameraROIDlg)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_Image);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_SpinHeight);
	DDX_Control(pDX, IDC_SPIN_LEFT, m_SpinLeft);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);
	DDX_Control(pDX, IDC_SPIN_TOP, m_SpinTop);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_iLeft);
	DDX_Text(pDX, IDC_EDIT_TOP, m_iTop);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	DDX_Control(pDX, IDC_STATIC_BINNING, m_BinningGroupBox);
	DDX_Control(pDX, IDC_SPIN_VERT, m_SpinVerticalBinning);
	DDX_Control(pDX, IDC_EDIT_VERT, m_VerticalBinningEdit);
	DDX_Control(pDX, IDC_STATIC_VERT, m_VerticalBinningLabel);
	DDX_Control(pDX, IDC_SPIN_HORIZ, m_SpinHorizontalBinning);
	DDX_Control(pDX, IDC_EDIT_HORIZ, m_HorizontalBinningEdit);
	DDX_Control(pDX, IDC_STATIC_HORIZ, m_HorizontalBinningLabel);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SVGigeCameraROIDlg message handlers

BOOL SVGigeCameraROIDlg::OnInitDialog()
{
	m_iLeft = m_pFormat->lHPos;
	m_iTop = m_pFormat->lVPos;
	m_iWidth = m_pFormat->lWidth;
	m_iHeight = m_pFormat->lHeight;

	CDialog::OnInitDialog();

	UpdateSpinTopRange();
	UpdateSpinLeftRange();
	UpdateSpinWidthRange();
	UpdateSpinHeightRange();

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

	ShowBinningGroup();
	SetupVerticalBinning();
	SetupHorizontalBinning();

	if( m_pImageHandle.empty() )
	{
		SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, m_pImageHandle );
	}

	if( m_pDevice != NULL )
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

void SVGigeCameraROIDlg::OnOK()
{
	UpdateData(TRUE);
	m_pFormat->lHPos = m_iLeft;
	m_pFormat->lVPos = m_iTop;
	m_pFormat->lWidth = m_iWidth;
	m_pFormat->lHeight = m_iHeight;
	CDialog::OnOK();
}

void SVGigeCameraROIDlg::OnCancel()
{
	RestoreBinning();
	CDialog::OnCancel();
}
#pragma endregion Virtual

void SVGigeCameraROIDlg::OnDeltaPosSpinHeight(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lVStep;
	long maxHeight = GetScaledMaxHeight();

	if ( (m_iHeight + pnm->iDelta) < m_pFormat->lVStep )
	{
		*pResult = S_FALSE;
	}
	
	if ( ( m_iTop + m_iHeight + pnm->iDelta ) > maxHeight )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iHeight = std::max< int >( m_pFormat->lVStep, std::min< int >( m_iHeight + pnm->iDelta, maxHeight ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVGigeCameraROIDlg::OnDeltaPosSpinWidth(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lHStep;

	long maxWidth = GetScaledMaxWidth();

	if ( ( m_iWidth + pnm->iDelta ) < m_pFormat->lHStep )
	{
		*pResult = S_FALSE;
	}

	if ( ( m_iLeft + m_iWidth + pnm->iDelta ) > maxWidth )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iWidth = std::max< int >( m_pFormat->lHStep, std::min< int >( m_iWidth + pnm->iDelta, maxWidth ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVGigeCameraROIDlg::OnDeltaPosSpinLeft(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lHPosStep;
	long maxWidth = GetScaledMaxWidth();

	if ( ( m_iLeft + pnm->iDelta ) < 0 )
	{
		*pResult = S_FALSE;
	}

	if ( ( m_iLeft + m_iWidth + pnm->iDelta ) > maxWidth )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iLeft = std::max< int >( 0, std::min< int >( m_iLeft + pnm->iDelta, maxWidth - m_pFormat->lHPosStep ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVGigeCameraROIDlg::OnDeltaPosSpinTop(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = (NMUPDOWN*) pNMHDR;
	pnm->iDelta *= m_pFormat->lVPosStep;
	long maxHeight = GetScaledMaxHeight();

	if ( (m_iTop + pnm->iDelta) < 0 )
	{
		*pResult = S_FALSE;
	}

	if ( (m_iTop + m_iHeight + pnm->iDelta) > maxHeight )
	{
		*pResult = S_FALSE;
	}

	if ( *pResult == S_OK )
	{
		m_iTop = std::max< int >( 0, std::min< int >( m_iTop + pnm->iDelta, maxHeight - m_pFormat->lVPosStep ) );
		UpdateData(FALSE);
		OnChangeROI();
	}
}

void SVGigeCameraROIDlg::OnDeltaPosSpinBinningVert(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = reinterpret_cast< NMUPDOWN* >( pNMHDR );

	long prevValue = m_verticalBinningParam->lValue;
	long newValue = m_verticalBinning.value + pnm->iDelta;
	if (prevValue == newValue || newValue < m_verticalBinning.minValue || newValue > m_verticalBinning.maxValue)
	{
		*pResult = S_FALSE;
	}
	else
	{
		CEdit* pEdit = dynamic_cast<CEdit *>(GetDlgItem(IDC_EDIT_VERT));
		if (pEdit)
		{
			m_verticalBinningParam->lValue = newValue;
			m_verticalBinning.value = newValue;

			// Update the control
			CString strVal;
			strVal.Format("%d", m_verticalBinning.value);
			pEdit->SetWindowText(strVal);

			double scaleFactor = static_cast<double>(prevValue) / static_cast<double>(m_verticalBinning.value);
			m_iHeight = static_cast<long>(GetScaledValue(m_iHeight, scaleFactor));
			m_iTop = static_cast<long>(GetScaledValue(m_iTop, scaleFactor));

			// Set Device parameters
			UpdateDeviceBinningParams();
			UpdateSpinHeightRange();
			UpdateSpinTopRange();
			UpdateData(false);

			UpdateImageDisplay();
		}
	}
}

void SVGigeCameraROIDlg::OnDeltaPosSpinBinningHoriz(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NMUPDOWN* pnm = reinterpret_cast< NMUPDOWN* >( pNMHDR );

	long prevValue = m_horizontalBinningParam->lValue;
	long newValue = m_horizontalBinning.value + pnm->iDelta;
	if (prevValue == newValue || newValue < m_horizontalBinning.minValue || newValue > m_horizontalBinning.maxValue)
	{
		*pResult = S_FALSE;
	}
	else
	{
		CEdit* pEdit = dynamic_cast<CEdit *>(GetDlgItem(IDC_EDIT_HORIZ));
		if (pEdit)
		{
			m_horizontalBinningParam->lValue = newValue;
			m_horizontalBinning.value = newValue;

			// Update the control
			CString strVal;
			strVal.Format("%d", m_horizontalBinning.value);
			pEdit->SetWindowText(strVal);

			double scaleFactor = static_cast<double>(prevValue) / static_cast<double>(m_horizontalBinning.value);
			m_iWidth = static_cast<long>(GetScaledValue(m_iWidth, scaleFactor));
			m_iLeft = static_cast<long>(GetScaledValue(m_iLeft, scaleFactor));

			// Set Device parameters
			UpdateDeviceBinningParams();

			UpdateSpinWidthRange();
			UpdateSpinLeftRange();
			UpdateData(false);

			UpdateImageDisplay();
		}
	}
}

void SVGigeCameraROIDlg::OnTakePicture()
{
	if( m_pDevice != NULL )
	{
		m_pDevice->SingleGrab( m_pImageHandle );
	}
	setImages();
}

void SVGigeCameraROIDlg::OnChangeROI()
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

// this gets called before the Spin button updates, and we can't edit the text directly...
// So what is the point of catching this event?
void SVGigeCameraROIDlg::OnChangeBinningVert()
{
}

// this gets called before the Spin button updates, and we can't edit the text directly...
// So what is the point of catching this event?
void SVGigeCameraROIDlg::OnChangeBinningHoriz()
{
}

void SVGigeCameraROIDlg::ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	std::map<long,_variant_t> ParaMap;
	int count = Seidenader::SVOMFCLibrary::DisplayHelper::FillParameterMap(ParaMap,ParameterList,ParameterValue);

	m_iWidth = ParaMap[CDSVPictureDisplay::P_X2].lVal - ParaMap[CDSVPictureDisplay::P_X1].lVal;
	m_iHeight = ParaMap[CDSVPictureDisplay::P_Y2].lVal - ParaMap[CDSVPictureDisplay::P_Y1].lVal;
	m_iLeft = ParaMap[CDSVPictureDisplay::P_X1].lVal;
	m_iTop = ParaMap[CDSVPictureDisplay::P_Y1].lVal;
	UpdateData(FALSE);
	SetGraphicROI();
}
#pragma endregion Protected Methods

#pragma region Private Methods

void SVGigeCameraROIDlg::RestoreBinning()
{
	// restore binning params
	if (m_verticalBinning.bExist && m_verticalBinningParam)
	{
		double scaleFactor = static_cast<double>(m_verticalBinning.value ) / static_cast<double>(m_verticalBinningOriginalValue);
		m_iHeight = static_cast<long>(GetScaledValue(m_iHeight, scaleFactor));
		m_iTop = static_cast<long>(GetScaledValue(m_iTop, scaleFactor));

		m_verticalBinning.value = m_verticalBinningOriginalValue;
		m_verticalBinningParam->lValue = m_verticalBinningOriginalValue;
	}

	if (m_horizontalBinning.bExist && m_horizontalBinningParam)
	{
		double scaleFactor = static_cast<double>(m_horizontalBinning.value) / static_cast<double>(m_horizontalBinningOriginalValue);
		m_iWidth = static_cast<long>(GetScaledValue(m_iWidth, scaleFactor));
		m_iLeft = static_cast<long>(GetScaledValue(m_iLeft, scaleFactor));

		m_horizontalBinning.value = m_horizontalBinningOriginalValue;
		m_horizontalBinningParam->lValue = m_horizontalBinningOriginalValue;
	}

	UpdateDeviceBinningParams();
}

void SVGigeCameraROIDlg::SetGraphicROI()
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

void SVGigeCameraROIDlg::SetupVerticalBinning()
{
	CString strValue;
	strValue.Format("%d", m_verticalBinning.value);
	m_VerticalBinningEdit.SetWindowText(strValue);

	m_SpinVerticalBinning.SetRange(static_cast<short>(m_verticalBinning.minValue), static_cast<short>(m_verticalBinning.maxValue));

	int nShow = (m_verticalBinning.bExist) ? SW_SHOW : SW_HIDE;
	m_VerticalBinningLabel.ShowWindow(nShow);
	m_VerticalBinningEdit.ShowWindow(nShow);
	m_VerticalBinningEdit.EnableWindow(m_verticalBinning.bExist);
	m_SpinVerticalBinning.ShowWindow(nShow);
	m_SpinVerticalBinning.EnableWindow(m_verticalBinning.bExist);
}

void SVGigeCameraROIDlg::SetupHorizontalBinning()
{
	CString strValue;
	strValue.Format("%d", m_horizontalBinning.value);
	m_HorizontalBinningEdit.SetWindowText(strValue);

	m_SpinHorizontalBinning.SetRange(static_cast<short>(m_horizontalBinning.minValue), static_cast<short>(m_horizontalBinning.maxValue));

	int nShow = (m_horizontalBinning.bExist) ? SW_SHOW : SW_HIDE;
	m_HorizontalBinningLabel.ShowWindow(nShow);
	m_HorizontalBinningEdit.ShowWindow(nShow);
	m_HorizontalBinningEdit.EnableWindow(m_horizontalBinning.bExist);
	m_SpinHorizontalBinning.ShowWindow(nShow);
	m_SpinHorizontalBinning.EnableWindow(m_horizontalBinning.bExist);
}

void SVGigeCameraROIDlg::ShowBinningGroup()
{
	int nShow = (!m_horizontalBinning.bExist && ! m_verticalBinning.bExist) ? SW_HIDE : SW_SHOW;
	m_BinningGroupBox.ShowWindow(nShow);
}

void SVGigeCameraROIDlg::UpdateImageDisplay()
{
	m_ImageInfo.SetExtentProperty(SVExtentPropertyWidth, GetScaledMaxWidth());
	m_ImageInfo.SetExtentProperty(SVExtentPropertyHeight, GetScaledMaxHeight());

	SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, m_pImageHandle );

	OnTakePicture();

	// Redraw the ROI
	OnChangeROI();
	setImages();
}

void SVGigeCameraROIDlg::UpdateDeviceBinningParams()
{
	if ( m_pDevice != nullptr )
	{
		// Set Device parameters
		SVDeviceParamCollection l_DeviceParams;
		if (m_horizontalBinningParam)
		{
			l_DeviceParams.SetParameter(SVDeviceParamWrapper(m_horizontalBinningParam));
		}
		if (m_verticalBinningParam)
		{
			l_DeviceParams.SetParameter(SVDeviceParamWrapper(m_verticalBinningParam));
		}
		if (l_DeviceParams.mapParameters.size())
		{
			m_pDevice->SetDeviceParameters(l_DeviceParams);
		}

		m_rImageFormatUpdater.SetWidth(GetScaledMaxWidth());
		m_rImageFormatUpdater.SetHeight(GetScaledMaxHeight());
		m_rImageFormatUpdater.Update(m_pDevice);
	}
}

void SVGigeCameraROIDlg::Normalize( CRect &l_roRect )
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
	//l_roRect.top    %= m_pFormat->lVPosStep;
	l_roRect.top    = (l_roRect.top / m_pFormat->lVPosStep) * m_pFormat->lVPosStep;
	l_roRect.left   = (l_roRect.left / m_pFormat->lHPosStep) * m_pFormat->lHPosStep;
	l_roRect.right  = (l_roRect.Width() / m_pFormat->lHStep) * m_pFormat->lHStep + l_roRect.left;
	l_roRect.bottom = (l_roRect.Height() / m_pFormat->lVStep) * m_pFormat->lVStep + l_roRect.top;

	// crop
	l_roRect.NormalizeRect();
	l_roRect.top    = std::max<long>(0L, l_roRect.top);
	l_roRect.left   = std::max<long>(0L, l_roRect.left);
	l_roRect.right  = std::min<long>((m_pFormat->lWidthMax / m_horizontalBinning.value), l_roRect.right);
	l_roRect.bottom = std::min<long>((m_pFormat->lHeightMax / m_verticalBinning.value), l_roRect.bottom);

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

void SVGigeCameraROIDlg::UpdateSpinTopRange()
{
	m_SpinTop.SetRange(0, static_cast<short>(GetScaledMaxHeight() - m_pFormat->lVPosStep) );
}

void SVGigeCameraROIDlg::UpdateSpinLeftRange()
{
	m_SpinLeft.SetRange(0, static_cast<short>(GetScaledMaxWidth() - m_pFormat->lHPosStep) );
}

void SVGigeCameraROIDlg::UpdateSpinWidthRange()
{
	m_SpinWidth.SetRange(static_cast<short>(m_pFormat->lHStep), static_cast<short>(GetScaledMaxWidth()));
}

void SVGigeCameraROIDlg::UpdateSpinHeightRange()
{
	m_SpinHeight.SetRange(static_cast<short>(m_pFormat->lVStep), static_cast<short>(GetScaledMaxHeight()));
}

long SVGigeCameraROIDlg::GetScaledMaxWidth() const
{
	return m_pFormat->lWidthMax / m_horizontalBinning.value;
}

long SVGigeCameraROIDlg::GetScaledMaxHeight() const
{
	return m_pFormat->lHeightMax / m_verticalBinning.value;
}

double SVGigeCameraROIDlg::GetScaledValue(long value, double scaleFactor) const
{
	return static_cast<double>(value) * scaleFactor;
}

void SVGigeCameraROIDlg::setImages()
{
	m_Image.setImage( m_pImageHandle );
	m_Image.Refresh();
}
#pragma endregion Private Methods
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGigeCameraROIDlg.cpp_v  $
 * 
 *    Rev 1.3   02 Jul 2014 13:06:46   mziegler
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
 *    Rev 1.1   11 Jun 2013 15:26:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 May 2013 18:26:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix acquisition image size issue when disabling binning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 May 2013 19:45:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 May 2013 09:51:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
