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

#include "stdafx.h"
#include "SVLightReferenceDialog.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVAcquisitionClass.h"


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
		if (hr == S_OK)
		{
			if (pLR->NumBands() == 0)
			{
				CString sCaption;
				if( !( pDevice.empty() ) )
				{
					sCaption.Format("%s (Dig_%d.Ch_%d)", pCamera->GetName(), pDevice->DigNumber(), pDevice->Channel());
				}
				else
				{
					sCaption = pCamera->GetName();
				}
				CString s;
				s.Format(_T("Light Reference not available for %s"), sCaption);
				AfxMessageBox(s);
				DestroyAllPages();
				return false;
			}

			int iMaxBand = iCamBand;    // will only ever need to do one set of tabs per camera
			// Viper Quad has only one band
			// Viper RGB color has 3 bands but are handled as channels on the tab
			// Viper RGB mono has 1 band per camera, 3 cameras
			for (int iBand=iCamBand; iBand <= iMaxBand; iBand++)
			{
				SVLightReferenceBand* pLRBand = &(pLR->Band(iBand));
				for (int iAttribute=0; iAttribute < pLRBand->NumAttributes(); iAttribute++)
				{
					// the IDs are no longer valid... use the text instead
					CString sTabText;
					CString sDigName; sDigName.Format(_T("Dig.%d"), pDevice->DigNumber());
					sTabText = pCamera->GetName() + CString(_T(" ")) + sDigName + CString(_T(" ")) + pLRBand->Attribute( iAttribute ).strName.c_str();
					SVLightReferenceDialogPropertyPageClass* pPage = new SVLightReferenceDialogPropertyPageClass( sTabText );
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
		}// end if (hr == S_OK)
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
	mpAcquisition = NULL;
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
	CString strTmp;
	strTmp.Format("%ld",min);
	GetDlgItem(IDC_MIN)->SetWindowText(strTmp);
	strTmp.Format("%ld",max);
	GetDlgItem(IDC_MAX)->SetWindowText(strTmp);

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

	if( mpDevice == NULL || mpLR == NULL || miAttributeType == -1 || mpCamera == NULL )
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
				if ( mpLR->Band(i).AttributeByName(SVString(CurrentName())) == NULL )
					bEnable = FALSE;
			}
			pWnd->EnableWindow(bEnable);
		}
	}

	if( pWnd = GetDlgItem( IDC_CHANNEL3 ) )
	{
		BOOL bEnable = miNumBands > 1 && (miBandSize > 3 || miCamBand == 3);
		if ( mpLR->Band(miCamBand).AttributeByName(SVString(CurrentName())) == NULL )
			bEnable = FALSE;
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

//** EOF **//

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLightReferenceDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:46:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   28 Sep 2011 12:58:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated source code to fix issues with camera image interface and methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   30 Mar 2011 16:06:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   22 Mar 2011 07:57:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated static global classes to singletons to promote proper and defined construction and destruction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   01 Jun 2010 10:49:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   16 Dec 2009 11:24:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   16 Sep 2008 14:24:30   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  634
   SCR Title:  Implement a File Acquistion Device
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised CreatePages to check if LightReference is available
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   21 Jun 2007 13:27:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   30 Jul 2004 11:45:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  441
   SCR Title:  Fix Light Reference Dialog - White Balance
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   01 Apr 2004 10:22:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  407
   SCR Title:  Fix Multi-band Issues with Light Reference Dialog
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added method CurrentName()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Dec 2003 11:43:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with attributes that are not on all bands
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   16 Dec 2003 12:04:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  402
   SCR Title:  Add Min and Max Values to Light Reference Dialog
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Made changes to Light Reference Dialog to display Min and Max at ends of slider control. OnSetActive()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   17 Nov 2003 16:27:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed problem handling multi-banded cams
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   22 Oct 2003 09:34:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed to reference attributes by type, not index
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Jul 2003 11:28:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed SVLightReferenceArray to SVLightReference
   general cleanup
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   22 Apr 2003 10:39:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   08 Dec 2002 22:27:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  311
   SCR Title:  Fix Light Reference save for RGB Mono
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated SVLightReferenceDialogPropertyPageClass::OnInitDialog to fix an issue with invalid member variables or parameters.
   
   Updated CreatePages method of the SVLightReferenceDialogPropertyPageClass to reflect changes in references of SVAcquisitionClass member variables to accessor functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   02 Dec 2002 11:37:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  302
   SCR Title:  Light Reference Dialog updates all bands through one camera for RGB Mono
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed function to update through the SVVirtualCamera object instead of the SVAcquisitionClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   19 Nov 2002 14:54:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Completely redone to use new architecture
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   17 Oct 2000 08:31:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  185
   SCR Title:  Color SVIM Light Reference Problem
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixes channel selection problem in light reference dialog and acquisition tool dialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   19 Sep 2000 09:31:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  185
   SCR Title:  Color SVIM Light Reference Problem
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     This fixes the problem with multi-channel digitizers.  It also restores the slider bar to the channel setting upon changing channels.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 13:36:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed references to SVToolSetView and added header.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
