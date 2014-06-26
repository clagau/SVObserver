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
#include <map>
#include <set>
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVImageLibrary\SVImageInfoClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

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
	long value;
	int order;
	bool bAvailableWithAutoResize;
	CString strName;
	SVMaskShapeProperty()
	{
		value = 0;
		order = 0;
		bAvailableWithAutoResize = false;
	}
	SVMaskShapeProperty(const SVMaskShapeProperty& rhs)
	{
		value = rhs.value;
		order = rhs.order;
		bAvailableWithAutoResize = rhs.bAvailableWithAutoResize;
		strName = rhs.strName;
	}
	SVMaskShapeProperty& operator = (const SVMaskShapeProperty& rhs)
	{
		if ( this != &rhs )
		{
			value = rhs.value;
			ASSERT( rhs.order != 0 );
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

	HRESULT Draw( HDC hDC ) const;
	HRESULT DrawOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb );
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

	CRect GetMaskImageRect(); //@TODO:  Change method to const?
	CRect GetRect(); //@TODO:  Change method to const?
	CSize GetSize(); //@TODO:  Change method to const?
	CPoint GetCenter(); //@TODO:  Change method to const?

protected:
	SVMaskShape();	// don't create instances of base class

	virtual HRESULT Render( CDC& dc, COLORREF rgbShape, COLORREF rgbBackground ) = 0;
	virtual HRESULT RenderOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb ) {return S_FALSE; }
	virtual HRESULT ValidateProperties(MapType& rmapProperties);

	HRESULT TranslateToDisplay(CRect rectViewport, CRect rectDisplay, CRect& rectShape); //@TODO:  Change method to const?
	HRESULT TranslateToDisplay(CRect rectViewport, CRect rectDisplay, std::vector<POINT>& rvecPoints); //@TODO:  Change method to const?

	SVDIBITMAPINFO m_dib;
	CDC m_RenderDC;
	MapType m_mapProperties;

	SVMaskFillPropertiesStruct m_svFillStruct;
	SVImageInfoClass m_svImageInfo;

	bool m_bImageInfoChanged;
	bool m_bImageInfoSet;
	bool m_bAutoResize;

private:
	void ComputeColors(COLORREF& rgbShape, COLORREF& rgbBackground); //@TODO:  Change method to const?

	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect); //@TODO:  Change method to const?
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentPointStruct& rPoint); //@TODO:  Change method to const?
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, std::vector<SVExtentPointStruct>& rvecPoints);
};

class SVMaskShapeRectangle : public SVMaskShape
{
public:
	SVMaskShapeRectangle();
protected:
	virtual HRESULT Render( CDC& dc, COLORREF rgbShape, COLORREF );
	virtual HRESULT RenderOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb );
};

class SVMaskShapeOval : public SVMaskShape
{
public:
	SVMaskShapeOval();
protected:
	virtual HRESULT Render( CDC& dc, COLORREF rgbShape, COLORREF );
	virtual HRESULT RenderOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb );
};

class SVMaskShapeSymmetricTrapezoid : public SVMaskShape
{
public:
	SVMaskShapeSymmetricTrapezoid();

	// future reference: rotation range will be (-180, 180]
	enum SymmetryOrientation
	{
		VerticalAxisTop      = 0,
		VerticalAxisBottom   = 180,
		HorizontalAxisLeft   = -90,
		HorizontalAxisRight  = 90,
	};
protected:
	virtual HRESULT Render( CDC& dc, COLORREF rgbShape, COLORREF );
	virtual HRESULT RenderOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb );
	virtual HRESULT ValidateProperties(MapType& rmapProperties);
private:
	HRESULT GetPoints(std::vector<POINT>& rvecPoints);
};

class SVMaskShapeDoughnut : public SVMaskShape
{
public:
	SVMaskShapeDoughnut();
	// the doughnut is the only shape so far that cares about the rgbBackground
protected:
	virtual HRESULT Render( CDC& dc, COLORREF rgbShape, COLORREF rgbBackground );
	virtual HRESULT RenderOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb );
	virtual HRESULT ValidateProperties(MapType& rmapProperties); //@TODO:  Change method to const?
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaskShape.h_v  $
 * 
 *    Rev 1.2   26 Jun 2014 17:49:54   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   move SVColor to PictureDisplay.h
 * cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jul 2013 17:56:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std map
 * Added include for std set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:27:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jan 2011 11:33:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Oct 2010 14:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 14:27:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Apr 2006 15:58:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented AvailableWithAutoResize for shape properties
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Mar 2006 09:20:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Auto Resize capability to the Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:06:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for self assignment in SVMaskShapeProperty
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:28:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/