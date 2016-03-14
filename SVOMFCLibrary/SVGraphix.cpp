//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGraphixDrawObjectClass
//* .File Name       : $Workfile:   SVGraphix.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   03 Jun 2014 13:48:50  $
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
		__int32 i( 0 );
		__int32 size = static_cast<__int32>(drawObjectArray.GetSize());
		// Clean up NULL pointer...
		for( i = size - 1 ; i >= 0; -- i )
		{
			if( drawObjectArray.GetAt( i ) == NULL )
				drawObjectArray.RemoveAt( i );
		}

		// Serialize draw objects...
		size = static_cast<__int32>(drawObjectArray.GetSize());
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
		
		__int32 size = 0;
		__int32 objectType = 0;
		SVGraphixDrawObjectClass* pNewObject = NULL;
		// Serialize draw objects...
		RArchive >> size;
		for( __int32 i = 0; i < size; ++i )
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

	size_t memFileSize = static_cast<size_t>(memFile.GetLength());
	HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, memFileSize );
	BYTE* pMem = ( BYTE* ) ::GlobalLock( hMem );
	if( hMem && pMem )
	{
		BYTE* pMemFile = memFile.Detach();
		memcpy( pMem, pMemFile, memFileSize );
		::GlobalUnlock( pMem );
		//Previous code attempted to free this memory by re-attaching to the CMemFile but it did not work. Use free.
		free(pMemFile);
		return hMem;
	}
	if(pMem) ::GlobalUnlock( pMem );
	if(hMem) ::GlobalFree( hMem );
	return NULL;
}

BOOL SVGraphixClass::SetGraphixData( HGLOBAL HGlobalMem ) 
{
	BOOL bRet = FALSE;
	SIZE_T memFileSize = ::GlobalSize( HGlobalMem ); 
	BYTE* pMemFile = ( BYTE* ) ::GlobalLock( HGlobalMem );
	if( pMemFile && memFileSize )
	{
		CMemFile memFile;
		memFile.Attach( pMemFile, static_cast<UINT>(memFileSize) );
		memFile.SeekToBegin();

		CArchive archive( &memFile, CArchive::load );
		Serialize( archive );
		archive.Close();

		memFile.Detach();
		
		::GlobalUnlock( pMemFile );

		bRet = TRUE;
	}
	::GlobalFree( HGlobalMem );

	return bRet;
}

