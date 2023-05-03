//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOutputObject.h"
#pragma endregion Includes

class SVDigitalOutputObject : public SVOutputObject
{
#pragma region Constructor
public:
	explicit SVDigitalOutputObject( LPCSTR strObjectName );
	explicit SVDigitalOutputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALOUTPUTOBJECT );

	virtual ~SVDigitalOutputObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;

	virtual HRESULT Write( const _variant_t& rValue ) override;
	virtual HRESULT Reset() override;
	virtual bool isCombined() const override;
	virtual bool isAndACK() const override;
	virtual long GetChannel() const override { return m_channel;}
	virtual void SetValueObjectID(uint32_t objectID, DWORD objectIDIndex = 0) override;
	virtual uint32_t GetValueObjectID(DWORD objectIDIndex = 0) const override;

	bool Force( bool bForce, bool bForcedValue );
	bool Invert( bool bInvert );
	void Combine( bool combined, bool isAndACK );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool GetValue() const;

	void SetChannel( long channel ) { m_channel = channel; }

#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	long m_channel {-1L};
	bool m_bCombined {false};
	bool m_bLastValue {false};
	bool m_bDefaultValue {false};
	bool m_isAndACK {true};
	uint32_t m_valueObjectID {SvDef::InvalidObjectId};
#pragma endregion Member Variables
};
