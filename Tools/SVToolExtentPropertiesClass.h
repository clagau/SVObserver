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
#include "Definitions/SVExtentEnums.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "InspectionEngine/SVExtentPropertiesInfoStruct.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvTo
{

class SVToolExtentPropertiesClass
{
public:
	SVToolExtentPropertiesClass();
	~SVToolExtentPropertiesClass();

	HRESULT Initialize();

	HRESULT GetProperties( SVImageExtentClass &rExtents ) const;

	HRESULT GetExtentObject( SvPb::SVExtentPropertyEnum eProperty, SvOi::IValueObject *&rpValueObject ) const;
	void SetExtentObject( SvPb::SVExtentPropertyEnum eProperty, SvOi::IValueObject *pValueObject );

	HRESULT GetPropertyInfo( SvPb::SVExtentPropertyEnum eProperty, SvIe::SVExtentPropertyInfoStruct& rInfo ) const;
	HRESULT SetPropertyInfo( SvPb::SVExtentPropertyEnum eProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo );

private:
	struct SVExtentPropertyInfoStructImpl
	{
		SvOi::IValueObject*  pValueObject;
		SvIe::SVExtentPropertyInfoStruct info;
	};
	typedef std::map<SvPb::SVExtentPropertyEnum, SVExtentPropertyInfoStructImpl> SVToolExtentPropertiesMap;

	SVToolExtentPropertiesMap m_svProperties;
};

} //namespace SvIe
