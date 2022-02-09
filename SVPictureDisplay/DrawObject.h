//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : DrawObject
//* .File Name       : $Workfile:   DrawObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:38  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

#pragma region Declarations
// HTTYPE is a DWORD lowByte are the defines in Winuser.h from HTNOWHERE to HTOBJECT
//high Byte are HTX_INNER_EDGE,  HTX_STOP_EDGE, HTX_POINT

typedef DWORD HTTYPE;
static const DWORD HTSELECTED = HTOBJECT + 10;
static const DWORD HTX_INNER_EDGE = 0x200;
static const DWORD HTX_STOP_EDGE = 0x400;

static const DWORD HTX_POINT = 0x800; //Which point is grabbed, are set by the lower bit.  (e.g. 0x801 == point[1] are used)
static const DWORD HTX_POINT_MOVE = 0x900; //Which point is grabbed, are set by the lower bit.  (e.g. 0x901 == point[1] are used)
static const DWORD HTX_ALL_FLAG = 0xF00; //For all High-value flags.

#define GRAB_SIZE 8
#define MIN_SIZE 4

class DrawObject;
typedef std::shared_ptr<DrawObject> DrawObjectRef;
typedef std::map<long, DrawObjectRef> DrawObjectList;
typedef std::map< long, long > LongParamMap;
typedef std::map< long, _variant_t > VariantParamMap;
#pragma endregion Declarations

class DrawObject
{
public:
#pragma region Constructor
	DrawObject();
	virtual ~DrawObject();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) = 0;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const = 0;
	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint) = 0;

	//************************************
	// Method:    SetParameter
	// Description: Set all parameter from the map one by one. 
	//				This method do not be needed to override by the subclass.
	//				Only if you want to call a method after (or before) setting all parameter, override the methods.
	//				If an unknown parameterID in the map, it will called a ASSERT.
	// Parameter: const VariantParamMap& ParameterMap: <ParameterId, ParameterValue>
	// Returns:   void
	//************************************
	virtual void SetParameter(const VariantParamMap& ParameterMap);

	//************************************
	// Method:    SetParameter
	// Description: Set a parameter. This method HAS to be override by the subclass.
	//				Every subclass should handle this own parameter and if the parameterID unknown it should call the parent-class.
	// Parameter: long parameterId
	// Parameter: _variant_t parameterValue
	// Returns:   void
	//************************************
	virtual void SetParameter(long parameterId, _variant_t parameterValue) = 0;

	//************************************
	// Method:    GetParameter
	// Description: Add the parameter to a map. This method HAS to be override by the subclass.
	//				Every subclass should first call the parent-class and then add this own parameter.
	// Parameter: VariantParamMap& ParameterMap the map with the new parameter.
	// Returns:   void
	//************************************
	virtual void GetParameter(VariantParamMap& ParameterMap) const = 0;
	virtual RECT GetRectangle() const = 0;

	HRESULT SetColor(COLORREF color);
	HRESULT SetEditAllowed( long p_lAllowed );
	long GetEditAllowed() const;
	long AddDrawObjectChild( long p_lHandle, DrawObjectRef p_DrawObject );
	const DrawObjectList& GetChildList() const;
	HRESULT GetBoundingRect(RECT& rec) const;
	bool RemoveChild( long Handle );

	static CSize GetMinSize();

	//************************************
	// Method:    GetHotSpotSize
	// Description:  Hot spot size depends on zoom settings
	// Returns:   CSize
	//************************************
	CSize GetHotSpotSize() const;

	//************************************
	// Method:    GetSelectionType
	// Description: Calculate Selection Type from the angle between center and Point
	// Parameter: const POINT & center
	// Parameter: const POINT & point
	// Returns:   HTTYPE Selection Type 
	//************************************
	HTTYPE GetSelectionType( const POINT &center, const POINT &point ) const;

	//************************************
	// Method:    TranslatePoints
	// Description:  Calculates zooming and offset for an vector of points
	// Parameter: POINT p_Offset
	// Parameter: float p_fZoomWidth
	// Parameter: float p_fZoomHeight
	// Parameter: std::vector<POINT> & aPoints
	// Returns:   void
	//************************************
	static void TranslatePoints(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, std::vector<POINT> &aPoints);

	//************************************
	// Method:    TranslateRect
	// Description: Calculates zooming and offset for a rect 
	// Parameter: POINT p_Offset
	// Parameter: POINT p_Offset
	// Parameter: float p_fZoomWidth
	// Parameter: float p_fZoomHeight
	// Parameter: RECT & rect
	// Returns:   void
	//************************************
	void TranslateRect(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CRect &rect) const;

	//************************************
	// Method:    GetHotSpot
	// Description:  Builds a region with the point in the center
	// Parameter: CPoint pt
	// Returns:   std::unique_ptr<CRgn>
	//************************************
	std::unique_ptr<CRgn> GetHotSpot( CPoint pt ) const;

	//************************************
	// Method:    GetHotSpot
	// Description:  Builds a region Between pt1 and pt2 
	// Parameter: CPoint pt1
	// Parameter: CPoint pt2
	// Returns:   std::unique_ptr<CRgn>
	//************************************
	std::unique_ptr<CRgn> GetHotSpot( CPoint pt1, CPoint pt2 ) const;

	void setImageSize(CSize val);
	void setViewSize( CSize val );
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void DrawChildren( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected );
	bool IsValidChildObjectAtPoint( HTTYPE& p_rType, const CPoint& imagePoint, const CPoint& viewPoint ) const;
	bool MoveChild(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint);

	static bool isPointOnLine(CPoint p1, CPoint p2, CPoint imagePoint);
#pragma endregion Protected Methods

#pragma region protected Member variables
protected:
	COLORREF m_color;
	COLORREF m_SelectedColor;
	int m_PenWidth;
	//remember Zoom factor to adjust Hotspot size
	double m_zoomWidth;
	double m_zoomHeight;
	CSize m_imageSize;
	CSize m_viewSize;
#pragma endregion protected Member variables

#pragma region static Member variables
	//Size of Hotpot i.e region where Overlay can be selected.
	const static int HotSpotSizeX = 4;
	const static int HotSpotSizeY = 4;
	const static int MinSizeX = 4;
	const static int MinSizeY = 4;
#pragma endregion static Member variables

#pragma region private Member variables
private:
	long m_lEditAllowed;
	DrawObjectList m_ChildDrawObjects;
#pragma endregion private Member variables
};

