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
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
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

// Removes and Destroys all Pages from List.
// All Pages must be dynamically created using new!
void SVLightReferenceDialogPropertySheetClass::DestroyAllPages()
{
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		CPropertyPage* pPage = GetPage( i );
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



bool SVLightReferenceDialogPropertySheetClass::CreatePages(SvIe::SVVirtualCameraPtrVector& rCameraVector, SVLightReferenceVector& rLRA)
{
	for(size_t i = 0; i < rCameraVector.size(); ++i)
	{
		SvIe::SVVirtualCamera* pCamera = rCameraVector[i];
		//If pointer is nullptr then do next camera
		if( nullptr == pCamera ){ continue; }
		SvIe::SVAcquisitionClassPtr pDevice = pCamera->GetAcquisitionDevice();
		SVLightReference& rLightRef = rLRA[i];
		int iBandSize = 1;
		int iCamBand = 0;
		/*HRESULT hr = */pCamera->GetBand(iCamBand);
		HRESULT hr = pCamera->GetBandSize(iBandSize);
		if (S_OK == hr )
		{
			if (0 == rLightRef.NumBands())
			{
				SvDef::StringVector msgList;
				if(nullptr != pDevice)
				{
					msgList.push_back(SvUl::Format("%s (Dig_%d.Ch_%d)", pCamera->GetName(), pDevice->DigNumber(), pDevice->Channel()));
				}
				else
				{
					msgList.push_back(pCamera->GetName());
				}
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LightReference_NotAvailable, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10059 );
				DestroyAllPages();
				return false;
			}

			int iMaxBand = iCamBand;    // will only ever need to do one set of tabs per camera
		
			for (int iBand=iCamBand; iBand <= iMaxBand; iBand++)
			{
				const SVLightReferenceBand& rLRBand = rLightRef.Band(iBand);
				for (int iAttribute=0; iAttribute < rLRBand.NumAttributes(); iAttribute++)
				{
					// the IDs are no longer valid... use the text instead
					std::string DigitizerName = SvUl::Format( _T("Dig.%d"), pDevice->DigNumber() );
					std::string TabText = pCamera->GetName() + std::string(_T(" ")) + DigitizerName + std::string(_T(" ")) + rLRBand.Attribute( iAttribute ).strName;
					SVLightReferenceDialogPropertyPageClass* pPage = new SVLightReferenceDialogPropertyPageClass( TabText.c_str() );
					pPage->m_pCamera = pCamera;
					pPage->m_pDevice = pDevice;
					pPage->m_pLR = &rLightRef;
					pPage->m_AttributeType = rLRBand.Attribute(iAttribute).dwType;
					pPage->m_AttributeName = rLRBand.Attribute(iAttribute).strName.c_str();
					pPage->miCurrentBand = iBand;
					AddPage(pPage);
				}
			}
		}
	}
	return true;
}

BOOL SVLightReferenceDialogPropertySheetClass::OnInitDialog()
{
	BOOL bReturn = CPropertySheet::OnInitDialog();
	
	return bReturn;
}

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
	m_Value = 0;
	//}}AFX_DATA_INIT

	m_AttributeType=-1;
	miCurrentBand = 0;
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
	DDX_Control(pDX, IDC_SLIDER1, m_ValueSlider);
	DDX_Text(pDX, IDC_EDIT1, m_Value);
	//}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVLightReferenceDialogPropertyPageClass 


void SVLightReferenceDialogPropertyPageClass::OnOK() 
{
	SetCurrentValue( (DWORD) m_Value );
	
	CPropertyPage::OnOK();
}

BOOL SVLightReferenceDialogPropertyPageClass::OnSetActive() 
{

	long max, min;
	m_pDevice->GetMaxLightReferenceValue( CurrentType(), max );
	m_pDevice->GetMinLightReferenceValue( CurrentType(), min );

	m_ValueSlider.SetRange( ( int ) min, ( int ) max, TRUE );
	m_ValueSlider.SetTic( (max-min)/10 );
	m_ValueSlider.SetPageSize( (max-min)/10 );

	// Set Min and Max Text 
	std::string Text = SvUl::Format( _T("%ld"), min );
	GetDlgItem(IDC_MIN)->SetWindowText( Text.c_str() );
	Text = SvUl::Format( _T("%ld"), max );
	GetDlgItem(IDC_MAX)->SetWindowText( Text.c_str() );

	// Update step width...
	unsigned long step;
	m_pDevice->GetLightReferenceValueStep( CurrentType(), step );

	m_Value = CurrentValue();
	m_ValueSlider.SetPos( ( int ) m_Value );

	
	UpdateData( FALSE ); // set data to dialog
	
	UpdateLightReference();

	return CPropertyPage::OnSetActive();
}

BOOL SVLightReferenceDialogPropertyPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( nullptr == m_pDevice || nullptr == m_pLR || -1 == m_AttributeType || nullptr == m_pCamera )
	{
		// Not supported use!!! Caller forgot to set parameters
		ASSERT( FALSE );
		if( GetParent() )
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );
		return FALSE;
	}

	m_NumBands=m_pLR->NumBands();
	/*HRESULT hr = */m_pCamera->GetBand(m_CamBand);
	/*HRESULT hr = */m_pCamera->GetBandSize(m_BandSize);

	UpdateData( TRUE );	// set data to dialog...

	m_ValueSlider.SetRange( 0, 255, TRUE );
	m_ValueSlider.SetTic( 127 );
	m_ValueSlider.SetPageSize( 1 );

	CWnd* pWnd{nullptr};

	std::vector<UINT> aChannels;
	aChannels.push_back(IDC_CHANNEL0);
	aChannels.push_back(IDC_CHANNEL1);
	aChannels.push_back(IDC_CHANNEL2);

	for(int i = 0; i < static_cast<int>(aChannels.size()); i++)
	{
		pWnd = GetDlgItem(aChannels[i]);
		if(nullptr != pWnd)
		{
			BOOL bEnable =  m_NumBands > 1 && (m_BandSize > 1 || m_CamBand == i);  // if there is only one band, don't enable any of the radio buttons
			if ( i < m_pLR->NumBands() )
			{
				if ( nullptr == m_pLR->Band(i).AttributeByName(std::string(CurrentName())) )
				{
					bEnable = false;
				}
			}
			pWnd->EnableWindow(bEnable);
		}
	}
	pWnd = GetDlgItem(IDC_CHANNEL3);
	if(nullptr != pWnd)
	{
		BOOL bEnable = m_NumBands > 1 && (m_BandSize > 3 || m_CamBand == 3);
		if ( nullptr == m_pLR->Band(m_CamBand).AttributeByName(std::string(CurrentName())) )
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
	m_pCamera->SetLightReference(*m_pLR);
}



void SVLightReferenceDialogPropertyPageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE ); // get data of dialog	
	
	if( &m_ValueSlider == ( CSliderCtrl* ) pScrollBar )
	{
		m_Value = ( DWORD ) m_ValueSlider.GetPos();
		SetCurrentValue( (DWORD) m_Value );
	}

	UpdateData( FALSE ); // set data to dialog

	UpdateLightReference();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVLightReferenceDialogPropertyPageClass::OnChannel0() 
{
	SetCurrentValue( ( DWORD ) m_Value );
	miCurrentBand = 0;

	m_Value = CurrentValue();
	m_ValueSlider.SetPos( ( int ) m_Value );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL0 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel1() 
{
	SetCurrentValue( ( DWORD ) m_Value );
	miCurrentBand = 1;

	m_Value = CurrentValue();
	m_ValueSlider.SetPos( ( int ) m_Value );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL1 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel2() 
{
	SetCurrentValue( ( DWORD ) m_Value );
	miCurrentBand = 2;

	m_Value = CurrentValue();
	m_ValueSlider.SetPos( ( int ) m_Value );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL2 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChannel3() 
{
	SetCurrentValue( ( DWORD ) m_Value );
	miCurrentBand = 3;

	m_Value = CurrentValue();
	m_ValueSlider.SetPos( ( int ) m_Value );

	CheckRadioButton( IDC_CHANNEL0, IDC_CHANNEL3, IDC_CHANNEL3 );
	UpdateData( FALSE ); // set data to dialog
}

void SVLightReferenceDialogPropertyPageClass::OnChangeValue() 
{
	UpdateData( TRUE ); // get data of dialog	
	
	m_ValueSlider.SetPos( ( int ) m_Value );		

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
	SetCurrentValue( ( DWORD ) m_Value );
	
	return CPropertyPage::OnKillActive();
}

DWORD SVLightReferenceDialogPropertyPageClass::CurrentValue()
{
	SVLightReferenceAttributeInfo* pAttribute = m_pLR->Band( miCurrentBand ).AttributeByType( CurrentType() );
	if ( pAttribute )
		return pAttribute->lValue;
	else
	{
		pAttribute = m_pLR->Band( miCurrentBand ).AttributeByName( std::string(CurrentName()) ); 
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
	SVLightReferenceAttributeInfo* pAttribute = m_pLR->Band( miCurrentBand ).AttributeByType( CurrentType() );
	if ( pAttribute )
		pAttribute->lValue = (long) dw;
	else
	{
		pAttribute = m_pLR->Band( miCurrentBand ).AttributeByName( std::string(CurrentName()) );
		if ( pAttribute )
			pAttribute->lValue = (long) dw;
		else
		{
			ASSERT( FALSE );
		}
	}
}
