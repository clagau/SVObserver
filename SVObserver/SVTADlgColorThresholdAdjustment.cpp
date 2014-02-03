//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdAdjustment
//* .File Name       : $Workfile:   SVTADlgColorThresholdAdjustment.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 12:09:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgColorThresholdAdjustment.h"

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"

#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVCommandInspectionRunOnce.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVSVIMStateClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdAdjustment, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdAdjustment)
	ON_BN_CLICKED(IDC_EXCLUDE_COLOR_THRESHOLD, OnExcludeColorThreshold)
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdAdjustment::SVTADlgColorThresholdAdjustment() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdAdjustment::IDD)
{
	mpUpperThreshold = NULL;
	mpLowerThreshold = NULL;
	mpExclude = NULL;
	mpCurrentDocument = NULL;
	mpTool = NULL;
	mpThreshold = NULL;
	mBandNumber = 0;

	//{{AFX_DATA_INIT(SVTADlgColorThresholdAdjustment)
	StrUpper = _T("");
	StrLower = _T("");
	m_exclude = FALSE;
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdAdjustment::~SVTADlgColorThresholdAdjustment()
{
}

HRESULT SVTADlgColorThresholdAdjustment::SetInspectionData()
{
	HRESULT l_hrOk = AddInputRequest( mpLowerThreshold, m_lowerThreshold );

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpUpperThreshold, m_upperThreshold );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpExclude, m_exclude );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpExtentLeft, m_pSheet->m_rectROI.left );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpExtentTop, m_pSheet->m_rectROI.top );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpExtentWidth, m_pSheet->m_rectROI.Width() );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( mpExtentHeight, m_pSheet->m_rectROI.Height() );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( mpTool );
	}

	return l_hrOk;
}

void SVTADlgColorThresholdAdjustment::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdAdjustment)
	DDX_Control(pDX, IDC_UPPER_THRESHOLD_SLIDER, UpperSliderCtrl);
	DDX_Control(pDX, IDC_LOWER_THRESHOLD_SLIDER, LowerSliderCtrl);
	DDX_Text(pDX, IDC_UPPER_THRESHOLD_STATIC, StrUpper);
	DDX_Text(pDX, IDC_LOWER_THRESHOLD_STATIC, StrLower);
	DDX_Check(pDX, IDC_EXCLUDE_COLOR_THRESHOLD, m_exclude);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COLOR_THRESHOLD_IMAGE, m_svDlgImage);
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdAdjustment message handlers

BOOL SVTADlgColorThresholdAdjustment::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();
	
	ASSERT( m_pSheet );

	mpTool = m_pSheet->GetTool();

	SetTaskObject( mpTool );

	if( mpTool->GetInspection() != NULL )
	{
		mpCurrentDocument = SVObjectManagerClass::Instance().GetIPDoc( mpTool->GetInspection()->GetUniqueObjectID() );
	}

	// Get the color threshold object
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SVOperatorObjectType;
	objectInfo.SubType = SVColorThresholdObjectType;

	mpThreshold = reinterpret_cast <SVColorThresholdClass*> (::SVSendMessage( mpTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) ) );

	if( mpThreshold )
	{
		// Get Train Color Extent Variables
		SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SVDoubleValueObjectType;

		// Get Train Color ROI Extent Left Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeLeftPositionObjectGuid;
		mpExtentLeft = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&extentObjectInfo) );

		// Get Train Color ROI Extent Top Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeTopPositionObjectGuid;
		mpExtentTop = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&extentObjectInfo) );

		// Get Train Color ROI Extent Width Object...
		extentObjectInfo.EmbeddedID = SVExtentWidthObjectGuid;
		mpExtentWidth = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&extentObjectInfo) );

		// Get Train Color ROI Extent Height Object...
		extentObjectInfo.EmbeddedID = SVExtentHeightObjectGuid;
		mpExtentHeight = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&extentObjectInfo) );
	}

	SVImageClass* pImage = NULL;
	
	// Get the Threshold variables
	objectInfo.SubType = SVNotSetSubObjectType;

	if( mBandNumber == 0 )
	{
		// Get the Image to Display
		pImage = mpThreshold->GetBand0OutputImage();

		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand0UpperThresholdObjectGuid;
		mpUpperThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.EmbeddedID = SVBand0LowerThresholdObjectGuid;
		mpLowerThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand0ThresholdExcludeObjectGuid;
		mpExclude = ( SVBoolValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
	}
	else if( mBandNumber == 1 )
	{
		pImage = mpThreshold->GetBand1OutputImage();
		
		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand1UpperThresholdObjectGuid;
		mpUpperThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.EmbeddedID = SVBand1LowerThresholdObjectGuid;
		mpLowerThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand1ThresholdExcludeObjectGuid;
		mpExclude = ( SVBoolValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
	}
	else
	{
		pImage = mpThreshold->GetBand2OutputImage();
		
		// Get the threshold variables
		objectInfo.ObjectType = SVLongValueObjectType;

		objectInfo.EmbeddedID = SVBand2UpperThresholdObjectGuid;
		mpUpperThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.EmbeddedID = SVBand2LowerThresholdObjectGuid;
		mpLowerThreshold = ( SVLongValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );

		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVBand2ThresholdExcludeObjectGuid;
		mpExclude = ( SVBoolValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&objectInfo) );
	}

	if( pImage )
	{
		m_svDlgImage.UpdateImageInfo( pImage );

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Get the Threshold bars from the SVColorThresholdClass
		// Note: Do this first
		SVDrawObjectListClass* pThresholdBarsFigure = mpThreshold->GetThresholdBarsFigure( mBandNumber );
		//m_svDlgImage.AddPoints( pThresholdBarsFigure );
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		SVDrawObjectClass* pGraphFigure = mpThreshold->GetGraphFigure( mBandNumber );
		//m_svDlgImage.AddPoints( *pGraphFigure, SVGraphScale );
		m_svDlgImage.AddOverlayPoints( *pGraphFigure, options );

		m_svDlgImage.refresh();
	}

	setInitialValues();

	updateGraphDisplay();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdAdjustment::getThresholdParams()
{
	// Update DDX variables
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
	{
		long value;
	
		mpLowerThreshold->GetValue( value );
		m_lowerThreshold = (unsigned char)value;

		mpUpperThreshold->GetValue( value );
		m_upperThreshold = (unsigned char)value;

		mpExclude->GetValue( m_exclude );

		UpdateData( FALSE );
	}
}

void SVTADlgColorThresholdAdjustment::setThresholdParams()
{
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
	{
		// Set the Values
		SetInspectionData();
	}
}

void SVTADlgColorThresholdAdjustment::updateControls()
{
	setScrollPos( &UpperSliderCtrl, m_upperThreshold );
	setScrollPos( &LowerSliderCtrl, m_lowerThreshold );
	
	StrUpper.Format( "%u", m_upperThreshold );
	StrLower.Format( "%u", m_lowerThreshold );

	UpdateData( FALSE );
}

void SVTADlgColorThresholdAdjustment::setInitialValues()
{
	if( mpUpperThreshold && mpLowerThreshold && mpExclude )
	{
		getThresholdParams();

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
	
		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );

		updateControls();

		updateGraphDisplay();
	}
}

void SVTADlgColorThresholdAdjustment::setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max )
{
	// Revise the scale ?
	m_Normalizer.SetRealRange( ( double )min, ( double )max );

	pSliderCtrl->SetRange( ( int ) m_Normalizer.GetNormalRangeMin(), ( int ) m_Normalizer.GetNormalRangeMax(), TRUE );
	pSliderCtrl->SetTic( ( int ) ( m_Normalizer.GetNormalRangeBaseSize() / 2 + m_Normalizer.GetNormalRangeMin() ) );
	int pageSize = ( int ) ( ( double ) ( m_Normalizer.GetNormalRangeBaseSize() / m_Normalizer.GetRealRangeBaseSize() ) );
	pSliderCtrl->SetPageSize( pageSize > 0 ? pageSize : 1 );
}

void SVTADlgColorThresholdAdjustment::setScrollPos( CSliderCtrl* pSliderCtrl, int pos )
{
	int iNormalizedValue = ( int ) m_Normalizer.CalcNormalizedValue( ( double ) pos );
	pSliderCtrl->SetPos( static_cast<int>(m_Normalizer.GetNormalRangeMax() - iNormalizedValue) );
}

void SVTADlgColorThresholdAdjustment::updateGraphDisplay()
{
	if( mpTool )
	{
		SVInspectionProcess* l_pInspection = mpTool->GetInspection();

		::SVSendMessage( mpTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

		if( mpTool != NULL && l_pInspection != NULL)
		{
			SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( l_pInspection->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( l_pInspection->GetUniqueObjectID(), l_CommandPtr );

			l_Command.Execute( 120000 );
		}

		m_svDlgImage.ClearOverlayPoints();

		SVDlgImageOverlayOptions options;
		options.sizeROI = CSize(255,255);
		options.rectMaxXY = CRect(0,0, 255,255);
		options.bFlipVertical = true;

		// Get the Threshold bars from the SVColorThresholdClass
		// Note: Do this first
		SVDrawObjectListClass* pThresholdBarsFigure = mpThreshold->GetThresholdBarsFigure( mBandNumber );
		m_svDlgImage.AddOverlayPoints( pThresholdBarsFigure, options );

		// Get the Graph Figure from the SVColorThresholdClass
		// Note: Do this last
		options.sizeROI = m_pSheet->m_rectROI.Size();
		options.bNormalizeY_ROI = true;
		options.bScaleY = false;
		SVDrawObjectClass* pGraphFigure = mpThreshold->GetGraphFigure( mBandNumber );
		//m_svDlgImage.AddPoints( *pGraphFigure, SVGraphScale );
		m_svDlgImage.AddOverlayPoints( *pGraphFigure, options );

		m_svDlgImage.refresh();
	}
}

void SVTADlgColorThresholdAdjustment::OnExcludeColorThreshold() 
{
	UpdateData();
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVTADlgColorThresholdAdjustment::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( mpUpperThreshold && mpLowerThreshold )
	{
		UpdateData( TRUE ); // get data of dialog	
	
		if( &UpperSliderCtrl == ( CSliderCtrl* ) pScrollBar )
		{
			m_upperThreshold = 255 - (unsigned char) m_Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			/*
			if ( m_upperThreshold < m_lowerThreshold )
			{
				m_upperThreshold = m_lowerThreshold;
				setScrollPos(&UpperSliderCtrl, m_upperThreshold);
			}
			//*/
			StrUpper.Format( "%u", m_upperThreshold );
		}
		else if( &LowerSliderCtrl == ( CSliderCtrl* ) pScrollBar )
		{
			m_lowerThreshold = 255 - (unsigned char) m_Normalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			/*
			if ( m_lowerThreshold > m_upperThreshold )
			{
				m_lowerThreshold = m_upperThreshold;
				setScrollPos(&LowerSliderCtrl, m_lowerThreshold);
			}
			//*/
			StrLower.Format( "%u", m_lowerThreshold );
		}

		UpdateData( FALSE ); // set data to dialog

		setThresholdParams();
		updateGraphDisplay();
	}
	
	SVTADlgColorThresholdBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL SVTADlgColorThresholdAdjustment::OnSetActive() 
{
	updateGraphDisplay();	
	return SVTADlgColorThresholdBasePage::OnSetActive();
}

void SVTADlgColorThresholdAdjustment::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( point.x == 0 && point.y == 0 )
	{
		// this is a reflected message from the image
		CPoint ptMouse = m_svDlgImage.GetMouseCoordinates();
		//m_sCoordinates.Format( _T("%d, %d"), ptMouse.x, ptMouse.y );
		//UpdateData(FALSE);

		if ( m_pSheet->m_pFigureEditor->IsCaptured() )
		{
			RefreshProperties();
			SetInspectionData();
			updateGraphDisplay();

			//m_svDlgImage.refresh();	// done in updateGraphDisplay
		}
	}
	else
	{
		SVTADlgColorThresholdBasePage::OnMouseMove(nFlags, point);
	}
}

void SVTADlgColorThresholdAdjustment::RefreshProperties()
{
	m_pSheet->m_rectROI = m_pSheet->m_pFigureEditor->GetRect();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgColorThresholdAdjustment.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 12:09:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:19:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   01 Aug 2012 12:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   09 Jul 2012 14:21:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   19 Jun 2012 14:25:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   07 Apr 2011 16:39:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Dec 2010 13:39:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Dec 2009 13:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Jun 2007 15:03:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Aug 2006 15:48:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Aug 2006 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Aug 2005 13:59:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed Tool Adjustment Dialog bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   24 Aug 2005 09:45:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Aug 2005 09:45:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Aug 2005 13:28:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Aug 2005 08:15:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jul 2005 14:49:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Jul 2005 11:50:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added static_cast for setting Threshold Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Feb 2005 10:48:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed color tool issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 15:49:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also use reset objects rather than recreating them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Feb 2005 15:19:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 15:43:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jan 2003 14:08:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the SetValue calls to send a long value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Nov 2002 13:34:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 18:47:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 May 2001 15:56:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * updateGraphDisplay
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
