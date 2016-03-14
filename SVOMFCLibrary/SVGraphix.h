//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGraphixDrawObjectClass
//* .File Name       : $Workfile:   SVGraphix.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:06:30  $
//******************************************************************************

#ifndef SVGRAPHIX_H
#define SVGRAPHIX_H

#include "SVTemplate.h"

enum SVGraphixOverlayModeEnum
{
    SVStretchMode,
    SVClipMode,
    SVClipCenteredMode
};


enum SVGraphixDrawObjectEnum
{
	SVDrawObjectNotValid,
	// Don't change the order!!!!!
	SVDrawObjectPoint,
	SVDrawObjectFilledRect,
	SVDrawObjectRect,
	SVDrawObjectFilledEllipse,
	SVDrawObjectEllipse,
	SVDrawObjectPolygon,
	SVDrawObjectPolyline,
	SVDrawObjectFill
	// Add here new objects...
};

class SVGraphixDrawObjectClass
{
public:
	void Serialize( CArchive& RArchive )
	{
		if( RArchive.IsStoring() )
		{
			RArchive << color
					 << penStyle
					 << bSolidBrush;
		}
		else
		{
			RArchive >> color
					 >> penStyle
					 >> bSolidBrush;

			// Force object to use serialized
			// color, penStyle and bSolidBrush
			// instead of default settings...
			SetDrawingColor( color );
		}

		// Serialize point array...
		pointArray.Serialize( RArchive );
	}

	SVGraphixDrawObjectClass()
	{
		maxPointCount	= 0;
		color			= RGB( 0, 0, 0 );
		penStyle		= PS_SOLID;
		bSolidBrush		= TRUE;
		hPen			= NULL;
		hBrush			= NULL;
		bNeedPen		= FALSE;
		bNeedBrush		= FALSE;
		objectType		= SVDrawObjectNotValid;
	};

	virtual ~SVGraphixDrawObjectClass()
	{
		if( hPen )
			::DeleteObject( hPen );
		hPen = NULL;
		
		if( hBrush )
			::DeleteObject( hBrush );
		hBrush = NULL;
	};

	void SetPenStyle( int NewStyle )
	{
		if( hPen )
			::DeleteObject( hPen );
		hPen = NULL;

		penStyle = NewStyle;
		if( bNeedPen )
			hPen = ::CreatePen( penStyle, 1, color );
	};

	void SetBrushStyle( BOOL BSolid )
	{
		if( hBrush )
			::DeleteObject( hBrush );
		hBrush = NULL;

		bSolidBrush = BSolid;
		if( bNeedBrush )
		{
			if( BSolid )
				hBrush	= ::CreateSolidBrush( color );
			else
				hBrush	= ::CreateHatchBrush( HS_DIAGCROSS, color );
		}
	};


	void SetDrawingColor( COLORREF Color )
	{
		color = Color;
		SetPenStyle( penStyle );
		SetBrushStyle( bSolidBrush );
	};

	const COLORREF& GetDrawingColor()
	{
		return color;
	};

	BOOL AddPoint( const POINT& NewPoint )
	{
		if( pointArray.GetSize() <= maxPointCount )
		{
			pointArray.Add( ( POINT ) NewPoint );
			return TRUE;
		}
		return FALSE;
	};

	BOOL RemovePoint( POINT& RemovedPoint )
	{
		if( pointArray.GetSize() > 0 )
		{
			INT_PTR size = pointArray.GetUpperBound();
			RemovedPoint = pointArray.ElementAt( size );
			pointArray.SetSize( size );
			return TRUE;
		}
		return FALSE;
	};

	BOOL ReplacePoint( POINT& ReplacePoint )
	{
		if( pointArray.GetSize() > 0 )
		{
			INT_PTR last = pointArray.GetUpperBound();
			POINT oldPoint = pointArray.ElementAt( last );
			pointArray.ElementAt( last ) = ReplacePoint;
			ReplacePoint = oldPoint;
			return TRUE;
		}
		return FALSE;
	};

	INT_PTR GetPointCount()
	{
		return pointArray.GetSize();
	};

	SVGraphixDrawObjectEnum	GetObjectType()
	{
		return objectType;
	};
	

protected:

	// current PEN ( and BRUSH ) color
	COLORREF color;
	// current PEN style PS_????
	int		 penStyle;
	// current BRUSH solid or hatch ( style: HS_DIAGCROSS )?
	BOOL	 bSolidBrush;
	HPEN	 hPen;
	HBRUSH	 hBrush;

	// Contents one or more points used 
	// by this drawing object.
	CArray< POINT, POINT& > pointArray;

	// Max. number of points for this object
	int maxPointCount;

	BOOL bNeedPen;
	BOOL bNeedBrush;

	SVGraphixDrawObjectEnum	objectType;

public:
	virtual void Draw( HDC HDeviceContext )
	{};

};



class SVGraphixPointDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPointDrawObjectClass()
	{
		bNeedPen		= FALSE;
		bNeedBrush		= FALSE;
		maxPointCount   = 1;
		objectType		= SVDrawObjectPoint;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		// Draw...
		for( int i = 0; i < pointArray.GetSize(); ++ i )
			::SetPixel( HDeviceContext, pointArray[ i ].x, pointArray[ i ].y, color );
	};
};




class SVGraphixFilledRectDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFilledRectDrawObjectClass()
	{
		bNeedPen		= TRUE;
		bNeedBrush		= TRUE;
		maxPointCount   = 2;
		// Create pen...
		SetPenStyle( penStyle );
		// Create brush...
		SetPenStyle( bSolidBrush );
		objectType		= SVDrawObjectFilledRect;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		if( pointArray.GetSize() >= maxPointCount )
		{
			HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );
			HBRUSH hOldBrush = ( HBRUSH ) ::SelectObject( HDeviceContext, hBrush );

			// Draw...
			::Rectangle( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y, pointArray[ 1 ].x, pointArray[ 1 ].y );

			::SelectObject( HDeviceContext, hOldPen );
			::SelectObject( HDeviceContext, hOldBrush );
		}
	};
};



class SVGraphixRectDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixRectDrawObjectClass()
	{
		bNeedPen		= TRUE;
		bNeedBrush		= FALSE;
		maxPointCount   = 2;
		// Create pen...
		SetPenStyle( penStyle );
		objectType		= SVDrawObjectRect;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		if( pointArray.GetSize() >= maxPointCount )
		{
			HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );

			// Draw...
			::MoveToEx( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y, NULL );
			::LineTo( HDeviceContext, pointArray[ 1 ].x, pointArray[ 0 ].y );
			::LineTo( HDeviceContext, pointArray[ 1 ].x, pointArray[ 1 ].y );
			::LineTo( HDeviceContext, pointArray[ 0 ].x, pointArray[ 1 ].y );
			::LineTo( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y );

			::SelectObject( HDeviceContext, hOldPen );
		}
	};
};



class SVGraphixFilledEllipseDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFilledEllipseDrawObjectClass()
	{
		bNeedPen		= TRUE;
		bNeedBrush		= TRUE;
		maxPointCount   = 2;
		// Create pen...
		SetPenStyle( penStyle );
		// Create brush...
		SetPenStyle( bSolidBrush );
		objectType		= SVDrawObjectFilledEllipse;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		if( pointArray.GetSize() >= maxPointCount )
		{
			HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );
			HBRUSH hOldBrush = ( HBRUSH ) ::SelectObject( HDeviceContext, hBrush );

			// Draw...
			::Ellipse( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y, pointArray[ 1 ].x, pointArray[ 1 ].y );

			::SelectObject( HDeviceContext, hOldPen );
			::SelectObject( HDeviceContext, hOldBrush );
		}
	};
};




class SVGraphixEllipseDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixEllipseDrawObjectClass()
	{
		bNeedPen		= TRUE;
		bNeedBrush		= FALSE;
		maxPointCount   = 2;
		// Create pen...
		SetPenStyle( penStyle );
		objectType		= SVDrawObjectEllipse;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		if( pointArray.GetSize() >= maxPointCount )
		{
			HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );

			// Draw...
			::Arc( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y, pointArray[ 1 ].x, pointArray[ 1 ].y, 0, 0, 0, 0 );

			::SelectObject( HDeviceContext, hOldPen );
		}
	};
};


class SVGraphixPolygonDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPolygonDrawObjectClass()
	{
		bNeedPen		= TRUE;
		bNeedBrush		= TRUE;
		maxPointCount   = 32767;
		// Create pen...
		SetPenStyle( penStyle );
		// Create brush...
		SetPenStyle( bSolidBrush );
		objectType		= SVDrawObjectPolygon;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );
		HBRUSH hOldBrush = ( HBRUSH ) ::SelectObject( HDeviceContext, hBrush );

		// Draw...
		::Polygon( HDeviceContext, pointArray.GetData(), static_cast< int >( pointArray.GetSize() ) );

		::SelectObject( HDeviceContext, hOldPen );
		::SelectObject( HDeviceContext, hOldBrush );
	};
};


class SVGraphixPolylineDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPolylineDrawObjectClass()
	{
		bNeedPen	  = TRUE;
		bNeedBrush	  = FALSE;
		maxPointCount = 32767;
		// Create pen...
		SetPenStyle( penStyle );
		objectType		= SVDrawObjectPolyline;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, hPen );

		// Draw...
		::Polyline( HDeviceContext, pointArray.GetData(), static_cast< int >( pointArray.GetSize() ) );

		::SelectObject( HDeviceContext, hOldPen );
	};
};


class SVGraphixFillDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFillDrawObjectClass()
	{
		bNeedPen	  = FALSE;
		bNeedBrush	  = TRUE;
		maxPointCount = 1;
		// Create brush...
		SetPenStyle( bSolidBrush );
		objectType		= SVDrawObjectFill;
	};

	virtual void Draw( HDC HDeviceContext )
	{
		if( pointArray.GetSize() >= maxPointCount )
		{
			HBRUSH hOldBrush = ( HBRUSH ) ::SelectObject( HDeviceContext, hBrush );

			// Draw...
			::FloodFill( HDeviceContext, pointArray[ 0 ].x, pointArray[ 0 ].y, color );

			::SelectObject( HDeviceContext, hOldBrush );
		}
	};
};












////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGraphixClass
// -----------------------------------------------------------------------------
// .Description : Provides Graphic Object Drawing on Bitmap Canvas and more...
//              : Stretching, clipping, ....
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :07.04.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVGraphixClass
{
//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	void Serialize( CArchive& RArchive );
	SVGraphixClass();
	virtual ~SVGraphixClass();

	void FlushDrawObjects();

    // BYTE SVGraphixClass::GetBackgroundMono8Color();
    // Get monochrome unsigned 8 Bit background color.
    // Return values: [0...255], [Black...Gray...White]
    // NOTE: It always returns the red value of COLORREF backGroundColor!
    //       That means, if this class would be expanded for more than
    //       monochrome 8 Bit color usage, this function should be replaced!
    BYTE GetBackgroundMono8Color()
    {
        return GetRValue( backGroundColor );
    };

    // SVGraphixOverlayModeEnum SVGraphixClass::GetOverlayMode();
    // Overlay Modes:
    // SVStretchMode         - Stretch Graphix drawing size to destination size
    // SVClipMode            - Draw Graphix on LEFT TOP position of destination, 
    //                         clip Graphix, if necessary.
    // SVClipCenteredMode    - Center Graphix by drawing to center of destination, 
    //                         clip Graphix, if necessary.
    SVGraphixOverlayModeEnum GetOverlayMode()
    {
        return overlayMode;
    };

    void GetGraphixRect( RECT& RRect, int ScaleX = 1, int ScaleY = 1 )
	{
		RRect = rect;
		if( ScaleX )
		{
			RRect.left   = ( ScaleX < 0 ) ? ( rect.left   / ( -ScaleX ) ) : ( rect.left   * ScaleX );
			RRect.right  = ( ScaleX < 0 ) ? ( rect.right  / ( -ScaleX ) ) : ( rect.right  * ScaleX );
		}

		if( ScaleY )
		{
			RRect.top    = ( ScaleY < 0 ) ? ( rect.top    / ( -ScaleY ) ) : ( rect.top    * ScaleY );
			RRect.bottom = ( ScaleY < 0 ) ? ( rect.bottom / ( -ScaleY ) ) : ( rect.bottom * ScaleY );
		}
    };

    void SetGraphixRect( const RECT& RRect )
    {
        rect = RRect;
    };

    // void SVGraphixClass::SetRasterOperationCode( DWORD DwNewROP = SRCCOPY );
    // DwNewROP: Refer to BitBlt for a list of common raster operation codes...
    void SetRasterOperationCode( DWORD DwNewROP = SRCCOPY )
    {
        dwROP = DwNewROP;
    };

    // void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum NewOverlayMode = SVStretchMode );
    // Overlay Modes:
    // SVStretchMode         - Stretch Graphix drawing size to destination size
    // SVClipMode            - Draw Graphix on LEFT TOP position of destination, 
    //                         clip Graphix, if necessary.
    // SVClipCenteredMode    - Center Graphix by drawing to center of destination, 
    //                         clip Graphix, if necessary.
    // NOTE: If Graphix size is smaller than destination size, Graphix Background color is used
    //       to fill remaining destination areas.
    void SetOverlayMode( SVGraphixOverlayModeEnum NewOverlayMode = SVStretchMode )
    {
        overlayMode = NewOverlayMode;
    };

    // void SVGraphixClass::SetBackgroundMono8Color( BYTE Mono8Color = 255 );
    // Set monochrome unsigned 8 Bit background color.
    // Valid values: [0...255], [Black...Gray...White]
    void SetBackgroundMono8Color( BYTE Mono8Color = 255 )
    {
        backGroundColor = RGB( Mono8Color, Mono8Color, Mono8Color );
    };

    // void SVGraphixClass::Draw( HDC HDestinyDC, const RECT& RDestinyRect );
    // Draw Graphix to destination dc.
    // Refer to:
    //  void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  void SVGraphixClass::SetRasterOperationCode( DWORD );
    //  void SVGraphixClass::SetBackgroundMono8Color( BYTE );
    void Draw( HDC HDestinyDC, const RECT& RDestinyRect );

	void InsertDrawObject( SVGraphixDrawObjectClass* PNewDrawObject )
	{
		if( PNewDrawObject )
			drawObjectArray.Add( PNewDrawObject );
	}

	// Check if point lies in Graphix canvas...
	BOOL CheckPoint( const POINT& RPoint, int ScaleX = 1, int ScaleY = 1 )
	{
		POINT pointLT, pointRB;
		pointLT.x = rect.left;
		pointLT.y = rect.top;
		pointRB.x = rect.right;
		pointRB.y = rect.bottom;

		// Regard overlay mode...
		ScalePoint( pointLT, TRUE );
		ScalePoint( pointRB, TRUE );

		// Regard extern scale factors...
		if( ScaleX )
		{
			pointLT.x = ( ScaleX < 0 ) ? ( pointLT.x / ( -ScaleX ) ) : ( pointLT.x * ScaleX );
			pointRB.x = ( ScaleX < 0 ) ? ( pointRB.x / ( -ScaleX ) ) : ( pointRB.x * ScaleX );
		}

		if( ScaleY )
		{
			pointLT.y = ( ScaleY < 0 ) ? ( pointLT.y / ( -ScaleY ) ) : ( pointLT.y * ScaleY );
			pointRB.y = ( ScaleY < 0 ) ? ( pointRB.y / ( -ScaleY ) ) : ( pointRB.y * ScaleY );
		}

		return( RPoint.x >= pointLT.x && RPoint.y >= pointLT.y  &&
			    RPoint.x <  pointRB.x && RPoint.y <  pointRB.y );
	};

	// Rescale point using current overlay mode...
	// It uses the scale factors of the last 
	// call to 
	//	void SVGraphixClass::Draw( HDC HDestinyDC, const RECT& RDestinyRect )
	void ScalePoint( POINT& RPoint, BOOL BDown = TRUE )
	{
		if( scaleX != 0.0 )
			RPoint.x = ( BDown ) ? ( ( long ) ( ( ( double ) RPoint.x ) / scaleX ) ) : ( ( long ) ( ( ( double ) RPoint.x ) * scaleX ) );
		if( scaleY != 0.0 )
			RPoint.y = ( BDown ) ? ( ( long ) ( ( ( double ) RPoint.y ) / scaleY ) ) : ( ( long ) ( ( ( double ) RPoint.y ) * scaleY ) );
	};

	// SVGraphixDrawObjectClass* 
	//		SVGraphixClass::GetDrawObject( SVGraphixDrawObjectEnum NewObject, BOOL BInsert = TRUE );
	// Creates new draw object.
	// If BInsert == TRUE ( Default ) the new object
	//	will be inserted into to the drawing list.
	SVGraphixDrawObjectClass* GetNewDrawObject( SVGraphixDrawObjectEnum NewObject, BOOL BInsert = TRUE );

	// Get serialized graphix data as a global memory handle
	// You have to free the memory!
	HGLOBAL GetGraphixData();

	// Set serialized graphix data as a global memory handle
	// You have allocate and unlock this handle,
	// the memory is freed by this function!
	// If FALSE is returned, you have to free the memory!
	BOOL SetGraphixData( HGLOBAL HGlobalMem );

protected:

    // dwROP: Refer to BitBlt for a list of common raster operation codes...
    DWORD   dwROP;

    // overlayMode: 
    //  Refer to 
    //      void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  for a list...
    SVGraphixOverlayModeEnum    overlayMode;

    // backGroundColor:
    //  Current background color, used as Graphix background and
    //  fill color, if Graphix size smaller than destination size
    //  Refer to
    //  void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  void SVGraphixClass::SetBackgroundMono8Color( BYTE );
    //  BYTE SVGraphixClass::GetBackgroundMono8Color();
    COLORREF    backGroundColor;

    // rect:
    //  Size of current Graphix canvas.
    //  ( Could be greater, equal or smaller than destination size )
    RECT    rect;

	// will be calculated in Draw() and
	// performs the ScalePoint() rescaling
	// valid values: scaleX > 0
	double  scaleX;
	// will be calculated in Draw() and
	// performs the ScalePoint() rescaling
	// valid values: scaleY > 0
	double	scaleY;

	// Contains the graphix draw objects
	CArray< SVGraphixDrawObjectClass*, SVGraphixDrawObjectClass* > drawObjectArray;
};

#endif

