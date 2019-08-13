//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShape
//* .File Name       : $Workfile:   SVMaskShape.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 17:49:54  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <string>
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVImageLibrary\SVImageInfoClass.h"
#pragma endregion Includes

namespace SvOp
{

struct SVMaskFillPropertiesStruct
{
	bool bMaskInsideShape;
	bool bFillMaskArea;
	COLORREF rgbFillColor;

	SVMaskFillPropertiesStruct()
	{
		bMaskInsideShape = false;
		bFillMaskArea = true;
		rgbFillColor = RGB(0,0,0);
	}
};

struct SVMaskShapeProperty
{
	long value {0};
	int order {0};
	bool bAvailableWithAutoResize {false};
	std::string strName;
	SVMaskShapeProperty() = default;
	SVMaskShapeProperty(const SVMaskShapeProperty& rhs) = default;
	SVMaskShapeProperty& operator = (const SVMaskShapeProperty& rhs)
	{
		if ( this != &rhs )
		{
			value = rhs.value;
			assert( rhs.order != 0 );
			if ( order == 0 || rhs.order != 0 )
				order = rhs.order;
			bAvailableWithAutoResize = rhs.bAvailableWithAutoResize;
			strName = rhs.strName;
		}
		return *this;
	}
	// allow easy value assignment
	SVMaskShapeProperty& operator = (long l)
	{
		value = l;
		return *this;
	}

	inline friend bool operator < (const SVMaskShapeProperty& a, const SVMaskShapeProperty& b)
	{
		return a.order < b.order;
	}
};

class SVMaskShape
{
public:
	typedef std::map<GUID, SVMaskShapeProperty> MapType;
	typedef MapType::const_iterator MapTypeConstIterator;
	struct SortedMapCompare {bool operator ()(const MapTypeConstIterator& a, const MapTypeConstIterator& b)
	{return a->second < b->second;} };
	typedef std::multiset<MapTypeConstIterator, SortedMapCompare> SortedMapViewType;
	static SortedMapViewType GetTempSortedPropertyMapView(const MapType& map);

	virtual ~SVMaskShape();	// this is a base class

	HRESULT Draw(HDC hDC) const;
	HRESULT DrawOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const;
	HRESULT Draw( SVMatroxBuffer mil );
	HRESULT Refresh();
	HRESULT SetProperties(const MapType& mapProperties);
	HRESULT GetProperties(MapType& rmapProperties) const;

	HRESULT SetAutoResize( bool bAutoResize );
	bool    IsAutoResize() const;
	HRESULT SetImageInfo( const SVImageInfoClass& svImageInfo );
	HRESULT GetImageInfo( SVImageInfoClass& svImageInfo ) const;
	HRESULT SetFillProperties( SVMaskFillPropertiesStruct svFillStruct );
	HRESULT GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct ) const;

	RECT GetMaskImageRect() const;
	RECT GetRect() const;
	SIZE GetSize() const;
	POINT GetCenter() const;

protected:
	SVMaskShape();	// don't create instances of base class

	virtual HRESULT Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const = 0;
	virtual HRESULT RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const {return S_FALSE; }
	virtual HRESULT ValidateProperties(MapType& rmapProperties) const;

	HRESULT TranslateToDisplay(RECT rectViewport, RECT rectDisplay, RECT& rShapeRect) const;
	HRESULT TranslateToDisplay(RECT rectViewport, RECT rectDisplay, std::vector<POINT>& rvecPoints) const;

	SVDIBITMAPINFO m_dib;
	HDC m_RenderDC {nullptr};
	MapType m_mapProperties;

	SVMaskFillPropertiesStruct m_svFillStruct;
	SVImageInfoClass m_svImageInfo;

	bool m_bImageInfoChanged {false};
	bool m_bImageInfoSet {false};
	bool m_bAutoResize {false};

private:
	void ComputeColors(COLORREF& rgbShape, COLORREF& rgbBackground); //@TODO:  Change method to const?

	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect);
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVPoint<double>& rPoint);
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, std::vector<SVPoint<double>>& rvecPoints);
};

class SVMaskShapeRectangle : public SVMaskShape
{
public:
	SVMaskShapeRectangle() = default;
	virtual ~SVMaskShapeRectangle() = default;

protected:
	virtual HRESULT Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const override;
	virtual HRESULT RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb ) const override;
};

class SVMaskShapeOval : public SVMaskShape
{
public:
	SVMaskShapeOval() = default;
	virtual ~SVMaskShapeOval() = default;

protected:
	virtual HRESULT Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const override;
	virtual HRESULT RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const override;
};

class SVMaskShapeSymmetricTrapezoid : public SVMaskShape
{
public:
	SVMaskShapeSymmetricTrapezoid();
	virtual ~SVMaskShapeSymmetricTrapezoid() = default;

	// future reference: rotation range will be (-180, 180]
	enum SymmetryOrientation
	{
		VerticalAxisTop      = 0,
		VerticalAxisBottom   = 180,
		HorizontalAxisLeft   = -90,
		HorizontalAxisRight  = 90,
	};
protected:
	virtual HRESULT Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const override;
	virtual HRESULT RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const override;
	virtual HRESULT ValidateProperties(MapType& rmapProperties) const override;
private:
	std::vector<POINT> GetPoints() const;
};

class SVMaskShapeDoughnut : public SVMaskShape
{
public:
	SVMaskShapeDoughnut();
	virtual ~SVMaskShapeDoughnut() = default;

	// the doughnut is the only shape so far that cares about the rgbBackground
protected:
	virtual HRESULT Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const override;
	virtual HRESULT RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const override;
	virtual HRESULT ValidateProperties(MapType& rmapProperties) const override;
};

} //namespace SvIe
