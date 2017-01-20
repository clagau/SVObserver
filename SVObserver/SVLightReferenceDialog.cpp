//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLightReferenceDialog
//* .File Name       : $Workfile:   SVLightReferenceDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:46:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLightReferenceDialog.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVAcquisitionClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

IMPLEMENT_DYNAMIC(SVLightReferenceDialogPropertySheetClass, CPropertySheet)

BEGIN_MESSAGE_MAP(SVLightReferenceDialogPropertySheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVLightReferenceDialogPropertySheetClass)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// All Pages must be dynamically created using new!
SVLightReferenceDialogPropertySheetClass::SVLightReferenceDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

// All Pages must be dynamically created using new!
SVLightReferenceDialogPropertySheetClass::SVLightReferenceDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

// All Pages must be dynamically created using new!
SVLightReferenceDialogPropertySheetClass::~SVLightReferenceDialogPropertySheetClass()
{
	// Destroy dynamically created pages...
	DestroyAllPages();
}

// Removes and Drestroys all Pages from List.
// All Pages must be dynamically created using new!
void SVLightReferenceDialogPropertySheetClass::DestroyAllPages()
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

void SVLightReferenceDialogPropertySheetClass::OnClose() 
{
	
	CPropertySheet::OnClose();
}



bool SVLightReferenceDialogPropertySheetClass::CreatePages( SVVirtualCameraPtrSet& setCameras, SVLightReferenceArray& apLRA)
{
	miNumPages=0;
	int i;
	SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin();

	for( i = 0; l_Iter != setCameras.end(); ++i, ++l_Iter )
	{
		SVVirtualCamera* pCamera = ( *l_Iter );
		//If pointer is nullptr then do next camera
		if( nullptr == pCamera ){ continue; }
		SVAcquisitionClassPtr pDevice = pCamera->GetAcquisitionDevice();
		SVLightReference* pLR = apLRA.GetAt(i);
		int iNumBands=pLR->NumBands();
		int iBandSize = 1;
		int iCamBand = 0;
		HRESULT hr = pCamera->GetBand(iCamBand);
		hr = pCamera->GetBandSize(iBandSize);
		if (S_OK == hr )
		{
			if (0 == pLR->NumBands() )
			{
				SVStringVector msgList;
				if( !( pDevice.empty() ) )
				{
					msgList.push_back(SvUl_SF::Format("%s (Dig_%d.Ch_%d)", pCamera->GetName(), pDevice->DigNumber(), pDevice->Channel()));
				}
				else
				{
					msgList.push_back(pCamera->GetName());
				}
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LightReference_NotAvailable, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10059 );
				DestroyAllPages();
				return false;
			}

			int iMaxBand = iCamBand;    // will only ever need to do one set of tabs per camera
		
			for (int iBand=iCamBand; iBand <= iMaxBand; iBand++)
			{
				SVLightReferenceBand* pLRBand = &(pLR->Band(iBand));
				for (int iAttribute=0; iAttribute < pLRBand->NumAttributes(); iAttribute++)
				{
					// the IDs are no longer valid... use the text instead
					SVString DigitizerName = SvUl_SF::Format( _T("Dig.%d"), pDevice->DigNumber() );
					SVString TabText = pCamera->GetName() + SVString(_T(" ")) + DigitizerName + SVString(_T(" ")) + pLRBand->Attribute( iAttribute ).strName;
					SVLightReferenceDialogPropertyPageClass* pPage = new SVLightReferenceDialogPropertyPageClass( TabText.c_str() );
					pPage->mpCamera = pCamera;
					pPage->mpDevice = pDevice;
					pPage->mpLR = pLR;
					pPage->miAttributeType = pLRBand->Attribute(iAttribute).dwType;
					pPage->msAttributeName = pLRBand->Attribute(iAttribute).strName.c_str();
					pPage->miCurrentBand = iBand;
					AddPage(pPage);
					miNumPages++;
				}// end for (int iAttribute=0; iAttribute < pLRBand->NumAttributes(); iAttribute++)
			}// end for (int iBand=0; iBand < nNumBands; iBand++)
		}// end if (S_OK == hr)
	}
	return true;
}

BOOL SVLightReferenceDialogPropertySheetClass::OnInitDialog()
{
	BOOL bReturn = CPropertySheet::OnInitDialog();
	
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVLightReferenceDialogPropertySheetClass 

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite SVLightReferenceDialogPropertyPageClass 

IMPLEMENT_DYNCREATE(SVLightReferenceDialogPropertyPageClass, CPropertyPage)

BEGIN_MESSAGE_MAP(SVLightReferenceDialogPropertyPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVLightReferenceDialogPropertyPageClass)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHANNEL0, OnChannel0)
	ON_BN_CLICKED(IDC_CHANNEL1, OnChannel1)
	ON_BN_CLICKED(IDC_CHANNEL2, OnChannel2)
	ON_BN_CLICKED(IDC_CHANNEL3, OnChannel3)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeValue)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVLightReferenceDialogPropertyPageClass::SVLightReferenceDialogPropertyPageClass( LPCTSTR lpszTitle )
: CPropertyPage( SVLightReferenceDialogPropertyPageClass::IDD, 0)
{
	//{{AFX_DATA_INIT(SVLightReferenceDialogPropertyPageClass)
	mlValue = 0;
	//}}AFX_DATA_INIT

	miAttributeType=-1;
	miCurrentBand = 0;
	mpAcquisition = nullptr;
	m_strCaption = lpszTitle;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

SVLightReferenceDialogPropertyPageClass::~SVLightReferenceDialogPropertyPageClass()
{
}

void SVLightReferenceDialogPropertyPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLightReferenceDialogPropertyPageClass)
	DDX_Control(pDX, IDC_SLIDER1, mValueSlider);
	DDX_Text(pDX, IDC_EDIT1, mlValue);
	//}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVLightReferenceDialogPropertyPageClass 


void SVLightReferenceDialogPropertyPageClass::OnOK() 
{
	SetCurrentValue( (DWORD) mlValue );
	
	CPropertyPage::OnOK();
}

BOOL SVLightReferenceDialogPropertyPageClass::OnSetActive() 
{

	long max, min;
	mpDevice->GetMaxLightReferenceValue( CurrentType(), max );
	mpDevice->GetMinLightReferenceValue( CurrentType(), min );

	mValueSlider.SetRange( ( int ) min, ( int ) max, TRUE );
	mValueSlider.SetTic( (max-min)/10 );
	mValueSlider.SetPageSize( (max-min)/10 );

	// Set Min and Max Text 
	SVString Text = SvUl_SF::Format( _T("%ld"), min );
	GetDlgItem(IDC_MIN)->SetWindowText( Text.c_str() );
	Text = SvUl_SF::Format( _T("%ld"), max );
	GetDlgItem(IDC_MAX)->SetWindowText( Text.c_str() );

	// Update step width...
	unsigned long step;
	mpDevice->GetLightReferenceValueStep( CurrentType(), step );

	mlValue = CurrentValue();
	mValueSlider.SetPos( ( int ) mlValue );

	
	UpdateData( FALSE ); // set data to dialog
	
	UpdateLightReference();

	return CPropertyPage::OnSetActive();
}

BOOL SVLightReferenceDialogPropertyPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( nullptr == mpDevice || nullptr == mpLR || -1 == miAttributeType || nullptr == mpCamera )
	{
		// Not supported use!!! Caller forgot to set parameters
		ASSERT( FALSE );
		if( GetParent() )
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );
		return FALSE;
	}

	miNumBands=mpLR->NumBands();
	HRESULT hr = mpCamera->GetBand(miCamBand);
	hr = mpCamera->GetBandSize(miBandSize);

	UpdateData( TRUE );	// set data to dialog...

	mValueSlider.SetRange( 0, 255, TRUE );
	mValueSlider.SetTic( 127 );
	mValueSlider.SetPageSize( 1 );

	CWnd* pWnd;

	std::vector<UINT> aChannels;
	aChannels.push_back(IDC_CHANNEL0);
	aChannels.push_back(IDC_CHANNEL1);
	aChannels.push_back(IDC_CHANNEL2);

	for ( int i = 0; i < static_cast<int>(aChannels.size()); i++)
	{
		if( pWnd = GetDlgItem( aChannels[i] ) )
		{
			BOOL bEnable =  miNumBands > 1 && (miBandSize > 1 || miCamBand == i);  // if there is only one band, don't enable any of the radio buttons
			if ( i < mpLR->NumBands() )
			{
				if ( nullptr == mpLR->Band(i).AttributeByName(SVString(CurrentName())) )
				{
					bEnable = false;
				}
			}
			pWnd->EnableWindow(bEnable);
		}
	}

	if( pWnd = GetDlgItem( IDC_CHANNEL3 ) )
	{
		BOOL bEnable = miNumBands > 1 && (miBandSize > 3 || miCamBand == 3);
		if ( nullptr == mpLR->Band(miCamBand).AttributeByName(SVString(CurrentName())) )
		{
			bEnable = false;
		}
		pWnd->EnableWindow(bEnable);
	}

	// NOTE: 
	//		Only one image source at this time...!!
	// &&&


	UpdateData( FALSE );	// set data to dialog...

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVLightReferenceDialogPropertyPageClass::UpdateLightReference()
{
	mpCamera->SetLightReference(*mpLR);
}



void SVLightReferenceDialogPropertyPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // get data of dialog	
	
	if( &mValueSlider == ( CSliderCtrl* ) pScrollBar )
	{
		mlValue = ( DWORD ) mValueSlider.GetPos();
		SetCurrentValue( (DWORD) mlValue );
	}

	UpdateData( FALSE ); // set data to dialog

	UpdateLightReference();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVLightReferenceDialogPropertyPageClass::OnChannel0() 
{
	SetCurrentValue( ( DWORD ) mlValue );
	miCurrentBand = 0;

	mlValue = CurrentValue();
	mValueSlider.SetPos( ( int ) mlValue );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL0 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel1() 
{
	SetCurrentValue( ( DWORD ) mlValue );
	miCurrentBand = 1;

	mlValue = CurrentValue();
	mValueSlider.SetPos( ( int ) mlValue );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL1 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel2() 
{
	SetCurrentValue( ( DWORD ) mlValue );
	miCurrentBand = 2;

	mlValue = CurrentValue();
	mValueSlider.SetPos( ( int ) mlValue );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL2 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel3() 
{
	SetCurrentValue( ( DWORD ) mlValue );
	miCurrentBand = 3;

	mlValue = CurrentValue();
	mValueSlider.SetPos( ( int ) mlValue );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL3 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChangeValue() 
{
	UpdateData( TRUE ); // get data of dialog	
	
	mValueSlider.SetPos( ( int ) mlValue );		

	UpdateData( FALSE ); // set data to dialog

	UpdateLightReference();	
}


void SVLightReferenceDialogPropertyPageClass::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
}

void SVLightReferenceDialogPropertyPageClass::OnReset() 
{
	CPropertyPage::OnReset();
}

BOOL SVLightReferenceDialogPropertyPageClass::OnKillActive() 
{
	UpdateData();
	SetCurrentValue( ( DWORD ) mlValue );
	
	return CPropertyPage::OnKillActive();
}

DWORD SVLightReferenceDialogPropertyPageClass::CurrentValue()
{
	SVLightReferenceAttributeInfo* pAttribute = mpLR->Band( miCurrentBand ).AttributeByType( CurrentType() );
	if ( pAttribute )
		return pAttribute->lValue;
	else
	{
		pAttribute = mpLR->Band( miCurrentBand ).AttributeByName( SVString(CurrentName()) ); 
		if ( pAttribute )
			return pAttribute->lValue;
		else
		{
			ASSERT( FALSE );
			return 0;
		}
	}
}

void SVLightReferenceDialogPropertyPageClass::SetCurrentValue(DWORD dw)
{
	SVLightReferenceAttributeInfo* pAttribute = mpLR->Band( miCurrentBand ).AttributeByType( CurrentType() );
	if ( pAttribute )
		pAttribute->lValue = (long) dw;
	else
	{
		pAttribute = mpLR->Band( miCurrentBand ).AttributeByName( SVString(CurrentName()) );
		if ( pAttribute )
			pAttribute->lValue = (long) dw;
		else
		{
			ASSERT( FALSE );
		}
	}
}
