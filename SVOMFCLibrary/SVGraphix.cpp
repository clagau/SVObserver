//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGraphixDrawObjectClass
//* .File Name       : $Workfile:   SVGraphix.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:06:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVGraphix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif







//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVGraphixClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

//******************************************************************************
// Serialization:
//******************************************************************************
void SVGraphixClass::Serialize( CArchive& RArchive )
{
	if( RArchive.IsStoring() )
	{
		INT_PTR i( 0 );
		INT_PTR size = drawObjectArray.GetSize();
		// Clean up NULL pointer...
		for( i = size - 1 ; i >= 0; -- i )
		{
			if( drawObjectArray.GetAt( i ) == NULL )
				drawObjectArray.RemoveAt( i );
		}

		// Serialize draw objects...
		size = drawObjectArray.GetSize();
		RArchive << size;
		for( i = 0; i < size; ++i )
		{
			RArchive << drawObjectArray.GetAt( i )->GetObjectType();
			drawObjectArray.GetAt( i )->Serialize( RArchive );
		}


		// Serialize graphix data...
		RArchive << dwROP
			     << overlayMode
				 << backGroundColor;

		RArchive << rect.left
				 << rect.top
				 << rect.right
				 << rect.bottom;

	}
	else
	{
		FlushDrawObjects();
		
		int size = 0;
		int objectType = 0;
		SVGraphixDrawObjectClass* pNewObject = NULL;
		// Serialize draw objects...
		RArchive >> size;
		for( int i = 0; i < size; ++i )
		{
			RArchive >> objectType;
			// Create and insert draw object...
			if( pNewObject = GetNewDrawObject( ( SVGraphixDrawObjectEnum ) objectType ) )
			{
				pNewObject->Serialize( RArchive );
			}
			else
			{
				SVGraphixDrawObjectClass dummyObject;
				dummyObject.Serialize( RArchive );
			}
		}

		// Serialize graphix data...
		int dummy;
		RArchive >> dwROP
			     >> dummy
				 >> backGroundColor;

		overlayMode = ( SVGraphixOverlayModeEnum ) dummy;

		RArchive >> rect.left
				 >> rect.top
				 >> rect.right
				 >> rect.bottom;

	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVGraphixClass::SVGraphixClass()
{
    // Default raster operation code: overwrite
    dwROP = SRCCOPY;

    // Default overlay mode: stretch
    overlayMode = SVStretchMode;

    // Default background color: white
    backGroundColor = RGB( 255, 255, 255 );

    // Default size of Graphix canvas: 100 x 100
    rect.left = 0;
    rect.top  = 0;
    rect.right  = 50;
    rect.bottom = 50;

	// Default scale factors...
	scaleX = 1.0;	// No scaling
	scaleY = 1.0;	// No scaling
}

SVGraphixClass::~SVGraphixClass()
{
	FlushDrawObjects();
}

void SVGraphixClass::FlushDrawObjects()
{
	for( INT_PTR i = drawObjectArray.GetSize() - 1; i >= 0; -- i )
		if( drawObjectArray.GetAt( i ) )
		{
			delete drawObjectArray.GetAt( i );
			drawObjectArray.SetAt( i, NULL );
		}

	drawObjectArray.RemoveAll();
}



void SVGraphixClass::Draw( HDC HDestinyDC, const RECT& RDestinyRect )
{
    HDC     hMemDC              = NULL;
    HBRUSH  hBackgroundBrush    = NULL;
    HBRUSH  hOldBrush           = NULL;
    HBITMAP hBM                 = NULL;
    HBITMAP hBMHolder           = NULL;
    HBITMAP hOldBM              = NULL;

    while( 1 )
    {
        int graphixWidth  = rect.right - rect.left;
        int graphixHeight = rect.bottom - rect.top;
        int destWidth  = RDestinyRect.right - RDestinyRect.left;
        int destHeight = RDestinyRect.bottom - RDestinyRect.top;
            
        if( ! ( hMemDC = ::CreateCompatibleDC( HDestinyDC ) ) )
            break;

        if( ! ( hBackgroundBrush = ::CreateSolidBrush( backGroundColor ) ) )
            break;

        if( ! ( hBM = ::CreateCompatibleBitmap( HDestinyDC, graphixWidth, graphixHeight ) ) )
            break;

        // Select background brush for memDC...
        hOldBrush = ( HBRUSH ) ::SelectObject( hMemDC, hBackgroundBrush );

        // Select bitmap for memDC...
        if( hOldBM = ( HBITMAP ) ::SelectObject( hMemDC, hBM ) )
        {
//JAB103108 - Do not delete old bitmap object?
//            ::DeleteObject( hOldBM );
//            hOldBM = NULL;
        }

        // Set memDC background color...
        ::PatBlt( hMemDC, rect.left, rect.top, graphixWidth, graphixHeight, PATCOPY );

        // Deselect background brush for memDC...
        hBackgroundBrush = ( HBRUSH ) ::SelectObject( hMemDC, hOldBrush );

        // Draw graphix...
        //...
		for( int i = 0; i < drawObjectArray.GetSize(); ++ i )
			if( drawObjectArray[ i ] )
				drawObjectArray[ i ]->Draw( hMemDC );


        // Select background brush for destiny DC...
        hOldBrush = ( HBRUSH ) ::SelectObject( HDestinyDC, hBackgroundBrush );

        if( overlayMode != SVStretchMode )
        {
            // Set destiny DC background color...
            ::PatBlt( HDestinyDC, RDestinyRect.left, RDestinyRect.top, destWidth, destHeight, PATCOPY );
        }

        // Set stretch mode...
        ::SetStretchBltMode( HDestinyDC, COLORONCOLOR );

        switch( overlayMode )
        {
            case SVStretchMode: 
            {
                ::StretchBlt( HDestinyDC, RDestinyRect.left, RDestinyRect.top, destWidth, destHeight, 
				              hMemDC, rect.left, rect.top, graphixWidth, graphixHeight, dwROP );

				scaleX = ( double ) graphixWidth  / ( double ) destWidth;
				scaleY = ( double ) graphixHeight / ( double ) destHeight;
            }
            break;

            case SVClipMode: 
            {
                ::BitBlt( HDestinyDC, RDestinyRect.left, RDestinyRect.top, graphixWidth, graphixHeight,
			              hMemDC, rect.left, rect.top, dwROP );

				scaleX = 1.0;
				scaleY = 1.0;
            }
            break;

            case SVClipCenteredMode: 
            {
                int destLeft = RDestinyRect.left + ( destWidth - graphixWidth ) / 2;
                int destTop  = RDestinyRect.top + ( destHeight - graphixHeight ) / 2;
                ::BitBlt( HDestinyDC, destLeft, destTop, graphixWidth, graphixHeight,
			              hMemDC, rect.left, rect.top, dwROP );

				scaleX = 1.0;
				scaleY = 1.0;
            }
            break;
        }

        // Deselect background brush for destiny DC...
        hBackgroundBrush = ( HBRUSH ) ::SelectObject( HDestinyDC, hOldBrush );

        // Deselect bitmap for memDC...
        hBMHolder = ( HBITMAP ) ::SelectObject( hMemDC, hOldBM );

//JAB103108		::DeleteObject( hBM );
        break;
    }

	if ( hBM )
	{
        ::DeleteObject( hBM );
	}
    if( hBackgroundBrush )
        ::DeleteObject( hBackgroundBrush );

    if( hMemDC )
        ::DeleteDC( hMemDC );
}



SVGraphixDrawObjectClass* SVGraphixClass::GetNewDrawObject( SVGraphixDrawObjectEnum NewObject, BOOL BInsert )
{
	SVGraphixDrawObjectClass* pDrawObject = NULL;

	switch( NewObject )
	{
		case SVDrawObjectPoint: 
			pDrawObject = new SVGraphixPointDrawObjectClass;
			break;

		case SVDrawObjectFilledRect: 
			pDrawObject = new SVGraphixFilledRectDrawObjectClass;
			break;
			
		case SVDrawObjectRect:
			pDrawObject = new SVGraphixRectDrawObjectClass;
			break;

		case SVDrawObjectFilledEllipse:
			pDrawObject = new SVGraphixFilledEllipseDrawObjectClass;
			break;

		case SVDrawObjectEllipse:
			pDrawObject = new SVGraphixEllipseDrawObjectClass;
			break;

		case SVDrawObjectPolygon:
			pDrawObject = new SVGraphixPolygonDrawObjectClass;
			break;

		case SVDrawObjectPolyline:
			pDrawObject = new SVGraphixPolylineDrawObjectClass;
			break;

		case SVDrawObjectFill:
			pDrawObject = new SVGraphixFillDrawObjectClass;
			break;

	}

	ASSERT( pDrawObject );

	if( BInsert ) 
		InsertDrawObject( pDrawObject );

	return pDrawObject;
}



HGLOBAL SVGraphixClass::GetGraphixData() 
{
	CMemFile memFile;
	memFile.SeekToBegin();
	CArchive archive( &memFile, CArchive::store );
	Serialize( archive );
	archive.Close();

	DWORD memFileSize = (DWORD)memFile.GetLength() + 10;
	HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, memFileSize );
	BYTE* pMem = ( BYTE* ) ::GlobalLock( hMem );
	if( hMem && pMem )
	{
		BYTE* pMemFile = memFile.Detach();
		memcpy( pMem, pMemFile, memFileSize );
		::GlobalUnlock( pMem );
		memFile.Attach( pMemFile, ( UINT ) memFileSize );

		return hMem;
	}
	return NULL;
}

BOOL SVGraphixClass::SetGraphixData( HGLOBAL HGlobalMem ) 
{
	UINT memFileSize = ( UINT ) ::GlobalSize( HGlobalMem ); 
	BYTE* pMemFile = ( BYTE* ) ::GlobalLock( HGlobalMem );
	if( pMemFile && memFileSize )
	{
		CMemFile memFile;
		memFile.Attach( pMemFile, memFileSize );
		memFile.SeekToBegin();

		CArchive archive( &memFile, CArchive::load );
		Serialize( archive );
		archive.Close();

		memFile.Detach();
		
		::GlobalUnlock( pMemFile );
		::GlobalFree( HGlobalMem );

		return TRUE;
	}
	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVGraphix.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:06:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   14 Nov 2008 13:28:10   jbrown
   Project:  SVObserver
   Change Request (SCR) nbr:  641
   SCR Title:  BoundsChecker results
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Improved handling of GDI objects within
   SVGraphixClass::Draw ()
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   04 Jun 2008 10:38:24   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed Typographical error in Draw where ScaleX was used instead of ScaleY.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   30 Oct 2007 15:21:18   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added DeleteObject in Draw to free GDI resource.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   21 Jun 2007 12:52:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   27 May 2003 16:06:58   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated beta to 19 and readded accidentally removed serialization code from the mask editor.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   22 Apr 2003 09:59:46   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/