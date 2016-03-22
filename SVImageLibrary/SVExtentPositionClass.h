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
	virtual ~SVExtentPositionClass();

	const SVExtentPositionClass &operator=( const SVExtentPositionClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual HRESULT DisableExtentProperty( SVExtentPropertyEnum p_eProperty );
	bool IsEnabled( SVExtentPropertyEnum p_eProperty ) const;

	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;

	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

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

