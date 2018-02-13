//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentDimensionsClass
//* .File Name       : $Workfile:   SVExtentDimensionsClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:02  $
//******************************************************************************

#pragma once

#include "Definitions/SVExtentEnums.h"

class SVExtentDimensionsClass
{
public:
	SVExtentDimensionsClass();
	SVExtentDimensionsClass( const SVExtentDimensionsClass &p_rsvValue );
	~SVExtentDimensionsClass();

	const SVExtentDimensionsClass &operator=( const SVExtentDimensionsClass &p_rsvValue );

	HRESULT Initialize();

	HRESULT DisableExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty );
	bool IsEnabled( SvDef::SVExtentPropertyEnum p_eProperty ) const;

	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;

	HRESULT SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue );

	bool operator==( const SVExtentDimensionsClass &p_rsvValue ) const;
	bool operator!=( const SVExtentDimensionsClass &p_rsvValue ) const;

private:
//	currently enabled properties
	unsigned long m_ulProperties;

	double m_dWidth;
	double m_dHeight;
	double m_dStartAngle;
	double m_dEndAngle;
	double m_dInnerRadius;
	double m_dOuterRadius;

	/// m_dWidthScaleFactor - indicates the scale factor to be used size the 
	///  output buffer width from the input image ROI width. Keep in mind that the
	///  input image context is the input to the current task object.
	double m_dWidthScaleFactor;

	/// m_dHeightScaleFactor - indicates the scale factor to be used size the 
	///  output buffer height from the input image ROI height. Keep in mind that the
	///  input image context is the input to the current task object.
	double m_dHeightScaleFactor;

	long m_lOutputWidth;
	long m_lOutputHeight;
};

