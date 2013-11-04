//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdROI
//* .File Name       : $Workfile:   SVTADlgColorThresholdROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:20:34  $
//******************************************************************************


#include "stdafx.h"
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdROI.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVValueObject.h"
#include "SVIPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdROI property page

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdROI, SVTADlgColorThresholdBasePage)


BEGIN_MESSAGE_MAP(SVTADlgColorThresholdROI, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdROI)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVTADlgColorThresholdROI::SVTADlgColorThresholdROI() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdROI::IDD)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdROI)
	mcsROIHeight = _T("");
	mcsROIWidth = _T("");
	mcsROIY = _T("");
	mcsROIX = _T("");
	m_strMouseColor = _T("");
	m_strMouseCoordinates = _T("");
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdROI::~SVTADlgColorThresholdROI()
{
}

void SVTADlgColorThresholdROI::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdROI)
	DDX_Text(pDX, IDC_COLOR_ROI_H_VALUE, mcsROIHeight);
	DDX_Text(pDX, IDC_COLOR_ROI_W_VALUE, mcsROIWidth);
	DDX_Text(pDX, IDC_COLOR_ROI_Y_VALUE, mcsROIY);
	DDX_Text(pDX, IDC_COLOR_ROI_X_VALUE, mcsROIX);
	DDX_Text(pDX, IDC_RGB_COLOR, m_strMouseColor);
	DDX_Text(pDX, IDC_COORDINATES, m_strMouseCoordinates);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdROI message handlers

BOOL SVTADlgColorThresholdROI::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();

	SetTaskObject( mpTool );

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SVOperatorObjectType;
	objectInfo.SubType = SVColorThresholdObjectType;

	// Get the color threshold object
	mpThreshold = ( SVColorThresholdClass* ) ::SVSendMessage( mpTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );

	if( mpThreshold )
	{
		// Get Train Color Extent Variables
		SVObjectTypeInfoStruct extentObjectInfo;
		extentObjectInfo.ObjectType = SVDoubleValueObjectType;

		// Get Train Color ROI Extent Left Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeLeftPositionObjectGuid;
		mpExtentLeft = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &extentObjectInfo );

		// Get Train Color ROI Extent Top Object...
		extentObjectInfo.EmbeddedID = SVExtentRelativeTopPositionObjectGuid;
		mpExtentTop = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &extentObjectInfo );

		// Get Train Color ROI Extent Width Object...
		extentObjectInfo.EmbeddedID = SVExtentWidthObjectGuid;
		mpExtentWidth = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &extentObjectInfo );

		// Get Train Color ROI Extent Height Object...
		extentObjectInfo.EmbeddedID = SVExtentHeightObjectGuid;
		mpExtentHeight = ( SVDoubleValueObjectClass* ) ::SVSendMessage( mpThreshold, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &extentObjectInfo );

		// Initialize Extent
		double value;
		mpExtentLeft->GetValue( value );
		m_pSheet->m_rectROI.left = (long)(value);

		mpExtentTop->GetValue( value );
		m_pSheet->m_rectROI.top = (long)(value);

		mpExtentWidth->GetValue( value );
		m_pSheet->m_rectROI.right = (long)(value) + m_pSheet->m_rectROI.left;

		mpExtentHeight->GetValue( value );
		m_pSheet->m_rectROI.bottom = (long)(value) + m_pSheet->m_rectROI.top;

		SVImageClass* pImage = mpTool->GetRGBImage();

		if( pImage )
		{
			SVImageInfoClass l_ImageInfo = pImage->GetImageInfo();

			l_ImageInfo.GetOutputRectangle( m_pSheet->m_ImageRect );

			m_pSheet->m_pFigureEditor->SetImageInfo( l_ImageInfo );
			m_pSheet->m_pFigureEditor->SetRect( m_pSheet->m_rectROI );

			m_svDlgImage.UpdateImageInfo( pImage );
			m_svDlgImage.refresh();
		}

		RefreshProperties();
		UpdateToolFigure();
	}// end if( mpThreshold )
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdROI::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( point.x == 0 && point.y == 0 )
	{
		// this is a reflected message from the image
		CPoint ptMouse = m_svDlgImage.GetMouseCoordinates();

		if ( m_pSheet->m_ImageRect.PtInRect( ptMouse ) )
		{
			// Mouse Color
			CPoint pt;
			::GetCursorPos(&pt);	// in screen coords
			ScreenToClient(&pt);
			HDC hDC = ::GetDC(m_hWnd);
			COLORREF rgb = ::GetPixel( hDC, pt.x, pt.y );
			::ReleaseDC( m_hWnd, hDC );
			m_strMouseColor.Format(_T("%d, %d, %d"), GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));


			// Mouse position
			m_strMouseCoordinates.Format( _T("(%d, %d)"), ptMouse.x, ptMouse.y );
			
			
			UpdateData(FALSE);
		}

		if ( m_pSheet->m_pFigureEditor->IsCaptured() )
		{
			RefreshProperties();
			UpdateToolFigure();

			m_svDlgImage.refresh();
		}
	}
	else
	{
		SVTADlgColorThresholdBasePage::OnMouseMove(nFlags, point);
	}
}

void SVTADlgColorThresholdROI::RefreshProperties()
{
	m_pSheet->m_rectROI = m_pSheet->m_pFigureEditor->GetRect();
}


void SVTADlgColorThresholdROI::UpdateToolFigure()
{
	SetInspectionData();

	// Update Tool Figure
	SVExtentFigureStruct l_svFigure = m_pSheet->m_rectROI;

	mcsROIX.Format(_T("%d"),m_pSheet->m_rectROI.left );
	mcsROIY.Format(_T("%d"),m_pSheet->m_rectROI.top );
	mcsROIHeight.Format(_T("%d"),m_pSheet->m_rectROI.Height() );
	mcsROIWidth.Format(_T("%d"),m_pSheet->m_rectROI.Width() );

	UpdateData(FALSE);
}

HRESULT SVTADlgColorThresholdROI::SetInspectionData()
{
	HRESULT l_hrOk = AddInputRequest( mpExtentLeft, m_pSheet->m_rectROI.left );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgColorThresholdROI.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:20:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   07 Apr 2011 16:39:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   25 Feb 2011 12:25:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   23 Aug 2006 09:46:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Re-Implemented the Mouse coordinates and color feedback
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Aug 2006 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Aug 2006 11:35:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced in-dialog editor with SVDlgImageEditor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Aug 2005 09:45:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Aug 2005 13:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Aug 2005 08:15:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
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
 *    Rev 1.6   12 Aug 2003 09:05:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   adjusted for m_ prefix change in SVDlgImageGraphClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 15:43:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2003 16:44:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jan 2003 16:06:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:34:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2001 17:12:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  224
 * SCR Title:  Fix 0.5 SVIM Color Tool lock-up problem when moving ROI
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Add type conversions in the OnInitDialog method.
 * 
 * Remove the UpdateDisplay method call in OnMouseMove.
 * 
 * Add UpdateDisplay method call in OnLButtonUp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:41:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

