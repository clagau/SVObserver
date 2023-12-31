//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutDlg
//* .File Name       : $Workfile:   SVLutDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:19:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <typeinfo.h>
#include "SVLutDlg.h"
#include "Definitions/SVUserMessage.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVLutTransformOperationMap SVLutDlgPage::m_mapOperations;
const CPoint SVLutDlgPage::m_ptNoLastMousePos(-1,-1);

BEGIN_MESSAGE_MAP(SVLutDlg, CPropertySheet)
	//{{AFX_MSG_MAP(SVLutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVLutDlg::SVLutDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

SVLutDlg::~SVLutDlg()
{
	DestroyAllPages();
}

bool SVLutDlg::Create(SvIe::SVVirtualCameraPtrVector& rCameraVector, SVLutMap& raLut)
{
	for(auto* pCamera : rCameraVector)
	{
		if( nullptr != pCamera )
		{
			SvIe::SVAcquisitionClassPtr pAcqDevice = pCamera->GetAcquisitionDevice();

			if(nullptr != pAcqDevice && raLut.find( pCamera->getObjectId() ) != raLut.end() && 0 < raLut[pCamera->getObjectId()].NumBands() )
			{
				std::string Caption = std::format("{} (Dig_{:d}.Ch_{:d})", pCamera->GetName(), pAcqDevice->DigNumber(), pAcqDevice->Channel());

				SVLutDlgPage* pPage;
				pPage = new SVLutDlgPage( this, Caption.c_str() );
				pPage->m_Lut = raLut[ pCamera->getObjectId() ];
				pPage->m_pCamera = pCamera;
				AddPage(pPage);
			}
		}
	}

	if( GetPageCount() < 1 )
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LUT_NotAvailable, SvStl::SourceFileParams(StdMessageParams));

		DestroyAllPages();

		return false;
	}
	return true;
}


void SVLutDlg::DestroyAllPages()
{
	CPropertyPage* pPage;
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		pPage = GetPage( i );
		RemovePage( i );
		if( pPage )
		{
			delete pPage;
		}
	}
}

void SVLutDlg::OnClose()
{
	CPropertySheet::OnClose();
}

BOOL SVLutDlg::OnInitDialog()
{
    BOOL bReturn = CPropertySheet::OnInitDialog();
    return bReturn;
}

BEGIN_MESSAGE_MAP(SVLutDlgPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVLutDlgPage)
	ON_BN_CLICKED(IDC_ACTIVATE_CHECK, OnActivateCheck)
	ON_CBN_SELCHANGE(IDC_LUT_MODE_COMBO, OnSelchangeLutModeCombo)
	ON_BN_CLICKED(IDC_CONTINUOUS_RECALC_CHECK, OnContinuousRecalcCheck)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_COLOR_BAND_ALL, OnColorBandAll)
	ON_BN_CLICKED(IDC_COLOR_BAND_BLUE, OnColorBandBlue)
	ON_BN_CLICKED(IDC_COLOR_BAND_GREEN, OnColorBandGreen)
	ON_BN_CLICKED(IDC_COLOR_BAND_RED, OnColorBandRed)
	ON_BN_CLICKED(IDC_COLOR_BAND_SYNC, OnColorBandSync)
	ON_EN_CHANGE(IDC_X1, OnChangeX1)
	ON_EN_CHANGE(IDC_X2, OnChangeX2)
	ON_EN_CHANGE(IDC_Y1, OnChangeY1)
	ON_EN_CHANGE(IDC_Y2, OnChangeY2)
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_REFRESH_DIALOG, OnGraphRefresh )
END_MESSAGE_MAP()

SVLutDlgPage::SVLutDlgPage( SVLutDlg* pParent, LPCTSTR Caption )
: CPropertyPage(SVLutDlgPage::IDD)
, m_pCamera(nullptr)
{
	//{{AFX_DATA_INIT(SVLutDlgPage)
	m_UpperClipValue = _T("");
	m_LowerClipValue = _T("");
	m_bUseLUT = TRUE;
	m_bContinuousRecalcLUT = FALSE;
	m_iBand = -1;
	m_iX1 = 0;
	m_iX2 = 0;
	m_iY1 = 0;
	m_iY2 = 0;
	m_RangeXMax = _T("");
	m_RangeYMax = _T("");
	m_RangeXYMax = _T("");
	//}}AFX_DATA_INIT

	m_pParentDialog	= pParent;

    m_strCaption = Caption;
    m_psp.pszTitle = m_strCaption;
    m_psp.dwFlags |= PSP_USETITLE;
}

SVLutDlgPage::~SVLutDlgPage()
{
}

void SVLutDlgPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLutDlgPage)
	DDX_Control(pDX, IDC_UPPER_SLIDER, m_UpperSlider);
	DDX_Control(pDX, IDC_LOWER_SLIDER, m_LowerSlider);
	DDX_Control(pDX, IDC_LUT_GRAPH, m_LUTGraph);
	DDX_Control(pDX, IDC_LUT_MODE_COMBO, m_LutModeCombo);
	DDX_Text(pDX, IDC_UPPER_EDIT, m_UpperClipValue);
	DDX_Text(pDX, IDC_LOWER_EDIT, m_LowerClipValue);
	DDX_Check(pDX, IDC_ACTIVATE_CHECK, m_bUseLUT);
	DDX_Radio(pDX, IDC_COLOR_BAND_RED, m_iBand);
	DDX_Text(pDX, IDC_X1, m_iX1);
	DDX_Text(pDX, IDC_X2, m_iX2);
	DDX_Text(pDX, IDC_Y1, m_iY1);
	DDX_Text(pDX, IDC_Y2, m_iY2);
	DDX_Text(pDX, IDC_STATIC_RANGE_X_MAX, m_RangeXMax);
	DDX_Text(pDX, IDC_STATIC_RANGE_Y_MAX, m_RangeYMax);
	DDX_Text(pDX, IDC_STATIC_RANGE_XY_MAX, m_RangeXYMax);
	//}}AFX_DATA_MAP
}

BOOL SVLutDlgPage::OnInitDialog() 
{
	m_pCamera->GetLut(m_Lut);

	m_pCamera->GetBand(m_iCamBand);
	m_pCamera->GetBandSize(m_iBandWidth);
	m_iNumBands = m_Lut.NumBands();

	m_iCurrentBand = -1;
	m_rgbLineColor = RGB(0,0,0);

	m_ptLastMousePos = m_ptNoLastMousePos;

	UINT uiMaxValue = m_Lut.Info().MaxValue();
	if ( uiMaxValue == 0 )	// legacy support
	{
		if (3 == m_iNumBands)
		{
			m_lMaxLutValue = 1023;	// color
		}
		else
		{
			m_lMaxLutValue = 255;	// mono
		}
	}
	else
	{
		m_lMaxLutValue = uiMaxValue;
	}

	m_RangeYMax.Format("(0, %d)", m_lMaxLutValue);
	m_RangeXMax.Format("(%d, 0)", m_Lut.Info().BandSize()-1);
	m_RangeXYMax.Format("(%d, %d)", m_Lut.Info().BandSize()-1, m_lMaxLutValue);

	CPropertyPage::OnInitDialog();

	bool bAllVariablesSet = m_pParentDialog && m_pCamera;

	ASSERT ( bAllVariablesSet );

	if( bAllVariablesSet )
	{
		CWnd* pWnd{nullptr};

		// Set Normalize Mode of Graph Control...
		m_LUTGraph.SetNormalizeMode( SvMc::SVNormalizeXYMinMax );
		m_LUTGraph.SetXYMinMax(0, 0, m_Lut.Info().BandSize()-1, m_lMaxLutValue);

		if ( 3 == m_iBandWidth )
		{
			// show color band controls if color LUT
			pWnd = GetDlgItem(IDC_COLOR_BAND_SYNC);
			if (nullptr != pWnd)
			{
				(static_cast<CButton*> (pWnd))->SetCheck(TRUE);
			}

			bool bSame = true;
			for (unsigned int j=0; bSame && j < m_Lut.Info().BandSize(); j++)
			{
				unsigned long lutValue = m_Lut(0, j);
				for (int iBand=1; iBand < m_Lut.NumBands(); iBand++)
				{
					bSame = bSame && (m_Lut(iBand, j) == lutValue);
				}
			}
			if (!bSame)
			{
				m_iCurrentBand = 0;	// default to red
				m_rgbLineColor = RGB(234, 0, 0);
			}

		}// end if ( 3 == miBandWidth )
		else
		{
			 m_pCamera->GetBand(m_iCurrentBand);

			// hide color band controls if not color LUT
			if (nullptr != (pWnd = GetDlgItem(IDC_COLOR_BAND_ALL)))
			{
				pWnd->ShowWindow( SW_HIDE );
			}
			if (nullptr != (pWnd = GetDlgItem(IDC_COLOR_BAND_BLUE)))
			{
				pWnd->ShowWindow( SW_HIDE );
			}
			if (nullptr != (pWnd = GetDlgItem(IDC_COLOR_BAND_GREEN)))
			{
				pWnd->ShowWindow( SW_HIDE );
			}
			if (nullptr != (pWnd = GetDlgItem(IDC_COLOR_BAND_RED)))
			{
				pWnd->ShowWindow( SW_HIDE );
			}
			if (nullptr != (pWnd = GetDlgItem(IDC_COLOR_BAND_SYNC)))
			{
				pWnd->ShowWindow( SW_HIDE );
			}
		}

		// range is negative because the stupid slider implementation has min value hardcoded at the top.
		// we will have to manually take the abs of the slider pos.

		m_UpperSlider.SetRange( -m_lMaxLutValue, 0 );
		m_LowerSlider.SetRange( -m_lMaxLutValue, 0 );
		//Slider does not update correctly on an RGB, the position needs to be set to
		//something  and then set to what it needs to be set at
		m_LowerSlider.SetPos(-30);
		m_LowerSlider.SetPos(-1);// default pos; should be 0 but slider doesn't initially update properly if it is 0.
		m_UpperSlider.SetPos(-m_lMaxLutValue);	// default pos

		// fill Mode combo box
		for (auto const& rEntry : m_mapOperations.GetTypes())
		{
			const SVLutTransformOperationMap::SVLutTransformTypeInfo& rType = rEntry.second;
			if (rType.m_pType)
			{
				int iPos = m_LutModeCombo.AddString( rType.m_Type.c_str() );
				m_LutModeCombo.SetItemData( iPos, rType.m_eType );
				if (m_Lut.Info().GetTransformOperation())
				{
					if (typeid(*rType.m_pType) == typeid(*m_Lut.Info().GetTransformOperation()))
					{
						m_LutModeCombo.SetCurSel(iPos);
						m_eLutMode = rType.m_eType;
					}
				}
			}
		}

		Refresh();

		return TRUE;
	}
	else
	{
		// Not valid call...
		if( GetParent() )
		{
			GetParent()->SendMessage( WM_CLOSE );
		}
		else
		{
			SendMessage( WM_CLOSE );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}

void SVLutDlgPage::ShowBoundaryControls(bool bShow)
{
	UINT cmd = (bShow) ? SW_SHOW : SW_HIDE;
	CWnd* pWnd = GetDlgItem( IDC_UPPER_EDIT );
	if(nullptr != pWnd)
	{
		pWnd->ShowWindow( cmd );
	}
	if(nullptr != (pWnd = GetDlgItem( IDC_UPPER_SLIDER )))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_STATIC)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_STATIC)))
	{
		pWnd->ShowWindow( cmd );
	}
}

void SVLutDlgPage::ShowRangeControls(bool bShow)
{
	UINT cmd = (bShow) ? SW_SHOW : SW_HIDE;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_X1);
	if(nullptr != pWnd)
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_Y1)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_X2)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_Y2)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_X1)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_Y1)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_X2)))
	{
		pWnd->ShowWindow( cmd );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_Y2)))
	{
		pWnd->ShowWindow( cmd );
	}
}

void SVLutDlgPage::Refresh()
{
	if( true )
	{
		// Recalc LUT...
		//pLUTOperator->RecalcLUT();
		m_iBand = m_iCurrentBand != -1 ? m_iCurrentBand : 4;

		// Refresh LUT Graph...
		m_pCamera->GetLut(m_Lut);
		int iBandSize = m_Lut.Info().BandSize();
		m_alBandData.resize(iBandSize);

		int iSourceBand = m_iCurrentBand;
		if (-1 == m_iCurrentBand)
		{
			iSourceBand = 0;
		}
		for( int j = 0; j < iBandSize; j++ )
		{
			long lVal = 0;
			lVal = m_Lut(iSourceBand, j);
			m_alBandData[j] = lVal;
		}// end for

		m_LUTGraph.SetColor(m_rgbLineColor, FALSE);
		m_LUTGraph.SetPoints( m_alBandData );

		if( true /*pLUTMode*/ )
		{
			// refresh lut mode combo settings...
			std::string Mode;
			const SVLutTransformOperation* pOperation = nullptr;
			if (-1 == m_iCurrentBand)
			{
				pOperation = m_Lut.GetTransformOperation();
			}
			else
			{
				pOperation = m_Lut(m_iCurrentBand).GetTransformOperation();
			}
			Log_Assert( nullptr != pOperation );
			if (nullptr != pOperation)
			{
				const SVLutTransformOperationMap::SVLutTransformTypeInfo* pInfo = m_mapOperations.GetInfo(pOperation);
				if (pInfo)
				{
					Mode = pInfo->m_Type;
					m_eLutMode = pInfo->m_eType;
				}
			}

			// Set cur sel in combo box...
			m_LutModeCombo.SelectString( -1, Mode.c_str() );

			// Show or hide Controls depending on LUT Mode...
			switch( m_eLutMode )
			{
				case LutTransformTypeNormal:	// Identity...
				case LutTransformTypeInversion:
				case LutTransformTypeSign:
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );

					// Show no special controls...
					ShowControls(false);

					break;
				
				case LutTransformTypeClip:
					{
					// Refresh upper clip...
					// get pos from lut
					int iBand = m_iCurrentBand != -1 ? m_iCurrentBand : 0;
					int iMin = 9999999, iMax = 0;

					SVLutTransformParameters param;
					m_Lut(iBand).Info().GetTransformParameters(param);
					if (2 == param.size() )
					{
						iMin = param[0];
						iMax = param[1];
					}
					else
					{
						for (unsigned int i = 0; i < m_Lut.Info().BandSize(); i++)
						{
							iMin = std::min(iMin, static_cast<int>(m_Lut(iBand, i)));
							iMax = std::max(iMax, static_cast<int>(m_Lut(iBand, i)));
						}
					}
					
					long lUpperClip = iMax;
					m_UpperClipValue.Format( _T( "%d" ), lUpperClip );
					m_UpperSlider.SetPos( ( int ) -lUpperClip );

					// Refresh lower clip...
					long lLowerClip = iMin;
					m_LowerClipValue.Format( _T( "%d" ), lLowerClip );
					m_LowerSlider.SetPos( ( int ) -lLowerClip );

					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );

					// Show Clip Slider, Edit, Captions...
					ShowControls(false, m_eLutMode );

					ShowBoundaryControls(true);
					}
					break;

				case LutTransformTypeFreeform:
					// Set Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( GraphMousePointCallback, this );

					// Hide all other controls...
					ShowControls(false);
					break;

				case LutTransformTypeTwoKnee:
					{
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );

					int iBand = m_iCurrentBand != -1 ? m_iCurrentBand : 0;
					SVLutTransformParameters param;
					m_Lut(iBand).Info().GetTransformParameters(param);
					if( 4 < param.size() )
					{
						m_iX1 = param[0];
						m_iY1 = param[1];
						m_iX2 = param[2];
						m_iY2 = param[3];
					}

					ShowControls(false, m_eLutMode );
					ShowRangeControls(true);
					}
					break;
					
				default: // Unknown Mode...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );

					// Hide all...
					ShowControls(false);
			}// end switch( meLutMode )
		}
		else
		{
			// No LUT Mode...
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );

			// Hide all...
			ShowControls(false);
		}

		if( ! m_bUseLUT )
		{
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
		}

		// Set controls activation states...
		EnableControls( ( TRUE == m_bUseLUT ) );
	}
	else
	{
		// Hide all...
		ShowControls(false);
		EnableControls(false);
	}

	UpdateData( FALSE ); // set data to dialog
}

void SVLutDlgPage::ShowControls(bool bShow, SVLutTransformOperationEnum e)
{
	UINT nCmd = bShow ? SW_SHOW : SW_HIDE;
	CWnd* pWnd = GetDlgItem(IDC_FORMULA_BUTTON);

	if(nullptr != pWnd)
	{
		pWnd->ShowWindow( nCmd );
	}
	UINT nCmdRangeShow = SW_SHOW;
	if ( LutTransformTypeClip == e )
	{
		nCmdRangeShow = SW_HIDE;
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_RANGE_0_0)))
	{
		pWnd->ShowWindow( nCmdRangeShow );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_RANGE_X_MAX)))
	{
		pWnd->ShowWindow( nCmdRangeShow );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_RANGE_Y_MAX)))
	{
		pWnd->ShowWindow( nCmdRangeShow );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_STATIC_RANGE_XY_MAX)))
	{
		pWnd->ShowWindow( nCmdRangeShow );
	}

	if ( !bShow && e == LutTransformTypeClip)
	{
	}
	else
	{
		ShowBoundaryControls(bShow);
	}
	if (!bShow && e == LutTransformTypeTwoKnee)
	{
	}
	else
	{
		ShowRangeControls(bShow);
	}
}

void SVLutDlgPage::EnableControls(bool bEnable)
{
	CWnd* pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK );

	if(nullptr != pWnd)
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LUT_MODE_COMBO)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LUT_GRAPH)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_FORMULA_BUTTON)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_EDIT)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_SLIDER)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_UPPER_STATIC)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_EDIT)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_SLIDER)))
	{
		pWnd->EnableWindow( bEnable );
	}
	if (nullptr != (pWnd = GetDlgItem(IDC_LOWER_STATIC)))
	{
		pWnd->EnableWindow( bEnable );
	}
}

void SVLutDlgPage::OnActivateCheck() 
{
	UpdateData( TRUE );	// get Data from Dialog
	Refresh();
}

void SVLutDlgPage::OnSelchangeLutModeCombo() 
{
	int sel = m_LutModeCombo.GetCurSel();
	if( CB_ERR != sel )
	{
		SVLutTransformOperationEnum LutTransformOperation = static_cast<SVLutTransformOperationEnum> (m_LutModeCombo.GetItemData(sel));
		const SVLutTransformOperationMap::SVLutTransformTypeInfo* pTypeInfo = m_mapOperations.GetInfo(LutTransformOperation);
		if (nullptr != pTypeInfo)
		{
			if (-1 == m_iCurrentBand)
			{
				m_Lut.SetTransformOperation(*pTypeInfo->m_pType);
			}
			else
			{
				m_Lut(m_iCurrentBand).SetTransformOperation(*pTypeInfo->m_pType);
			}
			m_eLutMode = pTypeInfo->m_eType;
		}

		m_ptLastMousePos = m_ptNoLastMousePos;

		// set up parameters;
		SVLutTransformParameters param;
		switch (m_eLutMode)
		{
			case LutTransformTypeClip:
				if (atoi(m_LowerClipValue) != 0 || atoi(m_UpperClipValue) != 0)
				{
					param.push_back( atoi(m_LowerClipValue) );
					param.push_back( atoi(m_UpperClipValue) );
				}
				else
				{
					param.push_back( 0 );
					param.push_back( m_lMaxLutValue );
				}
				break;
			case LutTransformTypeTwoKnee:
				param.push_back( m_iX1 );
				param.push_back( m_iY1 );
				param.push_back( m_iX2 );
				param.push_back( m_iY2 );
				param.push_back( m_lMaxLutValue );
			default:
				break;
		}
		
		SetLutParameters(param);
		Refresh();
	}
}

void SVLutDlgPage::SetLutParameters(const SVLutTransformParameters& param)
{
	if (-1 == m_iCurrentBand)
	{
		m_Lut.Transform(param);
	}
	else
	{
		m_Lut(m_iCurrentBand).Transform(param);	// only modify current band
	}
	m_pCamera->SetLut(m_Lut);
}

void SVLutDlgPage::OnContinuousRecalcCheck() 
{
}

LRESULT SVLutDlgPage::OnGraphRefresh(WPARAM, LPARAM)
{
	Refresh();
	return true;
}

void SVLutDlgPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		m_ptLastMousePos = m_ptNoLastMousePos;
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONDOWN, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
	{
		CPropertyPage::OnLButtonDown(nFlags, point);
	}
}

void SVLutDlgPage::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
	{
		CPropertyPage::OnLButtonUp(nFlags, point);
	}
}

void SVLutDlgPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &m_LUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_MOUSEMOVE, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
	{
		CPropertyPage::OnMouseMove(nFlags, point);
	}
}

void SVLutDlgPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // Get data from dialog...

	CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

	if( pSlider == &m_UpperSlider )
	{
		long lUpperClip = abs(( long ) m_UpperSlider.GetPos());
		long lLowerClip = abs(( long ) m_LowerSlider.GetPos());

		if ( lLowerClip > lUpperClip )
		{
			lLowerClip = lUpperClip;
		}

		SVLutTransformParameters param;
		param.push_back( lLowerClip );
		param.push_back( lUpperClip );
		SetLutParameters(param);
		Refresh();
	}
	else if( pSlider == &m_LowerSlider )
	{
		long lUpperClip = abs(( long ) m_UpperSlider.GetPos());
		long lLowerClip = abs(( long ) m_LowerSlider.GetPos());

		if ( lLowerClip > lUpperClip )
		{
			lUpperClip = lLowerClip;
		}

		SVLutTransformParameters param;
		param.push_back( lLowerClip );
		param.push_back( lUpperClip );
		SetLutParameters(param);
		Refresh();
	}
	else
	{
		CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}

void SVLutDlgPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CALLBACK SVLutDlgPage::GraphMousePointCallback( POINT point_, LPVOID pThis_ )
{
	if( pThis_ )
	{
		// Get Lut vector...
		SVLutDlgPage* pThis = ( SVLutDlgPage* ) pThis_;
		CPoint pt(point_);

		if (pt.x >= 0 && pt.x < static_cast<long>(pThis->m_Lut.Info().BandSize()) && pt.y >=0 && pt.y <= pThis->m_lMaxLutValue)
		{
			// interpolate straight line between last point and current point...
			if (pThis->m_ptLastMousePos.x != pt.x && pThis->m_ptLastMousePos != SVLutDlgPage::m_ptNoLastMousePos)
			{
				double dSlope = -((double) (pThis->m_ptLastMousePos.y - pt.y)) / ((double) (pThis->m_ptLastMousePos.x - pt.x));
				double dY = pt.y;
				int iDeltaX = (pThis->m_ptLastMousePos.x - pt.x) > 0 ? 1 : -1;
				dSlope *= (double)(-iDeltaX);
				for (int xPos=pt.x; xPos != pThis->m_ptLastMousePos.x; xPos += iDeltaX)
				{
					// set value at xPos, (int) dY
					if (-1 == pThis->m_iCurrentBand)
					{
						for (int iBand=0; iBand < pThis->m_Lut.NumBands(); iBand++)
						{
							pThis->m_Lut(iBand)(xPos) = (int) dY;
						}
					}
					else
					{
						pThis->m_Lut(pThis->m_iCurrentBand)(xPos) = (int) dY;
					}

					dY += dSlope;
				}
			}

			pThis->m_pCamera->SetLut(pThis->m_Lut);
			pThis->m_ptLastMousePos = pt;
			pThis->Refresh();
		}// end if (pt.x >= 0 && pt.x <= pThis->mLut.Info().BandSize() && pt.y >=0 && pt.y <= mlMaxLutValue)
		else
		{
			pThis->m_ptLastMousePos = SVLutDlgPage::m_ptNoLastMousePos;
		}
	}
	return FALSE;
}

void SVLutDlgPage::OnColorBandAll() 
{
	// don't implement for now... all three bands visible at once; disable sliders, etc.	
}

void SVLutDlgPage::OnColorBandBlue() 
{
	m_iCurrentBand = 2;
	m_rgbLineColor = RGB(0,0,255);
	Refresh();
}

void SVLutDlgPage::OnColorBandGreen() 
{
	m_iCurrentBand = 1;
	m_rgbLineColor = RGB(0,192,0);
	Refresh();
}

void SVLutDlgPage::OnColorBandRed() 
{
	m_iCurrentBand = 0;
	m_rgbLineColor = RGB(234,0,0);
	Refresh();
}

void SVLutDlgPage::OnColorBandSync() 
{
	// we will have to adjust this once we implement the "All" option... All -> Sync??? disable Sync if all is selected
	SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
	INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LUT_ShouldOverwriteAllBands, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
	if (IDYES == result )
	{
		// copy current band data to all other bands
		if ( -1 != m_iCurrentBand )
		{
			for (int iBand=0; iBand < m_Lut.NumBands(); iBand++)
			{
				if (iBand == m_iCurrentBand)
					continue;
				for (unsigned int j=0; j < m_Lut.Info().BandSize(); j++)
				{
					m_Lut(iBand, j) = m_Lut(m_iCurrentBand, j);
				}
				if (m_Lut(m_iCurrentBand).GetTransformOperation())
				{
					m_Lut(iBand).SetTransformOperation(*m_Lut(m_iCurrentBand).GetTransformOperation());
				}
			}
			m_pCamera->SetLut(m_Lut);
		}

		// set sync band info
		m_iCurrentBand = -1;
		m_rgbLineColor = RGB(0,0,0);
		Refresh();
	}
	else if ( m_iCurrentBand <= 2 && m_iCurrentBand >= 0)
	{
		// reselect previous band
		m_iBand = m_iCurrentBand;
		UpdateData(FALSE);
	}
}

void SVLutDlgPage::OnChangeKnee()
{
	UpdateData();
	SVLutTransformParameters param;
	param.push_back( m_iX1 );
	param.push_back( m_iY1 );
	param.push_back( m_iX2 );
	param.push_back( m_iY2 );
	param.push_back( m_lMaxLutValue );
	SetLutParameters(param);
	Refresh();
}

void SVLutDlgPage::OnChangeX1() 
{
	OnChangeKnee();
}

void SVLutDlgPage::OnChangeX2() 
{
	OnChangeKnee();
}

void SVLutDlgPage::OnChangeY1() 
{
	OnChangeKnee();
}

void SVLutDlgPage::OnChangeY2() 
{
	OnChangeKnee();
}

