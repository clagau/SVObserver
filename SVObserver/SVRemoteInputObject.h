//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputObject
//* .File Name       : $Workfile:   SVRemoteInputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:48:00  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVInputObject.h"
#pragma endregion Includes

class SVRemoteInputObject : public SVInputObject 
{
public:

	SVRemoteInputObject( LPCSTR strObjectName );
	SVRemoteInputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTOBJECT );

	virtual ~SVRemoteInputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();
	virtual BOOL IsCreated();

	virtual HRESULT Read( _variant_t& p_rValue );

	BOOL Lock();
	BOOL Unlock();

	HRESULT WriteCache( const _variant_t& p_rValue );
	HRESULT GetCache( _variant_t& p_rValue );

	long				m_lIndex;
private:
	CRITICAL_SECTION	m_hCriticalSection;
	_variant_t			m_vtRemoteCache;
};
