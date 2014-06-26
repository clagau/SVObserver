//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : Doughnut
//* .File Name       : $Workfile:   Doughnut.h  $
//* .Description     : New DrawObject for Doughnut
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:26:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "EllipseObject.h"
#pragma endregion Includes

class Doughnut : public EllipseObject
{
public:
#pragma region Constructor
	Doughnut();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw( POINT p_dOffset, double p_dZoomWidth, double p_dZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(const VariantParamMap& ParameterMap) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;

	//************************************
	// Method:    CalculateInnerRect
	// Description:  Calculate the inner rectangle from rect and Thickmess
	// Returns:   void
	//************************************
	void CalculateInnerRect();

	//************************************
	// Method:    AdjustThickness
	// Description:  Add x respectivly y   and limit the TopThickness and SideThickness to reasonable value 
	// Returns:   void
	//************************************
	void AdjustThickness(long x = 0, long y = 0);
#pragma endregion Public Methods

#pragma region Member Variables
private:
	CRect m_InnerRect;
	long m_TwiceSideThickness; //2*thickness to avoid rounding error
	long m_TwiceTopThickness; //2*thickness to avoid rounding error
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\Doughnut.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:26:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */