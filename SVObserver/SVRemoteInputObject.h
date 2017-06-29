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
#pragma region Constructor
public:
	SVRemoteInputObject( LPCSTR strObjectName );
	SVRemoteInputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTOBJECT );

	virtual ~SVRemoteInputObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL Create() override;
	virtual BOOL Destroy() override;

	virtual HRESULT Read( _variant_t& p_rValue ) override;

	BOOL Lock();
	BOOL Unlock();

	HRESULT WriteCache( const _variant_t& p_rValue );
	HRESULT GetCache( _variant_t& p_rValue );
	/// Update the GUID to a fix GUID depend of a position (must between 0 and 0x100).
	/// \param position [in]
	void updateGuid(int position);

	long m_lIndex;
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	CRITICAL_SECTION m_hCriticalSection;
	_variant_t m_vtRemoteCache;
#pragma endregion Member Variables
};
