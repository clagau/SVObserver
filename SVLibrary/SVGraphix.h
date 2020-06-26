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

#pragma once

#pragma region Inlcudes
//Moved to precompiled header: #include <vector>
#pragma endregion Inlcudes

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
	SVGraphixDrawObjectClass() = default;

	virtual ~SVGraphixDrawObjectClass();

	virtual void Draw(HDC ){};

	void SetPenStyle( int NewStyle );
	void SetBrushStyle( bool bSolidBrush );
	void SetDrawingColor( COLORREF Color );
	
	bool AddPoint(const POINT& rNewPoint);
	bool ReplacePoint(POINT& rReplacePoint);

	const COLORREF& GetDrawingColor() const { return m_color; }

	int getPenStyle() { return m_penStyle; }
	bool getBrushStyle() { return m_isSolidBrush; }
	size_t GetPointCount() { return m_pointVector.size(); }
	std::vector<POINT>& getPointVector() { return m_pointVector; }

	SVGraphixDrawObjectEnum	GetObjectType() { return m_objectType; }

protected:
	// Pen and brush color
	COLORREF m_color {RGB(0, 0, 0)};
	int		 m_penStyle {PS_SOLID};
	//Is BRUSH solid or hatch, style: HS_DIAGCROSS
	bool	 m_isSolidBrush {true};
	HPEN	 m_hPen {nullptr};
	HBRUSH	 m_hBrush {nullptr};

	// Contents one or more points used 
	// by this drawing object.
	std::vector<POINT> m_pointVector;

	// Max. number of points for this object
	int m_maxPointCount {0L};

	bool m_needPen {false};
	bool m_needBrush {false};

	SVGraphixDrawObjectEnum	m_objectType {SVDrawObjectNotValid};
};

class SVGraphixPointDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPointDrawObjectClass()
	{
		m_needPen		= false;
		m_needBrush		= false;
		m_maxPointCount = 1;
		m_objectType	= SVDrawObjectPoint;
	}

	virtual ~SVGraphixPointDrawObjectClass () = default;

	virtual void Draw( HDC hDC ) override
	{
		// Draw...
		for(const auto& rPoint : m_pointVector)
		{
			::SetPixel( hDC, rPoint.x, rPoint.y, m_color );
		}
	}
};

class SVGraphixFilledRectDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFilledRectDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush = true;
		m_maxPointCount = 2;
		m_objectType = SVDrawObjectFilledRect;
		// Create pen...
		SetPenStyle( m_penStyle );
		// Create brush...
		SetBrushStyle( m_isSolidBrush );
	}

	virtual ~SVGraphixFilledRectDrawObjectClass() = default;

	virtual void Draw( HDC hDC ) override
	{
		if( static_cast<long> (m_pointVector.size()) >= m_maxPointCount )
		{
			HPEN   hOldPen   = static_cast<HPEN> (::SelectObject( hDC, m_hPen ));
			HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject( hDC, m_hBrush ));

			// Draw...
			::Rectangle( hDC, m_pointVector[0].x, m_pointVector[0].y, m_pointVector[1].x, m_pointVector[1].y );

			::SelectObject( hDC, hOldPen );
			::SelectObject( hDC, hOldBrush );
		}
	}
};

class SVGraphixRectDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixRectDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush	= false;
		m_maxPointCount = 2;
		m_objectType = SVDrawObjectRect;
		// Create pen...
		SetPenStyle( m_penStyle );
	}

	virtual ~SVGraphixRectDrawObjectClass() = default;

	virtual void Draw( HDC HDeviceContext ) override
	{
		if(static_cast<long> (m_pointVector.size()) >= m_maxPointCount)
		{
			HPEN   hOldPen   = ( HPEN ) ::SelectObject( HDeviceContext, m_hPen );

			// Draw...
			::MoveToEx( HDeviceContext, m_pointVector[0].x, m_pointVector[0].y, nullptr );
			::LineTo( HDeviceContext, m_pointVector[1].x, m_pointVector[0].y );
			::LineTo( HDeviceContext, m_pointVector[1].x, m_pointVector[1].y );
			::LineTo( HDeviceContext, m_pointVector[0].x, m_pointVector[1].y );
			::LineTo( HDeviceContext, m_pointVector[0].x, m_pointVector[0].y );

			::SelectObject( HDeviceContext, hOldPen );
		}
	}
};

class SVGraphixFilledEllipseDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFilledEllipseDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush	= true;
		m_maxPointCount = 2;
		m_objectType = SVDrawObjectFilledEllipse;
		// Create pen...
		SetPenStyle( m_penStyle );
		// Create brush...
		SetBrushStyle( m_isSolidBrush );
	}

	virtual ~SVGraphixFilledEllipseDrawObjectClass() = default;

	virtual void Draw( HDC HDeviceContext ) override
	{
		if(static_cast<long> (m_pointVector.size()) >= m_maxPointCount)
		{
			HPEN   hOldPen   = static_cast<HPEN> (::SelectObject( HDeviceContext, m_hPen ));
			HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject( HDeviceContext, m_hBrush ));

			// Draw...
			::Ellipse( HDeviceContext, m_pointVector[0].x, m_pointVector[0].y, m_pointVector[1].x, m_pointVector[1].y );

			::SelectObject( HDeviceContext, hOldPen );
			::SelectObject( HDeviceContext, hOldBrush );
		}
	}
};

class SVGraphixEllipseDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixEllipseDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush = false;
		m_maxPointCount = 2;
		m_objectType = SVDrawObjectEllipse;
		// Create pen...
		SetPenStyle( m_penStyle );
	}

	virtual ~SVGraphixEllipseDrawObjectClass() = default;

	virtual void Draw( HDC hDC ) override
	{
		if(static_cast<long> (m_pointVector.size()) >= m_maxPointCount)
		{
			HPEN hOldPen   = static_cast<HPEN> (::SelectObject( hDC, m_hPen ));

			// Draw...
			::Arc( hDC, m_pointVector[0].x, m_pointVector[0].y, m_pointVector[1].x, m_pointVector[1].y, 0, 0, 0, 0 );

			::SelectObject( hDC, hOldPen );
		}
	}
};

class SVGraphixPolygonDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPolygonDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush	= true;
		m_maxPointCount   = 32767;
		m_objectType = SVDrawObjectPolygon;
		// Create pen...
		SetPenStyle( m_penStyle );
		// Create brush...
		SetBrushStyle( m_isSolidBrush );
	}

	virtual ~SVGraphixPolygonDrawObjectClass() = default;

	virtual void Draw( HDC hDC ) override
	{
		HPEN   hOldPen   = static_cast<HPEN> (::SelectObject( hDC, m_hPen ));
		HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject( hDC, m_hBrush ));

		// Draw...
		::Polygon( hDC, m_pointVector.data(), static_cast< int >( m_pointVector.size() ) );

		::SelectObject( hDC, hOldPen );
		::SelectObject( hDC, hOldBrush );
	}
};

class SVGraphixPolylineDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixPolylineDrawObjectClass()
	{
		m_needPen = true;
		m_needBrush = false;
		m_maxPointCount = 32767;
		m_objectType = SVDrawObjectPolyline;
		// Create pen...
		SetPenStyle( m_penStyle );
	}

	virtual ~SVGraphixPolylineDrawObjectClass()
	{
	}

	virtual void Draw( HDC hDC ) override
	{
		HPEN hOldPen = static_cast<HPEN> (::SelectObject( hDC, m_hPen ));

		// Draw...
		::Polyline( hDC, m_pointVector.data(), static_cast< int >( m_pointVector.size() ) );

		::SelectObject( hDC, hOldPen );
	}
};

class SVGraphixFillDrawObjectClass : public SVGraphixDrawObjectClass
{
public:
	SVGraphixFillDrawObjectClass()
	{
		m_needPen = false;
		m_needBrush	= true;
		m_maxPointCount = 1;
		m_objectType = SVDrawObjectFill;
		// Create brush...
		SetBrushStyle( m_isSolidBrush );
	}

	virtual ~SVGraphixFillDrawObjectClass() = default;

	virtual void Draw( HDC hDC ) override
	{
		if(static_cast<long> (m_pointVector.size()) >= m_maxPointCount)
		{
			HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject( hDC, m_hBrush ));

			// Draw...
			::FloodFill( hDC, m_pointVector[0].x, m_pointVector[0].y, m_color );

			::SelectObject( hDC, hOldBrush );
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGraphixClass
// -----------------------------------------------------------------------------
// .Description : Provides Graphic Object Drawing on Bitmap Canvas and more...
//              : Stretching, clipping, ....
////////////////////////////////////////////////////////////////////////////////
class SVGraphixClass
{
//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	SVGraphixClass();
	virtual ~SVGraphixClass();

	std::string store(bool convertToHex = false);
	void load(const std::string& rData, bool convertFromHex = false);

	void FlushDrawObjects();

    // BYTE SVGraphixClass::GetBackgroundMono8Color();
    // Get monochrome unsigned 8 Bit background color.
    // Return values: [0...255], [Black...Gray...White]
    // NOTE: It always returns the red value of COLORREF backGroundColor!
    //       That means, if this class would be expanded for more than
    //       monochrome 8 Bit color usage, this function should be replaced!
    BYTE GetBackgroundMono8Color() { return GetRValue( m_backGroundColor ); }

    // SVGraphixOverlayModeEnum SVGraphixClass::GetOverlayMode();
    // Overlay Modes:
    // SVStretchMode         - Stretch Graphix drawing size to destination size
    // SVClipMode            - Draw Graphix on LEFT TOP position of destination, 
    //                         clip Graphix, if necessary.
    // SVClipCenteredMode    - Center Graphix by drawing to center of destination, 
    //                         clip Graphix, if necessary.
    SVGraphixOverlayModeEnum GetOverlayMode() { return m_overlayMode; }

    void SetGraphixRect( const RECT& rRect ) { m_rect = rRect; }

    // void SVGraphixClass::SetRasterOperationCode( DWORD DwNewROP = SRCCOPY );
    // DwNewROP: Refer to BitBlt for a list of common raster operation codes...
    void SetRasterOperationCode( DWORD newROP = SRCCOPY ) { m_dwROP = newROP; }

    // void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum NewOverlayMode = SVStretchMode );
    // Overlay Modes:
    // SVStretchMode         - Stretch Graphix drawing size to destination size
    // SVClipMode            - Draw Graphix on LEFT TOP position of destination, 
    //                         clip Graphix, if necessary.
    // SVClipCenteredMode    - Center Graphix by drawing to center of destination, 
    //                         clip Graphix, if necessary.
    // NOTE: If Graphix size is smaller than destination size, Graphix Background color is used
    //       to fill remaining destination areas.
    void SetOverlayMode( SVGraphixOverlayModeEnum NewOverlayMode = SVStretchMode ) { m_overlayMode = NewOverlayMode; }

    // void SVGraphixClass::SetBackgroundMono8Color( BYTE Mono8Color = 255 );
    // Set monochrome unsigned 8 Bit background color.
    // Valid values: [0...255], [Black...Gray...White]
    void SetBackgroundMono8Color( BYTE Mono8Color = 255 ) { m_backGroundColor = RGB( Mono8Color, Mono8Color, Mono8Color ); }

    // void SVGraphixClass::Draw( HDC HDestinyDC, const RECT& RDestinyRect );
    // Draw Graphix to destination dc.
    // Refer to:
    //  void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  void SVGraphixClass::SetRasterOperationCode( DWORD );
    //  void SVGraphixClass::SetBackgroundMono8Color( BYTE );
    void Draw( HDC hDC, const RECT& rRect );

	void InsertDrawObject( SVGraphixDrawObjectClass* pNewDrawObject );

	// Check if point lies in Graphix canvas...
	bool CheckPoint( const POINT& rPoint, int ScaleX = 1, int ScaleY = 1 );

	// Rescale point using current overlay mode...
	// It uses the scale factors of the last 
	// call to 
	//	void SVGraphixClass::Draw( HDC HDestinyDC, const RECT& RDestinyRect )
	void ScalePoint( POINT& rPoint, bool bDown = true );

	// SVGraphixDrawObjectClass* 
	//		SVGraphixClass::GetDrawObject( SVGraphixDrawObjectEnum NewObject, BOOL BInsert = TRUE );
	// Creates new draw object.
	// If BInsert == TRUE ( Default ) the new object
	//	will be inserted into to the drawing list.
	SVGraphixDrawObjectClass* GetNewDrawObject( SVGraphixDrawObjectEnum NewObject, bool bInsert = true );

	// Get serialized graphix data as a global memory handle
	// You have to free the memory!
	HGLOBAL GetGraphixData();

	// Set serialized graphix data as a global memory handle
	// You have allocate and unlock this handle,
	// the memory is freed by this function!
	// If FALSE is returned, you have to free the memory!
	bool SetGraphixData( HGLOBAL hGlobalMem );

	void undoOnStep();
	bool canUndoSteps() const { return 0 < m_drawObjectVector.size(); };
	void redoOnStep();
	bool canRedoSteps() const { return 0 < m_redoObjectVector.size(); };

protected:

    // dwROP: Refer to BitBlt for a list of common raster operation codes...
    DWORD   m_dwROP;

    // overlayMode: 
    //  Refer to 
    //      void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  for a list...
    SVGraphixOverlayModeEnum    m_overlayMode;

    // backGroundColor:
    //  Current background color, used as Graphix background and
    //  fill color, if Graphix size smaller than destination size
    //  Refer to
    //  void SVGraphixClass::SetOverlayMode( SVGraphixOverlayModeEnum );
    //  void SVGraphixClass::SetBackgroundMono8Color( BYTE );
    //  BYTE SVGraphixClass::GetBackgroundMono8Color();
    COLORREF    m_backGroundColor;

    // rect:
    //  Size of current Graphix canvas.
    //  ( Could be greater, equal or smaller than destination size )
    RECT    m_rect;

	// will be calculated in Draw() and
	// performs the ScalePoint() rescaling
	// valid values: scaleX > 0
	double  m_scaleX;
	// will be calculated in Draw() and
	// performs the ScalePoint() rescaling
	// valid values: scaleY > 0
	double	m_scaleY;

	// Contains the graphix draw objects
	std::vector<SVGraphixDrawObjectClass*> m_drawObjectVector;
	std::vector<SVGraphixDrawObjectClass*> m_redoObjectVector;
};


