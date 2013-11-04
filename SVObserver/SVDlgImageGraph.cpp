//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageGraph
//* .File Name       : $Workfile:   SVDlgImageGraph.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgImageGraph.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectCPointArrayClass.h"
#include "SVImageClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVDlgImageGraphClass, SVDlgImageClass)
	//{{AFX_MSG_MAP(SVDlgImageGraphClass)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVDlgImageGraphClass::SVDlgImageGraphClass()
{
	m_dxMult = 0.0;
	m_dyMult = 0.0;
	m_lxDisp = 0;
	m_lyDisp = 0;
	m_lyClient = 0;

	m_dxScale = 1.0;
	m_dyScale = 1.0;

	m_ptPosition.x = 0;
	m_ptPosition.y = 0;

	m_xorMode = FALSE;

	m_hDefaultPen = ::CreatePen( PS_DOT, 1, RGB(0,255,0) );
}

SVDlgImageGraphClass::~SVDlgImageGraphClass()
{
	ClearPoints();

	if( m_hDefaultPen )
		DeleteObject( m_hDefaultPen );
}

void SVDlgImageGraphClass::init( SVImageClass* pImage )
{
	SVDlgImageClass::UpdateImageInfo( pImage );

	// Get Scale factors
	m_dxScale = ( double )m_ClientRect.Width() / ( double )m_SourceRect.right;
	m_dyScale = ( double )m_ClientRect.Height() / ( double )m_SourceRect.bottom;
}

void SVDlgImageGraphClass::init( const SVImageInfoClass* pImageInfo, SVSmartHandlePointer pImageHandle )
{
	SVDlgImageClass::UpdateImageInfo( pImageInfo, pImageHandle );
	
	// Get Scale factors
	m_dxScale = ( double )m_ClientRect.Width() / ( double )m_SourceRect.right;
	m_dyScale = ( double )m_ClientRect.Height() / ( double )m_SourceRect.bottom;
}

BOOL SVDlgImageGraphClass::AddPoints( SVDrawObjectListClass* PDrawObjectList, SVGraphNormalizeModeEnum mode )
{
	BOOL rc = TRUE;

	int size = PDrawObjectList->GetSize();
	
	for( int i = 0; i < size; i++ )
	{
		SVDrawObjectClass l_svDrawObject = PDrawObjectList->GetAt( i );
		rc = AddPoints( l_svDrawObject, mode, false ) && rc;
	}

	return ( rc && RedrawWindow() );
}

BOOL SVDlgImageGraphClass::AddPoints( const SVDrawObjectClass &p_rsvDrawObject, SVGraphNormalizeModeEnum mode, bool l_bRedraw )
{
	SVDrawObjectVectorStruct drawObjectVector;
	
	SVObjectCPointArrayClass* pPointVector = new SVObjectCPointArrayClass;

	drawObjectVector.normalizeMode = mode;

	drawObjectVector.pPointVector = pPointVector;
	drawObjectVector.m_svDrawObject = p_rsvDrawObject;

	const SVCPointArray& rPointArray = drawObjectVector.m_svDrawObject.GetPointArray();

	CopyPoints( rPointArray, pPointVector );
	
	BOOL rc = TRUE;

	if( mode == SVGraphScale ) //|| mode == SVGraphScaleAndFill )
	{
		Scale( drawObjectVector );
	}
	else if( mode == SVGraphNormalize1D || mode == SVGraphNormalize2D )
	{
		rc = Normalize( drawObjectVector );
	}
	
	if( rc )
		m_drawList.Add( drawObjectVector );

	if( rc && l_bRedraw )
	{
		rc = RedrawWindow();
	}

	return ( rc );
}

BOOL SVDlgImageGraphClass::ClearPoints()
{
	int i,s = m_drawList.GetSize();
	for (i = 0; i < s; i++)
	{
		SVDrawObjectVectorStruct& rOb = m_drawList.ElementAt(i);
		if ( rOb.pPointVector != NULL )
			delete rOb.pPointVector;
	}
	m_drawList.RemoveAll();
	return TRUE;
}

void SVDlgImageGraphClass::CopyPoints( const SVCPointArray& rPointArray, SVObjectCPointArrayClass* pPointVector )
{
	int size = rPointArray.GetSize();

	pPointVector->SetSize( size );

	for( int n = 0; n < size; n++ )
	{
		CPoint& rPoint = ( *pPointVector )[ n ];
		CPoint srcPoint = rPointArray.GetAt( n );
		
		// Make relative to the Image being displayed
		rPoint.x = srcPoint.x - m_ptPosition.x;
		rPoint.y = srcPoint.y - m_ptPosition.y;
	}
}

BOOL SVDlgImageGraphClass::Normalize( SVDrawObjectVectorStruct& RDrawObjectVector ) 
{
	SVObjectCPointArrayClass* pPointVec = RDrawObjectVector.pPointVector;

	if( ::IsWindow( m_hWnd ) )
	{
		CRect clientRect;
		GetClientRect( &clientRect );
		long xMax = 0;
		long xMin = 0;
		long yMax = 0;
		long yMin = 0;
		int size = pPointVec->GetSize();
		int i( 0 );
		
		// Get extremes...
		for( i = 0; i < size; ++ i )
		{
			CPoint& rPoint = (*pPointVec)[ i ];
			xMax = __max( xMax, rPoint.x );
			xMin = __min( xMin, rPoint.x );
			yMax = __max( yMax, rPoint.y );
			yMin = __min( yMin, rPoint.y );
		}

		// Get Displacement...
		m_lxDisp = - xMin;
		m_lyDisp = - yMin;

		if( RDrawObjectVector.normalizeMode == SVGraphNormalize2D )
		{
			// Make a square of extremes...
			xMax = yMax = __max( xMax, yMax );
			xMin = yMin = __min( xMin, yMin );
		}

		// Calc x multiplier...
		m_dxMult = clientRect.Width();
		if( xMax - xMin != 0 )
			m_dxMult /= xMax - xMin;
		else
			m_dxMult = 0;

		// Calc y multiplier...
		m_dyMult = clientRect.Height();
		if( yMax - yMin != 0 )
			m_dyMult /= yMax - yMin;
		else
			m_dyMult = 0;

		//m_lyClient = clientRect.Height();

		// Normalize...
		for( i = 0; i < size; ++ i )
		{
			CPoint& rPoint = (*pPointVec)[ i ];
			rPoint.x = ( long ) ( ( double ) ( rPoint.x + m_lxDisp ) * m_dxMult );
			//rPoint.y = m_lyClient - ( long ) ( ( double ) ( rPoint.y + m_lyDisp ) * m_dyMult );
			rPoint.y = ( long ) ( ( double ) ( rPoint.y + m_lyDisp ) * m_dyMult );
		}

		return TRUE;
	}
	return FALSE;
}

void SVDlgImageGraphClass::Scale( SVDrawObjectVectorStruct& RDrawObjectVector )
{
	SVObjectCPointArrayClass* pPointVec = RDrawObjectVector.pPointVector;

	int size = pPointVec->GetSize();

	for( int i = 0; i < size; i++ )
	{
		CPoint& rPoint = (*pPointVec)[ i ];
		rPoint.x = ( long ) ( ( double ) ( rPoint.x * m_dxScale ) );
				
		// Clip Point to be contained inside of image display
		if( rPoint.x < 0 ) 
			rPoint.x = 0;

		if( rPoint.x >= m_ClientRect.Width() ) 
			rPoint.x = m_ClientRect.Width() - 1;

		rPoint.y = ( long ) ( ( double ) ( rPoint.y * m_dyScale ) );

		// Clip Point to be contained inside of image display
		if( rPoint.y < 0 ) 
			rPoint.y = 0;

		if( rPoint.y >= m_ClientRect.Height() ) 
			rPoint.y = m_ClientRect.Height() - 1;
	}
}

void SVDlgImageGraphClass::RefreshPoints()
{
	SVObjectCPointArrayClass* pPointVector;

	for( int i = 0; i < m_drawList.GetSize(); i++ )
	{
		SVDrawObjectVectorStruct& rDrawObjectVector = m_drawList.GetAt(i);

		pPointVector = rDrawObjectVector.pPointVector;

		const SVCPointArray& rPointArray = rDrawObjectVector.m_svDrawObject.GetPointArray();

		CopyPoints( rPointArray, pPointVector );

		if( rDrawObjectVector.normalizeMode == SVGraphScale ) //||
			//rDrawObjectVector.normalizeMode == SVGraphScaleAndFill )
		{
			Scale( rDrawObjectVector );
		}
		else if( rDrawObjectVector.normalizeMode == SVGraphNormalize1D || 
				 rDrawObjectVector.normalizeMode == SVGraphNormalize2D )
		{
			Normalize( rDrawObjectVector );
		}
	}
	// Update Display
	refresh();
}

void SVDlgImageGraphClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if ( mbInit )
	{
		

		SVSmartHandlePointer l_ImageHandle;

		if ( mpSourceImage != NULL )
		{
			mpSourceImage->GetImageHandle( l_ImageHandle );
		}
		else
		{
			l_ImageHandle = mSourceImageHandle;
		}
		
		SVImageBufferHandleImage l_MilHandle;
		SVBitmapInfo DibInfo;
		SVMatroxBuffer l_NewBuffer;
		HBITMAP hbm = NULL;
		HBITMAP hOld = NULL;
		BYTE* pMilBuffer = NULL;

		if( !( l_ImageHandle.empty() ) )
		{
			l_ImageHandle->GetData( l_MilHandle );

			DibInfo = l_ImageHandle->GetBitmapInfo();
			pMilBuffer = ( BYTE* )l_ImageHandle->GetBufferAddress();

			SVMatroxBufferInterface::SVStatusCode l_Code = 
				SVMatroxBufferInterface::Create(l_NewBuffer, l_MilHandle.GetBuffer() );
			if( l_Code == S_OK)
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer( l_NewBuffer, l_MilHandle.GetBuffer());
				l_Code = SVMatroxBufferInterface::GetBitmapInfo( DibInfo, l_NewBuffer );
				l_Code = SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, l_NewBuffer );
			}
		}

		if( pMilBuffer != NULL && !( DibInfo.empty() ) )
		{
			BOOL b = m_DIB.DeleteObject();
			HRESULT hr = m_DIB.LoadDIBitmap( DibInfo.GetBitmapInfo(), pMilBuffer );
			
			if ( (HBITMAP) m_DIB != NULL )
			{
				hbm = (HBITMAP) m_DIB;
			}// end if

		}// end if
		
		//
		// Render the device independent bitmap (DIB) (stretching as required).
		//
		if ( hbm != NULL )
		{
			/////////////////////////////////////
			// Create 2 memory DCs
			/////////////////////////////////////
			CDC memDC[2];
			memDC[0].CreateCompatibleDC( &dc );
			memDC[1].CreateCompatibleDC( &dc );
			
			///////////////////////////////////////////
			// Create a bitmap for the Stretched Image
			///////////////////////////////////////////
			CBitmap imageBitmap;
			imageBitmap.CreateCompatibleBitmap( &dc, m_ClientRect.Width(), m_ClientRect.Height() );
			
			//
			// Render the device independent bitmap (DIB) (stretching as required).
			//
			HBITMAP hBitmapOld[2];
			hBitmapOld[0] = ( HBITMAP ) ::SelectObject( memDC[0].m_hDC, imageBitmap.GetSafeHandle() );
			hBitmapOld[1] = ( HBITMAP ) ::SelectObject( memDC[1].m_hDC, hbm );
			
			
			/////////////////////////////////////
			// Stretch the Image bitmap
			/////////////////////////////////////
			memDC[0].StretchBlt( 0, 0, m_ClientRect.Width(), m_ClientRect.Height(), 
				&memDC[1], 0, 0, m_SourceRect.right, m_SourceRect.bottom, SRCCOPY );
			
			/////////////////////////////////////
			// Select the Old bitmap
			/////////////////////////////////////
			::SelectObject( memDC[1].m_hDC, hBitmapOld[1] );
			
			/////////////////////////////////////
			// Set transparent background mode
			/////////////////////////////////////
			memDC[0].SetBkMode( TRANSPARENT );
			memDC[1].SetBkMode( TRANSPARENT );
			
			/////////////////////////////////////
			// Draw graphs...
			/////////////////////////////////////
			for( int i = 0;i < m_drawList.GetSize(); i++)
			{
				SVDrawObjectVectorStruct& rDrawObjectVector = m_drawList.GetAt(i);
				
				long size = rDrawObjectVector.pPointVector->GetSize();
				
				if( size > 0 )
				{
					CPen* pPen;
					
					HPEN hPen = ( HPEN )rDrawObjectVector.m_svDrawObject.GetDrawPen();
					
					if( !hPen )
					{
						// use default Pen
						pPen = CPen::FromHandle( m_hDefaultPen );
					}
					else
					{
						pPen = CPen::FromHandle( hPen );
					}
					
					/////////////////////////////////////
					// Select the pen
					/////////////////////////////////////
					CPen* pOldPen = memDC[0].SelectObject( pPen );
					
					/////////////////////////////////////
					// Draw the point set
					/////////////////////////////////////
					if( m_xorMode )
					{
						CRect rect;
						CPoint point;
						// Big Kludge!!!!
						// Assume it's a rectangle
						point = rDrawObjectVector.pPointVector->GetAt( 0 );
						rect.left = point.x;
						rect.top = point.y;
						
						point = rDrawObjectVector.pPointVector->GetAt( 1 );
						rect.right = point.x;
						
						point = rDrawObjectVector.pPointVector->GetAt( 2 );
						rect.bottom = point.y;
						
						memDC[0].DrawFocusRect( &rect );
					}
					else
					{
						if (rDrawObjectVector.m_svDrawObject.DrawSinglePoints())
						{
							COLORREF color = rDrawObjectVector.m_svDrawObject.GetPenColor();
							const CPoint* pPoints = rDrawObjectVector.pPointVector->GetData();
							for (int i = 0;i < size;i++)
							{
								memDC[0].SetPixel(pPoints[i].x, pPoints[i].y, color);
							}
						}
						else
						{
							memDC[0].Polyline( rDrawObjectVector.pPointVector->GetData(), size );
						}
					}
					
					/////////////////////////////////////
					// Select the old pen
					/////////////////////////////////////
					memDC[0].SelectObject( pOldPen );
				}
			}
			/////////////////////////////////////
			// Blit it to the Screen DC
			/////////////////////////////////////
			dc.BitBlt( 0, 0, m_ClientRect.Width(), m_ClientRect.Height(), &memDC[0], 0, 0, SRCCOPY );
			
			/////////////////////////////////////
			// Select the old bitmap
			/////////////////////////////////////
			::SelectObject( memDC[0].m_hDC, hBitmapOld[0] );
			
			/////////////////////////////////////
			// Delete the Stretched Image Bitmap
			/////////////////////////////////////
			imageBitmap.DeleteObject();
			
			/////////////////////////////////////
			// Delete the memory DCs
			/////////////////////////////////////
			memDC[0].DeleteDC();
			memDC[1].DeleteDC();
		}
		l_NewBuffer.clear();
	}

// Do not call CStatic::OnPaint() for painting messages
}

void SVDlgImageGraphClass::TransparentStretchBlit( CDC* pDC, CBitmap& rBitmap, COLORREF transparentColor )
{
	CDC DCMem[2];
	CBitmap* pOldBitmaps[2];
	CBitmap bitmapMask;

	BITMAP bmp;

	/////////////////////////////////////
	// Create two Memory Device Contexts
	/////////////////////////////////////
	DCMem[0].CreateCompatibleDC( pDC );
	DCMem[1].CreateCompatibleDC( pDC );

	/////////////////////////////////////
	// Get the Bitmap Info
	/////////////////////////////////////
	rBitmap.GetBitmap( &bmp );

	/////////////////////////////////////
	// Create the mask
	/////////////////////////////////////
	bitmapMask.CreateBitmap( bmp.bmWidth, bmp.bmHeight, 1, 1, NULL );


	/////////////////////////////////////
	// Select the Bitmap into the DC
	/////////////////////////////////////
	pOldBitmaps[0] = DCMem[0].SelectObject( &rBitmap );

	/////////////////////////////////////
	// Select the Bitmap Mask
	/////////////////////////////////////
	pOldBitmaps[1] = DCMem[1].SelectObject( &bitmapMask );

	/////////////////////////////////////
	// Set Transparent Color
	/////////////////////////////////////
	DCMem[0].SetBkColor( transparentColor );

	/////////////////////////////////////
	// Blit to the Mask DC
	/////////////////////////////////////
	DCMem[1].BitBlt( 0, 0, bmp.bmWidth, bmp.bmHeight, &DCMem[0], 0, 0, SRCCOPY );

	/////////////////////////////////////
	// Transparent Blit to the Real DC
	/////////////////////////////////////
	pDC->BitBlt( 0, 0, bmp.bmWidth, bmp.bmHeight, &DCMem[0], 0, 0, SRCINVERT );
	pDC->BitBlt( 0, 0, bmp.bmWidth, bmp.bmHeight, &DCMem[1], 0, 0, SRCAND );
	pDC->BitBlt( 0, 0, bmp.bmWidth, bmp.bmHeight, &DCMem[0], 0, 0, SRCINVERT );

	/////////////////////////////////////
	// Select the Original Bitmaps
	/////////////////////////////////////
	DCMem[0].SelectObject( pOldBitmaps[0] );
	DCMem[1].SelectObject( pOldBitmaps[1] );

	/////////////////////////////////////
	// Delete the mask
	/////////////////////////////////////
	bitmapMask.DeleteObject();

	/////////////////////////////////////
	// Delete the Memory Device Contexts
	/////////////////////////////////////
	DCMem[0].DeleteDC();
	DCMem[1].DeleteDC();
}

void SVDlgImageGraphClass::OnDestroy() 
{
	SVDlgImageClass::OnDestroy();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageGraph.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   07 Apr 2011 16:32:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   10 Feb 2011 14:39:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   27 Jan 2011 11:06:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Dec 2009 09:01:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   31 Dec 2008 11:32:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnPaint to support DrawSinglePoints
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 May 2008 11:40:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Temperary Buffer for OnPaint message handler so GetHostAddress and GetBitmapinfo will succeed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Jul 2007 11:47:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2007 12:27:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 May 2005 14:57:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Feb 2005 13:49:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jan 2004 10:58:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed MIL Display Resources to dislplay information to dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Aug 2003 09:01:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ClearPoints function
 * added m_ prefix to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2003 12:58:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added new init method
 * removed dead parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 09:09:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jan 2003 15:26:08   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnPaint method with new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Nov 2002 11:56:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Cosmetic changes
 *             Changed MIL image id’s to wrapper structs
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 13:40:24   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Change all occurences of SVLongVector to SVObjectDoubleArrayClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Apr 17 2001 15:49:44   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Sep 2000 16:28:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2000 11:53:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in. New Dialog class to display an Image with a graph and figures overlayed on the image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
