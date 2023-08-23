//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawObject
//* .File Name       : $Workfile:   SVDrawObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Mar 2014 09:43:24  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVImageLibrary/SVExtentLineStruct.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVOGui/SVDrawContext.h"
#pragma endregion Includes

enum 
{
	InvalidPoint = INT_MIN,
};

class SVDrawObject
{
public:
	SVDrawObject();
	SVDrawObject( const SVDrawObject &p_rsvObject );
	~SVDrawObject();

	SVDrawObject &operator=( const SVDrawObject &p_rsvObject );

	void AddExtentLineData( SVExtentLineStruct p_svLine, int PenStyle = PS_SOLID );

	BOOL Draw( SVDrawContext* PDrawContext );
	BOOL DrawHatch( SVDrawContext* PDrawContext, int& LastY );

	// If BUseThisPen == FALSE, the current selected Pen 
	// of the DC will be used to Draw.
	BOOL SetDrawPen( BOOL BUseThisPen = FALSE, int PenStyle = PS_SOLID, int PenWidth = 1, COLORREF PenColor = RGB( 255, 255, 255 ) );

	void Transform( SVDrawContext* PDrawContext );

	int AddPoint( const POINT Point );
	void SetPointAtGrow( int Index, POINT Point );
	void SetListSize( int NewSize );
 
	POINT GetPointAt( int Index );

	const std::vector<POINT>& GetPointArray();
	
	HGDIOBJ GetDrawPen();

protected:
	BOOL beginDraw( SVDrawContext* PDrawContext );
	void endDraw( HDC DC );
	
	std::vector<POINT> m_Points;
	std::vector<POINT> m_CalcPoints;

	HGDIOBJ drawPen;	// If nullptr the current selected pen of the DC
						// will be used!

	HGDIOBJ oldPen;

	BOOL m_BUseThisPen;
	int m_PenStyle;
	int m_PenWidth;
	COLORREF m_PenColor;
};

typedef std::vector<SVDrawObject> SVDrawObjectClassVector;


/////////////////////////////////////////////////////////////////////////////
// SVPolyLineDrawObjectClass 

class SVDrawObjectList
{
public:
	SVDrawObjectList() {m_bDrawFigureHatched = false;};
	
	~SVDrawObjectList() {};

	void Add( const SVDrawObjectList& rObjectList )
	{
		m_DrawObjectVector.insert(m_DrawObjectVector.end(), rObjectList.getList().begin(), rObjectList.getList().end());
	};

	void Add( SVDrawObject& rObject )
	{
		m_DrawObjectVector.push_back( rObject );
	};

	void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, bool p_bDoNotUseLineColor = false )
	{
		long Count = static_cast<long> (p_svMultiLine.m_svLineArray.size());

		for( long l = 0; l < Count; l++ )
		{
			SVDrawObject DrawObject;

			DrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[l] );

			if( p_bDoNotUseLineColor )
			{
				DrawObject.SetDrawPen( FALSE );
			}

			m_DrawObjectVector.push_back(DrawObject);
		}
	};

	void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, int PenStyle )
	{
		long l_lCount = static_cast<long> (p_svMultiLine.m_svLineArray.size());

		for( long l = 0; l < l_lCount; l++ )
		{
			SVDrawObject DrawObject;

			DrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[l], PenStyle );

			m_DrawObjectVector.push_back(DrawObject);
		}
	};

	BOOL Draw( SVDrawContext* PDrawContext )
	{
		BOOL RetVal = TRUE;

		if( m_bDrawFigureHatched )
		{
			long iLastYPointDrawn = InvalidPoint; // force the first line to be drawn.
			int LastLineY = InvalidPoint;
			for( int i = 0; i < static_cast<int> (m_DrawObjectVector.size()); ++i )
			{
				if( PDrawContext->ShouldDrawPoints( iLastYPointDrawn, m_DrawObjectVector[i].GetPointAt(0).y  ) )
				{
					RetVal = m_DrawObjectVector[ i ].DrawHatch( PDrawContext, LastLineY ) && RetVal; 
				}
			}
		}
		else
		{
			for( int i = 0; i < static_cast<int> (m_DrawObjectVector.size()); ++i )
			{
				RetVal = m_DrawObjectVector[ i ].Draw( PDrawContext ) && RetVal; 
			}
		}
	
		return RetVal;
	};

	// This function will not delete the contents!
	// Embed your DrawObjects instead of construct them dynamically!
	void Flush()
	{
		m_DrawObjectVector.clear();
	};

	int GetSize() const
	{
		return static_cast<int> (m_DrawObjectVector.size());
	}

	SVDrawObject GetAt( int i ) const
	{
		return (0 < m_DrawObjectVector.size()) ? m_DrawObjectVector[i] : SVDrawObject();
	}

	const SVDrawObjectClassVector& getList() const { return m_DrawObjectVector; };

	bool m_bDrawFigureHatched;

protected:
	SVDrawObjectClassVector m_DrawObjectVector;
};
