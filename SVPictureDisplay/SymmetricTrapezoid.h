//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SymmetricTrapezoid
//* .File Name       : $Workfile:   SymmetricTrapezoid.h  $
//* .Description	   New DrawObject for SymmetericTrapazoid
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "drawobject.h"
#include "rectangleObject.h"
#include "SVPictureDisplayIdl.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class SymmetricTrapezoid : public RectangleObject
{
#pragma region Constructor
public:
	SymmetricTrapezoid();
	virtual ~SymmetricTrapezoid();

#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	int GetPoints(std::vector<POINT> &aPoints) const;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//************************************
	// Method:    NormOffset
	// Description:  limit the offset to reasonable maximum
	// Returns:   void
	//************************************
	void NormOffset();
#pragma endregion Protected Methods

#pragma region Member Variables
private:
	long m_TrapOffset;
	ROISubType_Trapezoid m_Subtype;
#pragma endregion Member Variables
};

