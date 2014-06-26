//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : GraphObject
//* .File Name       : $Workfile:   GraphObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "SVPictureDisplayidl.h"
#pragma endregion Includes

class GraphObject : public DrawObject
{
#pragma region Constructor
public:
	GraphObject();
	virtual ~GraphObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(const VariantParamMap& ParameterMap) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//************************************
	// Method:    setPoints
	// Description: Set the points and recalculate the border rectangle. It delete the old points and set only the new points.
	// Parameter: _variant_t variantPoints Must be a array of long (VT_I4) or double (VT_R8) in the order x1,y1,x2,y2...
	// Returns:   void
	//************************************
	void setPoints( const _variant_t& variantPoints );

	//************************************
	// Method:    getPointsAsVariant
	// Description: Get the current points in a safeArray.
	// Returns:   _variant_t Return a safeArray of longs (VT_I4) in the order  x1,y1,x2,y2...
	//************************************
	_variant_t getPointsAsVariant() const;

	void calcRect();
	void calcFullViewPointsForDrawing( const CRect valueRect, POINT* const & points, size_t sizePoints );
#pragma endregion

#pragma region Member Variables
protected:
	CRect m_borderRect;
	std::vector<CPoint> m_points;
	ROISubType_Graph m_subType;
	long m_min_x;
	long m_max_x;
	long m_min_y;
	long m_max_y;
	bool m_isVerticalFlip;
	bool m_isHorizontalFlip;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\GraphObject.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:26:40   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jun 2010 11:58:50   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  8
 * SCR Title:  Font Training
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated Picture Control to be able to used a different color for selected objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 May 2010 07:06:48   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed move functions to return true if they can be resized or moved. There is now a limit on minimum size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:24:06   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/