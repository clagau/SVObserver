//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputObject
//* .File Name       : $Workfile:   SVRemoteInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:20:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRemoteInputObject.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

SVRemoteInputObject::SVRemoteInputObject( LPCSTR strObjectName )
: SVInputObject( strObjectName )
{
	LocalInitialize();
}

SVRemoteInputObject::SVRemoteInputObject( SVObjectClass* POwner, int StringResourceID )
: SVInputObject( POwner, StringResourceID )
{
	LocalInitialize();
}

HRESULT SVRemoteInputObject::Read( _variant_t& rValue ) const
{
	rValue = getCache();
	return S_OK;
}

HRESULT SVRemoteInputObject::writeCache( const _variant_t& rValue )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Temp = 0.0;

	if( rValue.vt & VT_ARRAY )
	{
		SvUl::SVSAFEARRAY l_SafeArray( rValue );

		if ( 1 == l_SafeArray.size() )
		{
			l_Status = l_SafeArray.GetElement( 0, l_Temp );
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Temp = rValue;
	}

	if( S_OK == l_Status )
	{
		if (VT_R8 != l_Temp.vt)
		{
			//Remote inputs are now of type VT_R8
			l_Temp.ChangeType(VT_R8);
		}
		std::lock_guard<std::mutex> guard(m_protectRemoteInput);
		m_remoteCache = l_Temp;
	}

	return l_Status;
}

_variant_t SVRemoteInputObject::getCache() const
{
	std::lock_guard<std::mutex> guard(m_protectRemoteInput);
	return m_remoteCache;
}

void SVRemoteInputObject::LocalInitialize()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVRemoteInputObjectType;
	m_startID = ObjectIdEnum::RemoteInputId;
}
