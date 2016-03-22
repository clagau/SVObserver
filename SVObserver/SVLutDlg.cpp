//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVAcquisitionClass.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVObserver.h"	// TEMP HACK for 1394 detection
#include "SVConfigurationObject.h"	// TEMP HACK for 1394 detection
#include "SVStatusLibrary/MessageManagerResource.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#pragma endregion Includes


// Undefine min/max, so can use std::min/max
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVLutTransformOperationMap SVLutDlgPage::m_mapOperations;
const CPoint SVLutDlgPage::ptNoLastMousePos(-1,-1);

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

bool SVLutDlg::Create( SVVirtualCameraPtrSet& setCameras, SVLutMap& raLut )
{
	SVVirtualCameraPtrSet::iterator l_Iter;

	for( l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++l_Iter )
	{
		SVVirtualCamera* pCamera = ( *l_Iter );

		if( nullptr != pCamera )
		{
			SVAcquisitionClassPtr pAcqDevice = pCamera->GetAcquisitionDevice();

			if( !( pAcqDevice.empty() ) && raLut.find( pCamera->GetUniqueObjectID() ) != raLut.end() && 0 < raLut[pCamera->GetUniqueObjectID()].NumBands() )
			{
				CString sCaption;

				sCaption.Format("%s (Dig_%d.Ch_%d)", pCamera->GetName(), pAcqDevice->DigNumber(), pAcqDevice->Channel());

				SVLutDlgPage* pPage;
				pPage = new SVLutDlgPage(this, sCaption);
				pPage->mLut = raLut[ pCamera->GetUniqueObjectID() ];
				pPage->mpCamera = pCamera;
				AddPage(pPage);
			}
		}
	}

	if( GetPageCount() < 1 )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvO::LUT_NotAvailable, StdMessageParams, SvOi::Err_10060 );

		DestroyAllPages();

		return false;
	}

	return true;
}

/*
void SVLutDlg::CreatePages()
{
}
*/

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



SVLutDlgPage::SVLutDlgPage( SVLutDlg* pParent, const CString& sCaption )
	: CPropertyPage(SVLutDlgPage::IDD)
{
	//{{AFX_DATA_INIT(SVLutDlgPage)
	mstrUpperClipValue = _T("");
	mstrLowerClipValue = _T("");
	mbUseLUT = TRUE;
	mbContinuousRecalcLUT = FALSE;
	m_iBand = -1;
	m_iX1 = 0;
	m_iX2 = 0;
	m_iY1 = 0;
	m_iY2 = 0;
	m_sRangeXMax = _T("");
	m_sRangeYMax = _T("");
	m_sRangeXYMax = _T("");
	//}}AFX_DATA_INIT

	mpParentDialog	= pParent;

    m_strCaption = sCaption;
    m_psp.pszTitle = m_strCaption;
    m_psp.dwFlags |= PSP_USETITLE;

}

void SVLutDlgPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLutDlgPage)
	DDX_Control(pDX, IDC_UPPER_SLIDER, mUpperSlider);
	DDX_Control(pDX, IDC_LOWER_SLIDER, mLowerSlider);
	DDX_Control(pDX, IDC_LUT_GRAPH, mLUTGraph);
	DDX_Control(pDX, IDC_LUT_MODE_COMBO, mLutModeCombo);
	DDX_Text(pDX, IDC_UPPER_EDIT, mstrUpperClipValue);
	DDX_Text(pDX, IDC_LOWER_EDIT, mstrLowerClipValue);
	DDX_Check(pDX, IDC_ACTIVATE_CHECK, mbUseLUT);
	DDX_Radio(pDX, IDC_COLOR_BAND_RED, m_iBand);
	DDX_Text(pDX, IDC_X1, m_iX1);
	DDX_Text(pDX, IDC_X2, m_iX2);
	DDX_Text(pDX, IDC_Y1, m_iY1);
	DDX_Text(pDX, IDC_Y2, m_iY2);
	DDX_Text(pDX, IDC_STATIC_RANGE_X_MAX, m_sRangeXMax);
	DDX_Text(pDX, IDC_STATIC_RANGE_Y_MAX, m_sRangeYMax);
	DDX_Text(pDX, IDC_STATIC_RANGE_XY_MAX, m_sRangeXYMax);
	//}}AFX_DATA_MAP
}


BOOL SVLutDlgPage::OnInitDialog() 
{
	mpCamera->GetLut(mLut);

	mpCamera->GetBand(miCamBand);
	mpCamera->GetBandSize(miBandWidth);
	miNumBands=mLut.NumBands();

	miCurrentBand = -1;
	mrgbLineColor = RGB(0,0,0);

	mptLastMousePos = ptNoLastMousePos;

	UINT uiMaxValue = mLut.Info().MaxValue();
	if ( uiMaxValue == 0 )	// legacy support
	{
		if (miNumBands == 3)
		{
			mlMaxLutValue = 1023;	// color
		}
		else
		{
			mlMaxLutValue = 255;	// mono
		}
	}
	else
	{
		mlMaxLutValue = uiMaxValue;
	}

	m_sRangeYMax.Format("(0, %d)", mlMaxLutValue);
	m_sRangeXMax.Format("(%d, 0)", mLut.Info().BandSize()-1);
	m_sRangeXYMax.Format("(%d, %d)", mLut.Info().BandSize()-1, mlMaxLutValue);

	CPropertyPage::OnInitDialog();

	bool bAllVariablesSet = mpParentDialog && mpCamera;

	ASSERT ( bAllVariablesSet );

	if( bAllVariablesSet )
	{
		CWnd* pWnd=NULL;

		// Set Normalize Mode of Graph Control...
		mLUTGraph.SetNormalizeMode( SvOml::SVNormalizeXYMinMax );
		mLUTGraph.SetXYMinMax(0, 0, mLut.Info().BandSize()-1, mlMaxLutValue);


		if ( miBandWidth == 3 )
		{
			// show color band controls if color LUT
//			if (pWnd = GetDlgItem(IDC_COLOR_BAND_ALL))
//				pWnd->ShowWindow( SW_SHOW );
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_SYNC))
			{
				((CButton*)pWnd)->SetCheck(TRUE);
			}

			bool bSame = true;
			for (unsigned int j=0; bSame && j < mLut.Info().BandSize(); j++)
			{
				long lVal = mLut(0,j);
				for (unsigned int iBand=1; iBand < mLut.NumBands(); iBand++)
				{
					bSame = bSame && (mLut(iBand,j) == lVal);
				}
			}
			if (!bSame)
			{
				miCurrentBand = 0;	// default to red
				mrgbLineColor = RGB(234,0,0);
			}

		}// end if ( miBandWidth == 3 )
		else
		{
			 mpCamera->GetBand(miCurrentBand);

			// hide color band controls if not color LUT
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_ALL))
				pWnd->ShowWindow( SW_HIDE );
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_BLUE))
				pWnd->ShowWindow( SW_HIDE );
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_GREEN))
				pWnd->ShowWindow( SW_HIDE );
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_RED))
				pWnd->ShowWindow( SW_HIDE );
			if (pWnd = GetDlgItem(IDC_COLOR_BAND_SYNC))
				pWnd->ShowWindow( SW_HIDE );

		}

		// range is negative because the stupid slider implementation has min value hardcoded at the top.
		// we will have to manually take the abs of the slider pos.

		mUpperSlider.SetRange( -mlMaxLutValue, 0 );
		mLowerSlider.SetRange( -mlMaxLutValue, 0 );
		//Slider does not update correctly on an RGB, the position needs to be set to
		//something  and then set to what it needs to be set at
		mLowerSlider.SetPos(-30);
		mLowerSlider.SetPos(-1);// default pos; should be 0 but slider doesn't initially update properly if it is 0.
		mUpperSlider.SetPos(-mlMaxLutValue);	// default pos


		// fill Mode combo box
		for (int i = 0; i  < m_mapOperations.GetTypes().GetSize(); i++)
		{
			const SVLutTransformOperationMap::SVLutTransformTypeInfo& rType = m_mapOperations.GetTypes()[i];
			if (rType.pType)
			{
				int iPos = mLutModeCombo.AddString( rType.sType );
				mLutModeCombo.SetItemData( iPos, i );
				if (mLut.Info().GetTransformOperation())
				{
					if (typeid(*rType.pType) == typeid(*mLut.Info().GetTransformOperation()))
					{
						mLutModeCombo.SetCurSel(iPos);
						meLutMode = rType.eType;
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
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben

}

void SVLutDlgPage::Refresh()
{
	CWnd* pWnd = NULL;
	if( true )
	{
		// Recalc LUT...
		//pLUTOperator->RecalcLUT();
		m_iBand = miCurrentBand != -1 ? miCurrentBand : 4;

		// Refresh LUT Graph...
		mpCamera->GetLut(mLut);
		int iBandSize = mLut.Info().BandSize();
		malBandData.SetSize(iBandSize);

		int iSourceBand = miCurrentBand;
		if (miCurrentBand == -1)
		{
			iSourceBand = 0;
		}
		for( int j = 0; j < iBandSize; j++ )
		{
			long lVal=0;
			lVal = mLut(iSourceBand, j);
			malBandData.SetAt( j, lVal );
		}// end for

		mLUTGraph.SetColor(mrgbLineColor, FALSE);
		mLUTGraph.SetPoints( malBandData );

		if( true /*pLUTMode*/ )
		{
			// refresh lut mode combo settings...
			CString strMode;
			const SVLutTransformOperation* pOperation=NULL;
			if (miCurrentBand == -1)
			{
				pOperation = mLut.GetTransformOperation();
			}
			else
			{
				pOperation = mLut(miCurrentBand).GetTransformOperation();
			}
			ASSERT( pOperation != NULL);
			if (pOperation != NULL)
			{
				const SVLutTransformOperationMap::SVLutTransformTypeInfo* pInfo = m_mapOperations.GetInfo(pOperation);
				if (pInfo)
				{
					strMode = pInfo->sType;
					meLutMode = pInfo->eType;
				}
			}

			// Set cur sel in combo box...
			mLutModeCombo.SelectString( -1, strMode );

			// Show or hide Controls depending on LUT Mode...
			switch( meLutMode )
			{
				case LutTransformTypeNormal:	// Identity...
				case LutTransformTypeInversion:
				case LutTransformTypeSign:
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Show no special controls...
					ShowControls(false);

					break;
				
				case LutTransformTypeClip:
					{
					// Refresh upper clip...
					// get pos from lut
					int iBand = miCurrentBand != -1 ? miCurrentBand : 0;
					int iMin=9999999, iMax=0;

					SVLutTransformParameters param;
					mLut(iBand).Info().GetTransformParameters(param);
					if (param.GetSize() == 2)
					{
						iMin = param.GetAt(0);
						iMax = param.GetAt(1);
					}
					else
					{
						for (unsigned int i=0; i < mLut.Info().BandSize(); i++)
						{
							iMin = std::min(iMin, static_cast<int>(mLut(iBand, i)));
							iMax = std::max(iMax, static_cast<int>(mLut(iBand, i)));
						}
					}
					//*/
//					long lUpperClip = abs((long)mUpperSlider.GetPos());
					long lUpperClip = iMax;
					mstrUpperClipValue.Format( _T( "%d" ), lUpperClip );
					mUpperSlider.SetPos( ( int ) -lUpperClip );

					// Refresh lower clip...
//					long lLowerClip = abs((long)mLowerSlider.GetPos());
					long lLowerClip = iMin;
					mstrLowerClipValue.Format( _T( "%d" ), lLowerClip );
					mLowerSlider.SetPos( ( int ) -lLowerClip );

					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Show Clip Slider, Edit, Captions...
					ShowControls(false, meLutMode );
					if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
						pWnd->ShowWindow( SW_SHOW );
					}
					break;

				/*
				case 4: // Formula...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Hide Clip Mode controls and others...
					ShowControls(false);
					// Show Formula Button...
					if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
						pWnd->ShowWindow( SW_SHOW );
					break;
				*/

				case LutTransformTypeFreeform:
					// Set Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( GraphMousePointCallback, this );

					// Hide all other controls...
					ShowControls(false);

					break;

				case LutTransformTypeTwoKnee:
					{
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( NULL, NULL );


					int iBand = miCurrentBand != -1 ? miCurrentBand : 0;
					SVLutTransformParameters param;
					mLut(iBand).Info().GetTransformParameters(param);
					if (param.GetSize() > 4)
					{
						m_iX1 = param.GetAt(0);
						m_iY1 = param.GetAt(1);
						m_iX2 = param.GetAt(2);
						m_iY2 = param.GetAt(3);
					}

					ShowControls(false, meLutMode );
					if( pWnd = GetDlgItem( IDC_STATIC_X1 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_STATIC_Y1 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_STATIC_X2 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_STATIC_Y2 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_X1 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_Y1 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_X2 ) )
						pWnd->ShowWindow( SW_SHOW );
					if( pWnd = GetDlgItem( IDC_Y2 ) )
						pWnd->ShowWindow( SW_SHOW );
					}
					break;
					
				default: // Unknown Mode...
					// Deactivate Mouse Proc Func of SVDlgGraph Control...
					mLUTGraph.SetMousePointProcFunc( NULL, NULL );

					// Hide all...
					ShowControls(false);
			}// end switch( meLutMode )
		}
		else
		{
			// No LUT Mode...
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			mLUTGraph.SetMousePointProcFunc( NULL, NULL );

			// Hide all...
			ShowControls(false);
		}


		/*
		if( pContinuousRecalcLUT )
		{
			// refresh LUT settings...
			pContinuousRecalcLUT->GetValue( mbContinuousRecalcLUT );
		}
		else
			bUseLUT = FALSE;
		*/

		if( ! mbUseLUT )
		{
			// Deactivate Mouse Proc Func of SVDlgGraph Control...
			mLUTGraph.SetMousePointProcFunc( NULL, NULL );
		}

		// Set controls activation states...
		EnableControls( ( mbUseLUT == TRUE ) );

		UpdateData( FALSE ); // set data to dialog

		return;

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
	CWnd* pWnd;

	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->ShowWindow( nCmd );

	UINT nCmdRangeShow = SW_SHOW;
	if ( e == LutTransformTypeClip)
	{
		nCmdRangeShow = SW_HIDE;
	}
	if ( pWnd = GetDlgItem( IDC_STATIC_RANGE_0_0 ) )
		pWnd->ShowWindow( nCmdRangeShow );
	if ( pWnd = GetDlgItem( IDC_STATIC_RANGE_X_MAX ) )
		pWnd->ShowWindow( nCmdRangeShow );
	if ( pWnd = GetDlgItem( IDC_STATIC_RANGE_Y_MAX ) )
		pWnd->ShowWindow( nCmdRangeShow );
	if ( pWnd = GetDlgItem( IDC_STATIC_RANGE_XY_MAX ) )
		pWnd->ShowWindow( nCmdRangeShow );



	if ( !bShow && e == LutTransformTypeClip)
	{
	}
	else
	{
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->ShowWindow( nCmd );
	}
	if (!bShow && e == LutTransformTypeTwoKnee)
	{
	}
	else
	{
		if( pWnd = GetDlgItem( IDC_STATIC_X1 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_STATIC_Y1 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_STATIC_X2 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_STATIC_Y2 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_X1 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_Y1 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_X2 ) )
			pWnd->ShowWindow( nCmd );
		if( pWnd = GetDlgItem( IDC_Y2 ) )
			pWnd->ShowWindow( nCmd );
	}
}

void SVLutDlgPage::EnableControls(bool bEnable)
{
	CWnd* pWnd;

	if( pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_LUT_MODE_COMBO ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_LUT_GRAPH ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
		pWnd->EnableWindow( bEnable );
	if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
		pWnd->EnableWindow( bEnable );
}



void SVLutDlgPage::OnActivateCheck() 
{
	UpdateData( TRUE );	// get Data from Dialog
	Refresh();
}

void SVLutDlgPage::OnSelchangeLutModeCombo() 
{
	int sel = mLutModeCombo.GetCurSel();
	if( sel != CB_ERR )
	{
		long lValue = ( long ) mLutModeCombo.GetItemData( sel );
		const SVLutTransformOperationMap::SVLutTransformTypeInfo& rType = m_mapOperations.GetTypes()[lValue];
		if (miCurrentBand == -1)
		{
			mLut.SetTransformOperation(*rType.pType);
		}
		else
		{
			mLut(miCurrentBand).SetTransformOperation(*rType.pType);
		}
		meLutMode = rType.eType;

		mptLastMousePos = ptNoLastMousePos;

		// set up parameters;
		SVLutTransformParameters param;
		switch (meLutMode)
		{
			case LutTransformTypeClip:
				if (atoi(mstrLowerClipValue) != 0 || atoi(mstrUpperClipValue) != 0)
				{
					param.Add(atoi(mstrLowerClipValue));
					param.Add(atoi(mstrUpperClipValue));
				}
				else
				{
					param.Add(0);
					param.Add(mlMaxLutValue);
				}
				break;
			case LutTransformTypeTwoKnee:
				param.Add(m_iX1);
				param.Add(m_iY1);
				param.Add(m_iX2);
				param.Add(m_iY2);
				param.Add(mlMaxLutValue);
			default:
				break;
		}
		
		SetLutParameters(param);
		Refresh();
	}
}

void SVLutDlgPage::SetLutParameters(const SVLutTransformParameters& param)
{
	if (miCurrentBand == -1)
	{
		mLut.Transform(param);
	}
	else
	{
		mLut(miCurrentBand).Transform(param);	// only modify current band
	}
	mpCamera->SetLut(mLut);
}

void SVLutDlgPage::OnContinuousRecalcCheck() 
{
}

LRESULT SVLutDlgPage::OnGraphRefresh( WPARAM wparam, LPARAM lparam )
{
	Refresh();
	return true;
}

void SVLutDlgPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &mLUTGraph )
	{
		mptLastMousePos = ptNoLastMousePos;
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONDOWN, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnLButtonDown(nFlags, point);
}

void SVLutDlgPage::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &mLUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnLButtonUp(nFlags, point);
}

void SVLutDlgPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = ChildWindowFromPoint( point );
	if( pWnd == &mLUTGraph )
	{
		ClientToScreen( &point );
		pWnd->ScreenToClient( &point );
		pWnd->SendMessage( WM_MOUSEMOVE, nFlags, MAKELPARAM( point.x, point.y ) );
	}
	else
		CPropertyPage::OnMouseMove(nFlags, point);
}

void SVLutDlgPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // Get data from dialog...

	CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

	if( pSlider == &mUpperSlider )
	{
		long lUpperClip = abs(( long ) mUpperSlider.GetPos());
		long lLowerClip = abs(( long ) mLowerSlider.GetPos());

		if ( lLowerClip > lUpperClip )
		{
			lLowerClip = lUpperClip;
		}

		SVLutTransformParameters param;
		param.Add(lLowerClip);
		param.Add(lUpperClip);
		SetLutParameters(param);
		Refresh();

		return;
	}
	else if( pSlider == &mLowerSlider )
	{
		long lUpperClip = abs(( long ) mUpperSlider.GetPos());
		long lLowerClip = abs(( long ) mLowerSlider.GetPos());

		if ( lLowerClip > lUpperClip )
		{
			lUpperClip = lLowerClip;
		}

		SVLutTransformParameters param;
		param.Add(lLowerClip);
		param.Add(lUpperClip);
		SetLutParameters(param);
		Refresh();

		return;
	}
	
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SVLutDlgPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	OnVScroll(nSBCode, nPos, pScrollBar);
	
	//CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CALLBACK SVLutDlgPage::GraphMousePointCallback( POINT point_, LPVOID pThis_ )
{
	if( pThis_ )
	{
		// Get Lut vector...
		SVLutDlgPage* pThis = ( SVLutDlgPage* ) pThis_;
		CPoint pt(point_);

		if (pt.x >= 0 && pt.x < static_cast<long>(pThis->mLut.Info().BandSize()) && pt.y >=0 && pt.y <= pThis->mlMaxLutValue)
		{

			// interpolate straight line between last point and current point...
			if (pThis->mptLastMousePos.x != pt.x && pThis->mptLastMousePos != SVLutDlgPage::ptNoLastMousePos)
			{
				double dSlope = -((double) (pThis->mptLastMousePos.y - pt.y)) / ((double) (pThis->mptLastMousePos.x - pt.x));
				double dY = pt.y;
				int iDeltaX = (pThis->mptLastMousePos.x - pt.x) > 0 ? 1 : -1;
				dSlope *= (double)(-iDeltaX);
				for (int xPos=pt.x; xPos != pThis->mptLastMousePos.x; xPos += iDeltaX)
				{
					// set value at xPos, (int) dY
					if (pThis->miCurrentBand == -1)
					{
						for (unsigned int iBand=0; iBand < pThis->mLut.NumBands(); iBand++)
							pThis->mLut(iBand)(xPos) = (int) dY;
					}
					else
					{
						pThis->mLut(pThis->miCurrentBand)(xPos) = (int) dY;
					}

					dY += dSlope;
				}
			}

			pThis->mpCamera->SetLut(pThis->mLut);
			pThis->mptLastMousePos = pt;
			pThis->Refresh();
		}// end if (pt.x >= 0 && pt.x <= pThis->mLut.Info().BandSize() && pt.y >=0 && pt.y <= mlMaxLutValue)
		else
		{
			pThis->mptLastMousePos = SVLutDlgPage::ptNoLastMousePos;
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
	miCurrentBand = 2;
	mrgbLineColor = RGB(0,0,255);
	Refresh();
}

void SVLutDlgPage::OnColorBandGreen() 
{
	miCurrentBand = 1;
	mrgbLineColor = RGB(0,192,0);
	Refresh();
}

void SVLutDlgPage::OnColorBandRed() 
{
	miCurrentBand = 0;
	mrgbLineColor = RGB(234,0,0);
	Refresh();
}

void SVLutDlgPage::OnColorBandSync() 
{
	// we will have to adjust this once we implement the "All" option... All -> Sync??? disable Sync if all is selected
	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvO::LUT_ShouldOverwriteAllBands, StdMessageParams, SvOi::Err_10235, NULL, nullptr, MB_YESNO );
	if (IDYES == result )
	{
		// copy current band data to all other bands
		if ( miCurrentBand != -1 )
		{
			for (unsigned int iBand=0; iBand < mLut.NumBands(); iBand++)
			{
				if (iBand == miCurrentBand)
					continue;
				for (unsigned int j=0; j < mLut.Info().BandSize(); j++)
				{
					mLut(iBand, j) = mLut(miCurrentBand, j);
				}
				if (mLut(miCurrentBand).GetTransformOperation())
					mLut(iBand).SetTransformOperation(*mLut(miCurrentBand).GetTransformOperation());
			}
			mpCamera->SetLut(mLut);
		}

		// set sync band info
		miCurrentBand = -1;
		mrgbLineColor = RGB(0,0,0);
		Refresh();
	}
	else if ( miCurrentBand <= 2 && miCurrentBand >= 0)
	{
		// reselect previous band
		m_iBand = miCurrentBand;
		UpdateData(FALSE);
	}
}

void SVLutDlgPage::OnChangeKnee()
{
	UpdateData();
	SVLutTransformParameters param;
	param.Add(m_iX1);
	param.Add(m_iY1);
	param.Add(m_iX2);
	param.Add(m_iY2);
	param.Add(mlMaxLutValue);
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

