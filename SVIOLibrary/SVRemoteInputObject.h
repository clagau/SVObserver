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
#include "SVOResource/resource.h"
#pragma endregion Includes

class SVRemoteInputObject : public SVInputObject 
{
#pragma region Constructor
public:
	explicit SVRemoteInputObject( LPCSTR strObjectName );
	explicit SVRemoteInputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTOBJECT );

	virtual ~SVRemoteInputObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:

	virtual HRESULT Read(_variant_t& rValue) const override;
	virtual long GetChannel() const override { return m_channel; }

	HRESULT writeCache(const _variant_t& rValue);

	void SetChannel(long channel) { m_channel = channel; }
#pragma endregion Public Methods

#pragma region Private Methods
private:
	_variant_t getCache() const;
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	mutable std::mutex m_protectRemoteInput;

	_variant_t m_remoteCache{0.0};
	long m_channel{-1};
#pragma endregion Member Variables
};
