//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;
	///PLC output does not need to write any value
	virtual HRESULT Write( const _variant_t&) override { return S_OK; }
	virtual HRESULT Reset() override { return S_OK;}
	virtual bool isCombined() const override {return m_isCombined; }
	virtual bool isAndACK() const override { return m_isAndACK; }
	virtual void SetValueObjectID(uint32_t objectID, DWORD objectIDIndex = 0) override;
	virtual uint32_t GetValueObjectID(DWORD objectIDIndex = 0) const override;

	void Combine( bool bCombine, bool bCombineACK );
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	bool m_isCombined {false};
	bool m_isAndACK {true};
	std::array<uint32_t, SvDef::cObjectIndexMaxNr> m_valueObjectIDList {0, 0, 0, 0};
#pragma endregion Member Variables
};
