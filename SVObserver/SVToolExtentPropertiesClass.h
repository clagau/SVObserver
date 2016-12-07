//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentPropertiesClass
//* .File Name       : $Workfile:   SVToolExtentPropertiesClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:44:06  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVImageLibrary/SVExtentEnums.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOCore/SVExtentPropertiesInfoStruct.h"
#pragma endregion Includes

class SVValueObjectClass;

class SVToolExtentPropertiesClass
{
public:
	SVToolExtentPropertiesClass();
	~SVToolExtentPropertiesClass();

	HRESULT Initialize();

	HRESULT GetProperties( SVImageExtentClass &p_rsvExtents ) const;
	HRESULT GetProperties( unsigned long p_ulIndex, SVImageExtentClass &p_rsvExtents ) const;

	HRESULT GetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue ) const;
	HRESULT SetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue );

	HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;
	HRESULT SetPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

private:
	struct SVExtentPropertyInfoStructImpl
	{
		SVValueObjectClass*  pValueObject;
		SVExtentPropertyInfoStruct info;
	};
	typedef std::map< SVExtentPropertyEnum, SVExtentPropertyInfoStructImpl > SVToolExtentPropertiesMap;

	SVToolExtentPropertiesMap m_svProperties;
};

