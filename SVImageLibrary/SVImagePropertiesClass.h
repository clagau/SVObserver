//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePropertiesClass
//* .File Name       : $Workfile:   SVImagePropertiesClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:26  $
//******************************************************************************

#pragma once

#include "Definitions/SVImageEnums.h"

class SVImagePropertiesClass
{
public:
	SVImagePropertiesClass();
	SVImagePropertiesClass( const SVImagePropertiesClass &p_rsvValue );
	~SVImagePropertiesClass();

	const SVImagePropertiesClass &operator=( const SVImagePropertiesClass &p_rsvValue );

	HRESULT Initialize();

	HRESULT DisableImageProperty( SvDef::SVImagePropertyEnum p_eProperty );

	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, int &p_riValue ) const;
	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long &p_rlValue ) const;

	HRESULT SetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long p_lValue );

	bool operator==( const SVImagePropertiesClass &p_rsvValue ) const;
	bool operator!=( const SVImagePropertiesClass &p_rsvValue ) const;

private:

	unsigned long m_ulProperties;

	int m_iFormat;
	int m_iPixelDepth;
	int m_iBandNumber;
	int m_iBandLink;
};

