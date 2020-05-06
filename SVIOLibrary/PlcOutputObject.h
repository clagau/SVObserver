//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file PlcOutputObject.h
/// \brief Output object for PLC connection
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOutputObject.h"
#pragma endregion Includes

class PlcOutputObject : public SVOutputObject
{
#pragma region Constructor
public:
	explicit PlcOutputObject( LPCSTR strObjectName );
	explicit PlcOutputObject( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_PLCOUTPUTOBJECT);

	virtual ~PlcOutputObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	///PLC output does not need to write any value
	virtual HRESULT Write( const _variant_t& rValue ) override { return S_OK; }
	virtual HRESULT Reset() override { return S_OK;}
	virtual bool isCombined() const override {return m_isCombined; }
	virtual bool isAndACK() const override { return m_isAndACK; }
	virtual long GetChannel() const override {return m_Channel; }

	void Combine( bool bCombine, bool bCombineACK );

	void SetChannel(long channel) { m_Channel = channel; }

#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	long m_Channel {-1L};
	bool m_isCombined {false};
	bool m_isAndACK {true};
#pragma endregion Member Variables
};
