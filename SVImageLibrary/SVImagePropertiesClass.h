//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePropertiesClass
//* .File Name       : $Workfile:   SVImagePropertiesClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:26  $
//******************************************************************************

#ifndef SVIMAGEPROPERTIESCLASS_H
#define SVIMAGEPROPERTIESCLASS_H

#include "ObjectInterfaces\SVImageEnums.h"

class SVImagePropertiesClass
{
public:
	SVImagePropertiesClass();
	SVImagePropertiesClass( const SVImagePropertiesClass &p_rsvValue );
	virtual ~SVImagePropertiesClass();

	const SVImagePropertiesClass &operator=( const SVImagePropertiesClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual HRESULT DisableImageProperty( SVImagePropertyEnum p_eProperty );

	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eProperty, int &p_riValue ) const;
	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eProperty, long &p_rlValue ) const;

	virtual HRESULT SetImageProperty( SVImagePropertyEnum p_eProperty, long p_lValue );

	bool operator==( const SVImagePropertiesClass &p_rsvValue ) const;
	bool operator!=( const SVImagePropertiesClass &p_rsvValue ) const;

private:

	unsigned long m_ulProperties;

	int m_iFormat;
	int m_iPixelDepth;
	int m_iBandNumber;
	int m_iBandLink;
};

#endif

