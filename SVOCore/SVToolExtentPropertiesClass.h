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
#include "SVExtentPropertiesInfoStruct.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

class SVToolExtentPropertiesClass
{
public:
	SVToolExtentPropertiesClass();
	~SVToolExtentPropertiesClass();

	HRESULT Initialize();

	HRESULT GetProperties( SVImageExtentClass &rExtents ) const;
	HRESULT GetProperties( unsigned long Index, SVImageExtentClass &rExtents ) const;

	HRESULT GetExtentObject( SVExtentPropertyEnum eProperty, SvOi::IValueObject *&rpValueObject ) const;
	HRESULT SetExtentObject( SVExtentPropertyEnum eProperty, SvOi::IValueObject *pValueObject );

	HRESULT GetPropertyInfo( SVExtentPropertyEnum eProperty, SVExtentPropertyInfoStruct& rInfo ) const;
	HRESULT SetPropertyInfo( SVExtentPropertyEnum eProperty, const SVExtentPropertyInfoStruct& rInfo );

private:
	struct SVExtentPropertyInfoStructImpl
	{
		SvOi::IValueObject*  pValueObject;
		SVExtentPropertyInfoStruct info;
	};
	typedef std::map< SVExtentPropertyEnum, SVExtentPropertyInfoStructImpl > SVToolExtentPropertiesMap;

	SVToolExtentPropertiesMap m_svProperties;
};

