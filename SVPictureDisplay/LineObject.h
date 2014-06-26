//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LineObject
//* .File Name       : $Workfile:   LineObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#pragma endregion Includes

class LineObject : public DrawObject
{
public:
#pragma region Constructor
	LineObject();
	LineObject( long x1, long y1, long x2, long y2, COLORREF color, long lAllowEdit );
	virtual ~LineObject();
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT SetLine(long x1, long y1, long x2, long y2);
	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual void GetLine( POINT& p_rStart, POINT& p_rEnd ) const;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Member Variables
protected:
	POINT m_StartPos;
	POINT m_EndPos;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\LineObject.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:26:42   mziegler
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