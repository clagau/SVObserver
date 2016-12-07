//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVExtentFigureStruct.h"
#include "SVImageLibrary/SVExtentLineStruct.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVImageLibrary/SVDrawContext.h"
#pragma endregion Includes

typedef SVVector< CPoint > SVCPointArray;

enum 
{
	InvalidPoint = INT_MIN,
};

class SVDrawObjectClass
{
public:
	SVDrawObjectClass();
	SVDrawObjectClass( const SVDrawObjectClass &p_rsvObject );
	~SVDrawObjectClass();

	const SVDrawObjectClass &operator=( const SVDrawObjectClass &p_rsvObject );

	void AddExtentLineData( SVExtentLineStruct p_svLine, int PenStyle = PS_SOLID );

	BOOL Draw( SVDrawContext* PDrawContext );
	BOOL DrawHatch( SVDrawContext* PDrawContext, int& LastY );

	// If BUseThisPen == FALSE, the current selected Pen 
	// of the DC will be used to Draw.
	BOOL SetDrawPen( BOOL BUseThisPen = FALSE, int PenStyle = PS_SOLID, int PenWidth = 1, COLORREF PenColor = RGB( 255, 255, 255 ) );

	void Transform( SVDrawContext* PDrawContext );

	int AddPoint( const POINT p_oPoint );
	int AddPoint( const CPoint& RPoint );
	void SetPointAtGrow( int Index, const CPoint& RPoint );
	void SetPointAtGrow( int Index, POINT p_oPoint );
	void SetListSize( int NewSize );
 
	CPoint GetPointAt( int Index );

	const SVCPointArray& GetPointArray();
	
	HGDIOBJ GetDrawPen();

protected:
	BOOL beginDraw( SVDrawContext* PDrawContext );
	void endDraw( HDC DC );
	
	SVCPointArray	 points;
	SVCPointArray	 calcPoints;

	HGDIOBJ drawPen;	// If nullptr the current selected pen of the DC
						// will be used!

	HGDIOBJ oldPen;

	BOOL m_BUseThisPen;
	int m_PenStyle;
	int m_PenWidth;
	COLORREF m_PenColor;
};

typedef SVVector< SVDrawObjectClass > SVDrawObjectClassArray;


/////////////////////////////////////////////////////////////////////////////
// SVPolyLineDrawObjectClass 

class SVDrawObjectListClass
{
public:
	SVDrawObjectListClass() {m_bDrawFigureHatched = false;};
	
	~SVDrawObjectListClass() {};

	void Add( SVDrawObjectListClass &p_rsvObject )
	{
		m_svDrawObjectArray.Append( p_rsvObject.m_svDrawObjectArray );
	};

	void Add( SVDrawObjectClass &p_rsvObject )
	{
		m_svDrawObjectArray.Add( p_rsvObject );// only adds SVDrawObjectClass data and does not add array data
	};

	void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, bool p_bDoNotUseLineColor = false )
	{
		long l_lCount = static_cast< long >( p_svMultiLine.m_svLineArray.GetSize() );

		for( long l = 0; l < l_lCount; l++ )
		{
			SVDrawObjectClass l_svDrawObject;

			l_svDrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[ l ] );

			if( p_bDoNotUseLineColor )
			{
				l_svDrawObject.SetDrawPen( FALSE );
			}

			m_svDrawObjectArray.Add( l_svDrawObject );
		}
	};

	void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, int PenStyle )
	{
		long l_lCount = static_cast< long >( p_svMultiLine.m_svLineArray.GetSize() );

		for( long l = 0; l < l_lCount; l++ )
		{
			SVDrawObjectClass l_svDrawObject;

			l_svDrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[ l ], PenStyle );

			m_svDrawObjectArray.Add( l_svDrawObject );
		}
	};

	BOOL Draw( SVDrawContext* PDrawContext )
	{
		BOOL RetVal = TRUE;

		if( m_bDrawFigureHatched )
		{
			long iLastYPointDrawn = InvalidPoint; // force the first line to be drawn.
			int LastLineY = InvalidPoint;
			for( int i = 0; i < m_svDrawObjectArray.GetSize(); ++i )
			{
				if( PDrawContext->ShouldDrawPoints( iLastYPointDrawn, m_svDrawObjectArray[i].GetPointAt(0).y  ) )
				{
					RetVal = m_svDrawObjectArray[ i ].DrawHatch( PDrawContext, LastLineY ) && RetVal; 
				}
			}
		}
		else
		{
			for( int i = 0; i < m_svDrawObjectArray.GetSize(); ++i )
			{
				RetVal = m_svDrawObjectArray[ i ].Draw( PDrawContext ) && RetVal; 
			}
		}
	
		return RetVal;
	};

	// This function will not delete the contents!
	// Embed your DrawObjects instead of construct them dynamically!
	void Flush()
	{
		m_svDrawObjectArray.RemoveAll();
	};

	int GetSize() const
	{
		return static_cast< int >( m_svDrawObjectArray.GetSize() );
	}

	SVDrawObjectClass GetAt( int i ) const
	{
		return m_svDrawObjectArray.GetAt( i );
	}
	bool m_bDrawFigureHatched;

protected:
	SVDrawObjectClassArray m_svDrawObjectArray;
};
