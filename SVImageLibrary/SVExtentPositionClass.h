//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPositionClass
//* .File Name       : $Workfile:   SVExtentPositionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:38  $
//******************************************************************************

#pragma once

#include "SVExtentEnums.h"
#include "SVExtentPointStruct.h"

class SVExtentPositionClass
{
public:
	SVExtentPositionClass();
	SVExtentPositionClass( const SVExtentPositionClass &p_rsvValue );
	~SVExtentPositionClass();

	const SVExtentPositionClass &operator=( const SVExtentPositionClass &p_rsvValue );

	HRESULT Initialize();

	HRESULT DisableExtentProperty( SVExtentPropertyEnum p_eProperty );
	bool IsEnabled( SVExtentPropertyEnum p_eProperty ) const;

	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;

	HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	bool operator==( const SVExtentPositionClass &p_rsvValue ) const;
	bool operator!=( const SVExtentPositionClass &p_rsvValue ) const;

private:
	unsigned long m_ulProperties;

	// New Extent Data
	double m_dRotationAngle;

	SVExtentPointStruct m_svPositionPoint;
	SVExtentPointStruct m_svPositionPointEndOfLine;
	SVExtentPointStruct m_svTranslationOffset;
	SVExtentPointStruct m_svOutputPositionPoint;

	// Old Extent Data
	double m_dOldRotationAngle;

	SVExtentPointStruct m_svOldPositionPoint;
	SVExtentPointStruct m_svOldAbsPositionPoint;
	SVExtentPointStruct m_svOldRotationPoint;
	SVExtentPointStruct m_svOldTranslationOffset;
};

